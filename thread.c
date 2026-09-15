#include <stdint.h>
#include <stdio.h>
#include <sys/trunix.h>

struct tcb *current_task_tcb;

extern uint32_t read_cr3(void);
extern uint32_t read_esp(void);

void
init_thread(void (*eip)(void))
{
	struct tcb *task;
	uintptr_t addr = (uintptr_t)alloc_mem(2);

	task = (struct tcb *)(addr + 0x2000 - sizeof(struct tcb));

	task->esp = (uint32_t *)task - 3;
	task->esp[2] = (uint32_t)eip;

	task->cr3 = (uint32_t *)read_cr3();

	current_task_tcb = task;
	current_task_tcb->next = current_task_tcb;
}

void
create_kernel_thread(void (*eip)(void))
{
	struct tcb *new_task;
	uintptr_t addr = (uintptr_t)alloc_mem(2);

	new_task = (struct tcb *)(addr + 0x2000 - sizeof(struct tcb));

	new_task->esp = (uint32_t *)new_task - 3;
	new_task->esp[2] = (uint32_t)eip;

	new_task->cr3 = (uint32_t *)read_cr3();

	new_task->next = current_task_tcb->next;
	current_task_tcb->next = new_task;
}

void
schedule(void)
{
		switch_to(current_task_tcb->next);
}
