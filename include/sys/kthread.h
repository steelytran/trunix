#ifndef _SYS_KTHREAD_H
#define _SYS_KTHREAD_H

#include <stddef.h>
#include <stdint.h>


struct task {
	uint32_t *esp;
	uint32_t *esp0;
	uint32_t *cr3;
	int32_t pid;
	uint8_t state;
	struct task *next;
};

void kthread_init(void);
void switch_to(struct task *, struct task *);
int kthread_create(void (*)(void));
void kthread_exit(int);
void schedule(void);
void sched_init(void);

#endif
