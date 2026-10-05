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
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "trunix.h"
#include "proc.h"
#include "vm.h"
#include "file.h"

#define STACK_SIZE 0x1000
#define KSTACK_SIZE 0x1000

#define ELFMAGIC 0x464C457F

extern u32 *kpagedir;
extern struct task_state_segment tss;
extern struct kinfo k;

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
	u32 vaddr;
	u32 paddr;
	u32 filesz;
	u32 memsz;
	u32 flags;
	u32 align;
};

TAILQ_HEAD(pqueue, proc);
struct pqueue queue = TAILQ_HEAD_INITIALIZER(queue);

extern trapret();

void init_sched();
void initsys();
void enqueue(struct proc *);
void dequeue();
void yield();
struct proc *kthread_create(void(*)());
__dead void scheduler();

sys_fork();
sys_execve();

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

	kstack = alloc_mem(KSTACK_SIZE);
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
	static unsigned int pid = 1;
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

static
loadelf(struct proc *p, const char *path)
{
	u32 mem;
	u32 *pd;

	u32 bin;
	struct elfhdr *elf;
	struct proghdr *ph;
	int i;
	u32 start, end;
	u32 min = ~0;
	u32 max = 0;
	u32 sz;

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
			break;
		case 2: /* FALLTHROUGH */
		case 3:
			return -1;
		default:
			continue;
		}

		start = ph[i].vaddr;
		end = start + ph[i].memsz;

		if (start < min)
			min = pg_rounddown(start);

		if (end > max)
			max = pg_roundup(end);
	}

	sz = max - min + STACK_SIZE;
	mem = alloc_mem(sz);
	memset(mem, 0, sz);

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

		memmove((mem + ph[i].vaddr % 0x1000),
		    (bin + ph[i].off),
		    ph[i].filesz);

		alloc_pt(pd,
		    ph[i].vaddr,
		    ph[i].memsz,
		    virt2phys(mem),
		    PG_US | PG_RW | PG_P);

		mem += pg_roundup(ph[i].memsz);
	}

	p->stack = mem - STACK_SIZE;
	p->start = min;
	p->end = max;
	p->cr3 = virt2phys(pd);
	p->tf->eip = elf->entry;
	p->tf->esp = KERNEL_OFFSET;

	alloc_pt(pd,
	    KERNEL_OFFSET - STACK_SIZE,
	    STACK_SIZE,
	    virt2phys(mem - STACK_SIZE),
	    PG_US | PG_RW | PG_P);

	return 0;
}

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

void
initsys()
{
	struct proc *p = proc_create();
	if (p == NULL)
		panic();

	if (loadelf(p, "/sbin/init") < 0)
		panic();

	enqueue(p);
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

#if 0
int
sys_fork()
{
	u32 mem;
	u32 sz;
	struct proc *p = proc_create();
	u32 *pd = copykvm();

	sz = curthread->end - curthread->start;

	mem = alloc_pages(sz, PG_US | PG_RW | PG_P);
	memset(mem, 0, sz);
	p->start = mem;

	printf("0x%x\n", mem);

	memmove(p->start, curthread->start, sz);
	p->cr3 = virt2phys(pd);
	alloc_pt(pd, 0x40000000, sz, virt2phys(mem), PG_US | PG_RW | PG_P);

	p->stack = alloc_pages(STACK_SIZE, PG_US | PG_RW | PG_P);
	memmove(p->stack, curthread->stack, STACK_SIZE);

	alloc_pt(pd,
	    KERNEL_OFFSET - STACK_SIZE,
	    STACK_SIZE,
	    virt2phys(p->stack),
	    PG_US | PG_RW | PG_P);

	*p->tf = *curthread->tf;
	p->tf->eax = 0; /* child returns 0 */

	enqueue(p);

	return p->pid;
}

int
sys_execve(const char *path, const char **argv, const char **envp)
{
	loadelf(curthread, path);
}
#endif
