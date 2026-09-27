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

extern struct task_state_segment tss;

static struct proc *curthread;
static uint32_t kpagedir;
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

static void forkret(void);
static struct proc *alloc_proc(void);

static void
forkret(void)
{
	if (curthread->cr3)
		swap_cr3(curthread->cr3);

	tss.esp0 = curthread->kstack + STACK_SIZE;

	return;
}

static struct proc *
alloc_proc(void)
{
	static unsigned int pid = 1;
	struct proc *p;
	uintptr_t kstack;
	uintptr_t sp;

	kstack = (uintptr_t)mmap(NULL, 0x2000, PG_RW | PG_P);
	p = (struct proc *)(kstack + 0x2000 - sizeof(*p));
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

	p->pid = pid++;

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

__dead void
scheduler(void)
{
	int found = 0;
	struct proc *p;

	for (;;) {
		cli();

		if (TAILQ_EMPTY(&queue))
			goto wait;

		found = 0;
		TAILQ_FOREACH(p, &queue, entries) {
			if (p->state == READY) {
				found = 1;
				break;
			}
		}

		if (!found)
			goto wait;

		curthread = p;
		TAILQ_REMOVE(&queue, p, entries);
		curthread->state = RUNNING;

		switch_to(&idle->context, p->context);

		TAILQ_INSERT_TAIL(&queue, p, entries);
		continue;

wait:
		__asm__ volatile("sti\n\thlt");

	}
}

void
init_sched(void)
{
	idle = kthread_create(scheduler);
	curthread = idle;

	tss.ss0 = (2 * 8) | 0;

	scheduler();
}

struct proc *
kthread_create(void (*eip)(void))
{
	struct proc *p = alloc_proc();

	p->cr3 = (uintptr_t)NULL;
	p->state = READY;

	p->tf->gs = (2 * 8) | 0;
	p->tf->fs = (2 * 8) | 0;
	p->tf->ds = (2 * 8) | 0;
	p->tf->es = (2 * 8) | 0;

	p->tf->eip = (uint32_t)eip;
	p->tf->cs = (1 * 8) | 0;
	p->tf->eflags = 0x0202;
	p->tf->ss = (2 * 8) | 0;

	return p;
}

struct proc *
initsys(void)
{
	uint32_t pte, pde;

	struct proc *p = alloc_proc();
	uintptr_t mem = (uintptr_t)mmap(NULL, 0x1000, PG_US | PG_RW | PG_P);

	uint32_t *pd = mmap(NULL, 0x1000, PG_RW | PG_P);
	uint32_t *pt = mmap(NULL, 0x1000, PG_RW | PG_P);

	memset((void *)mem, 0, 0x1000);

	memcpy(pd, (uint32_t *)phys2virt(read_cr3()), 0x1000);
	p->cr3 = (uintptr_t)virt2phys(pd);

	pde = 0x40000000 >> 22;
	pte = (0x40000000 >> 12) & 0x3FF;

	pd[pde] = (virt2phys(pt) & 0xFFFFF000) | (PG_US | PG_RW | PG_P);
	pt[pte] = (virt2phys(mem) & 0xFFFFF000) | (PG_US | PG_RW | PG_P);

	memmove((void *)mem, init, 0x500);

/* stack frame */
	p->tf->gs = (4 * 8) | 3;
	p->tf->fs = (4 * 8) | 3;
	p->tf->es = (4 * 8) | 3;
	p->tf->ds = (4 * 8) | 3;

	p->tf->eip = 0x40000000;
	p->tf->cs = (3 * 8) | 3;
	p->tf->eflags = 0x0202;
	p->tf->esp = 0x40001000;
	p->tf->ss = (4 * 8) | 3;

	p->state = READY;

	return p;
}
