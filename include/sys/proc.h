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

#ifdef ASM_FILE
#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define DEAD 3
#endif

#ifndef ASM_FILE
#include <sys/queue.h>
#include <stddef.h>
#include <stdint.h>

enum procstate {RUNNING, READY, SLEEPING, BLOCKED, DEAD};

struct proc {
	uint32_t *esp;
	uint32_t *esp0;
	uint32_t *cr3;
	uint32_t pid;
	enum procstate state;
	void *chan; /* sleep if non zero */
	TAILQ_ENTRY(proc) entries;
};

void switch_to(struct proc *, struct proc *);
void init_proc(void);
void enqueue(struct proc *);
void dequeue(void);
void yield(void);
struct proc *proc_create(uint32_t, uint32_t, uint32_t);
struct proc *kthread_create(void (*)(void));

#endif
#endif
