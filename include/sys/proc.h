/*
 * processes
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

#ifndef _SYS_PROC_H
#define _SYS_PROC_H

#include <sys/queue.h>
#include <sys/cdefs.h>
#include <stddef.h>
#include <stdint.h>

enum procstate {RUNNING, READY, SLEEPING, BLOCKED, DEAD};

struct trapframe {
/* in order of pushal instruction */
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t __esp; /* unused */
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;

	uint32_t gs;
	uint32_t fs;
	uint32_t es;
	uint32_t ds;

/* iret stack frame */
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
 /* unused by cpl 0 -> 0 switch */
	uint32_t esp;
	uint32_t ss;
};

struct context {
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t ebx;
	uint32_t eip;
};

struct proc {
	uint32_t pid;
	uint32_t cr3;
	uintptr_t kstack;
	struct context *context;
	struct trapframe *tf;
	enum procstate state;
	TAILQ_ENTRY(proc) entries;
};

void switch_to(struct context **, struct context *);
void enqueue(struct proc *);
void dequeue(void);
void yield(void);
__dead void scheduler(void);
struct proc *kthread_create(void (*)(void));
void init_sched(void);
struct proc *initsys(void);

#endif
