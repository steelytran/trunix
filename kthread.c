#include <stdint.h>
#include <stdio.h>

#include <sys/trunix.h>
#include <sys/kthread.h>
#include <sys/mman.h>

struct thread *curthread;

extern uint32_t read_cr3(void);
extern uint32_t read_esp(void);

void kthread_init(void);
void switch_to(struct thread *, struct thread *);
int kthread_create(void (*)(void));
void kthread_exit(int);
void yield(void);
void sched_init(void);

enum {
	TASK_RUNNING,
	TASK_READY,
	TASK_BLOCKED,
	EXIT_DEAD
};

static void
swapper(void)
{
	for (;;)
		__asm__ volatile("sti; hlt");
}

void
sched_init(void)
{
	struct thread boot_task;

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
	struct thread *idle;
	uintptr_t addr = (uintptr_t)alloc_mem(1);

	idle = (struct thread *)(addr + 0x1000 - sizeof(struct thread));
	idle->esp = (uint32_t *)idle - 6;
	idle->cr3 = (uint32_t *)read_cr3();
	idle->pid = 0;
	idle->state = TASK_RUNNING;
	idle->next = idle;

	/* ret stack frame */
	idle->esp[4] = (uint32_t)swapper;
	idle->esp[5] = (uint32_t)kthread_exit;

	curthread = idle;
}

int
kthread_create(void (*eip)(void))
{
	struct thread *task;
	uintptr_t addr = (uintptr_t)alloc_mem(2);

	cli();

	task = (struct thread *)(addr + 0x2000 - sizeof(struct thread));
	task->esp = (uint32_t *)task - 6;
	task->cr3 = (uint32_t *)read_cr3();
	task->state = TASK_READY;

	/* iret stack frame */
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
	struct thread *np = curthread;

	cli();

	curthread->state = EXIT_DEAD;
	while (np->next != curthread)
		np = np->next;

	np->next = curthread->next;

	schedule();
}

void
schedule(void)
{
	struct thread *prev = curthread;
	struct thread *next = curthread->next;

	cli();

	if (prev != next) {
		curthread = next;
		switch_to(prev, next);
	}

}

static inline void
context_switch(struct thread *prev, struct thread *next)
{
	/* 
	 * TODO: switch page directory here,
	 * if different than cr3 register.
	 */

	//switch_to(prev, next, prev);
}
