#ifndef _SYS_KTHREAD_H
#define _SYS_KTHREAD_H

#ifdef ASM_FILE
#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define DEAD 3
#endif

#ifndef ASM_FILE
#include <stddef.h>
#include <stdint.h>

struct task {
	uint32_t *esp;
	uint32_t *esp0;
	uint32_t *cr3;
	int32_t pid;

	enum taskstate {
		RUNNING,
		READY,
		BLOCKED,
		DEAD
	} state;

	struct task *next;
};

extern struct task *curthread;

void kthread_init(void);
void switch_to(struct task *, struct task *);
int kthread_create(void (*)(void));
void kthread_exit(int);
void schedule(void);
void sched_init(void);

#endif
#endif
