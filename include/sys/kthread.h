#ifndef _KTHREAD_H
#define _KTHREAD_H

#include <stdint.h>
#include <stddef.h>

struct thread {
	uint32_t *esp;
	uint32_t *esp0;
	uint32_t *cr3;
	int32_t pid;
	uint8_t state;
	struct thread *next;
};

void kthread_init(void);
void switch_to(struct thread *, struct thread *);
int kthread_create(void (*)(void));
void kthread_exit(int);
void schedule(void);
void sched_init(void);

#endif
