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

#include <sys/trunix.h>
#include <sys/queue.h>
#include <sys/proc.h>
#include <sys/mman.h>
#include <sys/cdefs.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define STACK_SIZE 0x1000
#define KSTACK_SIZE 0x1000

extern struct task_state_segment tss;

static struct proc *curthread;
static struct proc *idle;

TAILQ_HEAD(pqueue, proc);
struct pqueue queue = TAILQ_HEAD_INITIALIZER(queue);

extern void swap_cr3(uint32_t);
extern void trapret(void (*)(void));
extern void init(void);

void init_sched(void);
void enqueue(struct proc *);
void dequeue(void);
void yield(void);
struct proc *kthread_create(void (*)(void));
__dead void scheduler(void);

static void forkret(void);
static struct proc *alloc_thread(void);

static void
forkret(void)
{
	return;
}

static struct proc *
alloc_thread(void)
{
	struct proc *p;
	uintptr_t kstack;
	uintptr_t sp;

	kstack = (uintptr_t)mmap(NULL, KSTACK_SIZE, PG_RW | PG_P);
	p = (struct proc *)(kstack + KSTACK_SIZE - sizeof(*p));
	p->kstack = kstack;
	sp = (uintptr_t)p;

	sp -= sizeof(*p->tf);
	p->tf = (struct trapframe *)sp;

	sp -= sizeof(uint32_t);
	*(uint32_t *)sp = (uint32_t)trapret;

	sp -= sizeof(*p->context);
	p->context = (struct context *)sp;
	memset(p->context, 0, sizeof(*p->context));
	p->context->eip = (uint32_t)forkret;

	return p;
}

static struct proc *
proc_create(void)
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

struct proc *
kthread_create(void (*eip)(void))
{
	struct proc *p = alloc_thread();

	p->cr3 = (uintptr_t)NULL;
	p->state = READY;

	p->tf->gs = (2 * 8) | 0;
	p->tf->fs = (2 * 8) | 0;
	p->tf->ds = (2 * 8) | 0;
	p->tf->es = (2 * 8) | 0;

	/* iret */
	p->tf->eip = (uint32_t)eip;
	p->tf->cs = (1 * 8) | 0;
	p->tf->eflags = 0x0202;

	return p;
}

struct proc *
initsys(void)
{
	uintptr_t mem;
	size_t initsize = 0x1000;
	struct proc *p = proc_create();
	uint32_t *pd = copykvm();

	p->size = initsize + STACK_SIZE;

	mem = (uintptr_t)mmap(NULL, p->size, PG_US | PG_RW | PG_P);
	memset((void *)mem, 0, p->size);
	p->addr = mem;

	p->cr3 = (uintptr_t)virt2phys(pd);
	alloc_pt(pd, 0x40000000, p->size, virt2phys(mem), PG_US | PG_RW | PG_P);

	memmove((void *)mem, init, initsize);

	p->tf->eip = 0x40000000;
	p->tf->esp = 0x40000000 + p->size;

	return p;
}

void
enqueue(struct proc *task)
{
	TAILQ_INSERT_TAIL(&queue, task, entries);
}

void
dequeue(void)
{
	cli();
	curthread->state = DEAD;
	yield();
}

void
yield(void)
{
	cli();
	curthread->state = READY;
	switch_to(&curthread->context, idle->context);
}

void
init_sched(void)
{
	idle = kthread_create(scheduler);
	curthread = idle;

	tss.ss0 = (2 * 8) | 0;

	scheduler();
}

__dead void
scheduler(void)
{
	struct proc *p;

	for (;;) {
		cli();

		TAILQ_FOREACH(p, &queue, entries)
			if (p->state == READY)
				goto found;

		sti();
		for (;;)
			hlt();

found:
		curthread = p;
		TAILQ_REMOVE(&queue, p, entries);
		curthread->state = RUNNING;

		if (curthread->cr3) {
			swap_cr3(curthread->cr3);
			tss.esp0 = (uintptr_t)curthread;
		}

		switch_to(&idle->context, p->context);

		TAILQ_INSERT_TAIL(&queue, p, entries);
	}
}

int
fork(void)
{
	uintptr_t mem;
	struct proc *p = proc_create();
	uint32_t *pd = copykvm();

	p->size = curthread->size;
	mem = (uintptr_t)mmap(NULL, p->size, PG_US | PG_RW | PG_P);
	memset((void *)mem, 0, p->size);
	p->addr = mem;
	memcpy((void *)p->addr, (void *)curthread->addr, p->size);

	p->cr3 = (uintptr_t)virt2phys(pd);
	alloc_pt(pd, 0x40000000, p->size, virt2phys(mem), PG_US | PG_RW | PG_P);

	*p->tf = *curthread->tf;
	p->tf->eax = 0; /* child returns 0 */

	enqueue(p);
	return p->pid;
}

