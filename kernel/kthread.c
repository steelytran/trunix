/*
 * kernel threads
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

#include <stdint.h>
#include <stdio.h>

#include <trunix/trunix.h>
#include <sys/kthread.h>
#include <sys/mman.h>

struct task *curthread;

extern uint32_t read_cr3(void);
extern uint32_t read_esp(void);

void kthread_init(void);
void switch_to(struct task *, struct task *);
int kthread_create(void (*)(void));
void kthread_exit(int);
void yield(void);
void sched_init(void);

static void
swapper(void)
{
	for (;;)
		__asm__ volatile("sti; hlt");
}

void
sched_init(void)
{
	struct task boot_task;

/*
 * overwriting the structure as it is
 * written to once and never used again.
 */
	boot_task.esp = (uint32_t *)&boot_task;
	switch_to(&boot_task, curthread);
}

void
kthread_init(void)
{
	struct task *idle;
	uintptr_t addr = (uintptr_t)mmap(NULL, 0x1000, PROT_WRITE, 0, 0, 0);

	idle = (struct task *)(addr + 0x1000 - sizeof(struct task));
	idle->esp = (uint32_t *)idle - 6;
	idle->cr3 = (uint32_t *)read_cr3();
	idle->pid = 0;
	idle->state = READY;
	idle->next = idle;

	/* stack frame */
	idle->esp[4] = (uint32_t)swapper;
	idle->esp[5] = (uint32_t)kthread_exit;

	curthread = idle;
}

int
kthread_create(void (*eip)(void))
{
	struct task *task;
	uintptr_t addr = (uintptr_t)mmap(NULL, 0x2000, PROT_WRITE, 0, 0, 0);

	cli();

	task = (struct task *)(addr + 0x2000 - sizeof(struct task));
	task->esp = (uint32_t *)task - 6;
	task->cr3 = (uint32_t *)read_cr3();
	task->state = READY;

	/* stack frame */
	task->esp[4] = (uint32_t)eip;
	task->esp[5] = (uint32_t)kthread_exit;


	task->next = curthread->next;
	curthread->next = task;

	sti();

	return 0;
}

void
kthread_exit(int ecode)
{
	struct task *np = curthread;

	cli();

	curthread->state = DEAD;
	while (np->next != curthread)
		np = np->next;

	np->next = curthread->next;

	schedule();
}

void
schedule(void)
{
	struct task *prev = curthread;
	struct task *next = curthread->next;

	cli();

	while (next->state != READY) {
		if (next == prev) {
			sti();
			return;
		}

		next = next->next;
	}

	if (prev->state == RUNNING)
		prev->state = READY;

	curthread = next;
	curthread->state = RUNNING;
	switch_to(prev, next);
}
