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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static struct proc *curthread;

TAILQ_HEAD(pqueue, proc);
struct pqueue queue = TAILQ_HEAD_INITIALIZER(queue);

extern void swap_cr3(uint32_t);

void init_proc(void);
void enqueue(struct proc *);
void dequeue(void);
void yield(void);
struct proc *proc_create(uint32_t, uint32_t, uint32_t);
struct proc *kthread_create(void (*)(void));

static void
idle(void)
{
	for (;;)
		__asm__ volatile("sti; hlt");
}

static uint32_t *
uvm_alloc(uint32_t *p)
{
	uint32_t *t = mmap(NULL, 0x1000, PG_RW | PG_P);
	*p = virt2phys(t);

	return t;
}

void
enqueue(struct proc *task)
{
	cli();
	TAILQ_INSERT_TAIL(&queue, task, entries);
	sti();
}
void
dequeue(void)
{
	cli();
	curthread->state = DEAD;
	yield();
}

void
init_proc(void)
{
	struct proc boot_task;
	struct proc *swapper;
	uintptr_t addr = (uintptr_t)mmap(NULL, 0x1000, PG_RW | PG_P);

	swapper = (struct proc *)(addr + 0x1000 - sizeof(struct proc));
	swapper->esp = (uint32_t *)swapper - 6;
	swapper->pid = 0;
	swapper->cr3 = (uint32_t *)read_cr3();
	swapper->state = RUNNING;
	curthread = swapper;

	/* stack frame */
	swapper->esp[4] = (uint32_t)idle;

/*
 * overwriting the structure as it is
 * written to once and never used again.
 */
	boot_task.esp = (uint32_t *)&boot_task;
	switch_to(&boot_task, curthread);
}

void
yield(void)
{
	struct proc *p, *n;

	cli();

	if (TAILQ_EMPTY(&queue)) {
		sti();
		return;
	}

	p = curthread;
	TAILQ_FOREACH(n, &queue, entries) {
		if (n->state != READY)
			continue;

		if (p->state == RUNNING) {
			p->state = READY;
			TAILQ_INSERT_TAIL(&queue, curthread, entries);
		}

		curthread = TAILQ_FIRST(&queue);
		TAILQ_REMOVE(&queue, curthread, entries);
		curthread->state = RUNNING;
		switch_to(p, curthread);

		break;
	}

	sti();
	return;
}

struct proc *
kthread_create(void (*eip)(void))
{
	struct proc *task;
	uintptr_t addr = (uintptr_t)mmap(NULL, 0x1000, PG_RW | PG_P);

	task = (struct proc *)(addr + 0x1000 - sizeof(struct proc));
	task->esp = (uint32_t *)task - 6;
	task->cr3 = (uint32_t *)read_cr3();
	task->state = READY;

	/* stack frame */
	task->esp[4] = (uint32_t)eip;
	task->esp[5] = (uint32_t)dequeue;

	return task;
}

struct proc *
proc_create(uint32_t addr, uint32_t v_addr, uint32_t len)
{
	static int pid = 1;

	struct proc *p;
	uint32_t *pd, *pt;
	uintptr_t ss0, ss;
	uint32_t frame, ph;
	uint32_t p_addr = virt2phys(addr);
	uint32_t entry;
	int pte, pde;

	pd = mmap(NULL, 0x1000, PG_RW | PG_P);
	memcpy(pd, (uint32_t *)phys2virt(read_cr3()), 0x1000);

	v_addr = pg_rounddown(v_addr);
	p_addr = pg_rounddown(p_addr);
	entry = v_addr + (addr & 0x0FFF);
	len = pg_roundup(len);

	for (; len > 0; len -= 0x1000) {
		frame = p_addr;

		pde = v_addr >> 22;
		pte = (v_addr >> 12) & 0x3FF;
		pt = uvm_alloc(&ph);

		pd[pde] = (ph & 0xFFFFF000) | (PG_US | PG_RW | PG_P);
		pt[pte] = (frame & 0xFFFFF000) | (PG_US | PG_RW | PG_P);

		p_addr += 0x1000;
		v_addr += 0x1000;
	}

	ss0 = (uintptr_t)mmap(NULL, 0x1000, PG_RW | PG_P);
	ss = (uintptr_t)mmap(NULL, 0x1000, PG_US | PG_RW | PG_P);
	p = (struct proc *)(ss0 + 0x1000 - sizeof(struct proc));

/*
	p->esp = (uint32_t *)(ss + 0x1000) - 9;
	p->esp[4] = entry;
	p->esp[5] = (3 * 8) | 3;
	p->esp[6] = 0x0202;
	p->esp[7] = v_addr + ((uint32_t)p->esp & 0x0FFF);
	p->esp[8] = (4 * 8) | 3;
*/

	p->esp = (uint32_t *)(ss + 0x1000) - 6;
	p->esp[4] = entry;
	p->esp0 = (uint32_t *)p - 6;
	p->state = READY;
	p->cr3 = (uint32_t *)virt2phys(pd);
	p->pid = pid++;

	return p;
}
