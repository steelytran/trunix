/*
 * processes and threads
 * Copyright (C) 2026  spenna
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <sys/queue.h>
#include <sys/cdefs.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

#include "trunix.h"
#include "proc.h"
#include "vm.h"
#include "file.h"

#define STACK_SIZE 0x1000
#define KSTACK_SIZE 0x1000
#define ELFMAGIC 0x464C457F

#define GETMEMMAP(x) (&memtab[(x)->pid - 1])

extern u32 *kpagedir;
extern struct task_state_segment tss;
extern struct kinfo k;

struct mem_map {
	u32 va;
	u32 pa;
	u32 len;
	SLIST_ENTRY(mem_map) entry;
};

struct proc_mmap {
        struct mem_map *slh_first;
	u32 sz;
};

static struct proc_mmap *memtab;

static struct proc *curthread;
static struct proc *idle;

struct elfhdr {
	u32 magic;
	u8 elf[12];
	u16 type;
	u16 machine;
	u32 version;
	u32 entry;
	u32 ph_off;
	u32 sh_off;
	u32 flags;
	u16 hdrsz;
	u16 ph_entsz;
	u16 ph_n;
	u16 sh_entsz;
	u16 sh_n;
	u16 sh_strndx;
};

struct proghdr {
	u32 type;
	u32 off;
	u32 va;
	u32 pa;
	u32 filesz;
	u32 memsz;
	u32 flags;
	u32 align;
};

TAILQ_HEAD(pqueue, proc);
static struct pqueue queue = TAILQ_HEAD_INITIALIZER(queue);
static unsigned int pid = 1;

extern void trapret();

void init_sched();
void initsys();
void enqueue(struct proc *);
void dequeue();
void yield();
__dead void scheduler();

#ifdef NOKTHREAD
static
#endif
struct proc *kthread_create(void(*)());

int sys_fork();
int sys_execve(const char *, const char **, const char **);

static void forkret();
static struct proc *alloc_thread();
static struct proc *proc_create();
static int loadelf(struct proc *, const char *);

static void
forkret()
{
	return;
}

static struct proc *
alloc_thread()
{
	struct proc *p;
	u32 kstack;
	u32 sp;

	kstack = alloc_pages(NULL, KSTACK_SIZE);
	assert(kstack != NULL);
	memset(kstack, 0, KSTACK_SIZE);
	p = (struct proc *)(kstack + KSTACK_SIZE - sizeof(*p));
	p->kstack = kstack;
	sp = p;

	sp -= sizeof(*p->tf);
	p->tf = (struct trapframe *)sp;

	sp -= sizeof(u32);
	*(u32 *)sp = trapret;

	sp -= sizeof(*p->context);
	p->context = (struct context *)sp;
	memset(p->context, 0, sizeof(*p->context));
	p->context->eip = (u32)forkret;

	return p;
}

static struct proc *
proc_create()
{
	struct proc *p = alloc_thread();

	p->tf->gs = (4 * 8) | 3;
	p->tf->fs = (4 * 8) | 3;
	p->tf->es = (4 * 8) | 3;
	p->tf->ds = (4 * 8) | 3;

	/* iret */
	p->tf->cs = (3 * 8) | 3;
	p->tf->eflags = 0x0202;

	p->tf->ss = (4 * 8) | 3;

	p->state = READY;
	p->pid = pid++;

	return p;
}

static int
loadelf(struct proc *p, const char *path)
{
	int i;
	u32 *pd;
	u32 mem, bin, sz;
	struct elfhdr *elf;
	struct proghdr *ph;
	struct mem_map *m;
	u32 start, end;
	u32 min = ~0;
	u32 max = 0;
	int n = 0;

	for (i = ROOT_INO; i < k.ino_n; ++i)
		if (strcmp(path, k.dir_tbl[i].name) == 0)
			goto found;

	return -1;

found:
	bin = k.ino_tbl[i].addr;
	elf = (struct elfhdr *)bin;

	if (elf->magic != ELFMAGIC)
		return -1;

	pd = copykvm();
	ph = (struct proghdr *)(bin + elf->ph_off);
	for (i = 0; i < elf->ph_n; ++i) {
		switch (ph[i].type) {
		case 1:
			++n;
			break;
		case 2: /* FALLTHROUGH */
		case 3:
			return -1;
		default:
			continue;
		}

		start = ph[i].va;
		end = start + ph[i].memsz;

		if (start < min)
			min = pg_rounddown(start);

		if (end > max)
			max = pg_roundup(end);
	}

	sz = pg_roundup(max - min + STACK_SIZE);
	mem = alloc_pages(NULL, sz);
	/* assert */
	memset(mem, 0, sz);
	m = kmalloc(sizeof(struct mem_map) * (n + 1));
	GETMEMMAP(p)->sz = sz;

	n = 0;

	for (i = 0; i < elf->ph_n; ++i) {
		switch (ph[i].type) {
		case 1:
			break;
		case 2: /* FALLTHROUGH */
		case 3:
			return -1;
		default:
			continue;
		}

		m[n].va = ph[i].va;
		m[n].pa = v2p(mem);
		m[n].len = pg_roundup(ph[i].memsz);
		
		SLIST_INSERT_HEAD(GETMEMMAP(p), &m[n], entry);

		memmove((mem + ph[i].va % 0x1000),
		    (bin + ph[i].off),
		    ph[i].filesz);

		alloc_pt(pd,
		    m[n].va,
		    m[n].pa,
		    m[n].len,
		    PG_US | PG_RW | PG_P);

		++n;
		mem += pg_roundup(ph[i].memsz);
	}

	p->cr3 = v2p(pd);
	p->tf->eip = elf->entry;
	p->tf->esp = KERNEL_OFFSET;
	
	m[n].va = KERNEL_OFFSET - STACK_SIZE;
	m[n].pa = v2p(mem);
	m[n].len = STACK_SIZE;

	alloc_pt(pd,
	    m[n].va,
	    m[n].pa,
	    m[n].len,
	    PG_US | PG_RW | PG_P);

	SLIST_INSERT_HEAD(GETMEMMAP(p), &m[n], entry);

	return 0;
}

#ifdef NOKTHREAD
static
#endif
struct proc *
kthread_create(void (*eip)())
{
	struct proc *p = alloc_thread();

	p->cr3 = NULL;
	p->state = READY;

	p->tf->gs = (2 * 8) | 0;
	p->tf->fs = (2 * 8) | 0;
	p->tf->ds = (2 * 8) | 0;
	p->tf->es = (2 * 8) | 0;

	/* iret */
	p->tf->eip = eip;
	p->tf->cs = (1 * 8) | 0;
	p->tf->eflags = 0x0202;

	return p;
}

struct proc *
getproc(void)
{
	return curthread;
}

void
init_pmm()
{
	int i;

	memtab = alloc_pages(NULL, sizeof(struct proc_mmap) * 128);
	assert(memtab != NULL);

	for (i = 0; i < 128; ++i)
		SLIST_INIT(&memtab[i]);

}

void
initsys()
{
	curthread = proc_create();
	curthread->pid = pid++;

	assert(curthread != NULL);
	assert(loadelf(curthread, "/sbin/init") >= 0);
	sys_open("/dev/console", 0);

	enqueue(curthread);
}

void
init_sched()
{
	idle = kthread_create(scheduler);
	curthread = idle;

	tss.ss0 = (2 * 8) | 0;

	scheduler();
}

void
enqueue(struct proc *task)
{
	TAILQ_INSERT_TAIL(&queue, task, entries);
}

void
dequeue()
{
	cli();
	curthread->state = DEAD;
	yield();
}

void
yield()
{
	cli();
	if (curthread->state == RUNNING)
		curthread->state = READY;

	switch_to(&curthread->context, idle->context);
}

__dead void
scheduler()
{
	struct proc *p;

	for (;;) {
		cli();

		TAILQ_FOREACH(p, &queue, entries)
			if (p->state == READY)
				goto found;

		for (;;) {
			sti();
			hlt();
		}

found:
		curthread = p;
		TAILQ_REMOVE(&queue, p, entries);
		curthread->state = RUNNING;

		if (curthread->cr3) {
			write_cr3(curthread->cr3);
			tss.esp0 = curthread;
		}

		switch_to(&idle->context, p->context);

		TAILQ_INSERT_TAIL(&queue, p, entries);
	}
}

int
sys_fork()
{
	int i;
	u32 *pd;
	u32 mem;
	struct mem_map *m1, *m2;
	struct proc *n = proc_create();

	if (n == NULL)
		return -1;

	n->pid = pid++;

	mem = alloc_pages(NULL, GETMEMMAP(curthread)->sz);
	if (mem == NULL)
		return -1;

	pd = copykvm();
	if (pd == NULL)
		return -1;

	SLIST_FOREACH(m1, GETMEMMAP(curthread), entry) {
		m2 = kmalloc(sizeof(struct mem_map));

		m2->pa = v2p(mem);
		m2->va = m1->va;
		m2->len = m1->len;

		alloc_pt(pd,
		    m2->va,
		    m2->pa,
		    m2->len,
		    PG_US | PG_RW | PG_P);

		memmove(mem, p2v(m1->pa), m1->len);

		mem += pg_roundup(m2->len);
		SLIST_INSERT_HEAD(GETMEMMAP(n), m2, entry);
	}

	n->cr3 = v2p(pd);
	*n->tf = *curthread->tf;
	n->tf->eax = 0; /* child returns 0 */
	enqueue(n);

	return n->pid;
}

int
sys_execve(const char *path, const char **argv, const char **envp)
{
	struct proc *p = proc_create();
	p->pid = curthread->pid;

	if (loadelf(p, path) < 0)
		return -1;

	enqueue(p);
	dequeue();
}
