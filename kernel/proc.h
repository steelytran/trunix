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

#ifndef _PROC_H
#define _PROC_H

#include <sys/queue.h>
#include <sys/cdefs.h>

enum procstate {RUNNING, READY, SLEEPING, BLOCKED, DEAD};

struct trapframe {
/* in order of pushal instruction */
	u32 edi;
	u32 esi;
	u32 ebp;
	u32 __esp; /* unused */
	u32 ebx;
	u32 edx;
	u32 ecx;
	u32 eax;

	u32 gs;
	u32 fs;
	u32 es;
	u32 ds;

/* iret stack frame */
	u32 eip;
	u32 cs;
	u32 eflags;
 /* unused by cpl 0 -> 0 switch */
	u32 esp;
	u32 ss;
};

struct context {
	u32 edi;
	u32 esi;
	u32 ebp;
	u32 ebx;
	u32 eip;
};

struct proc {
	int pid;
	u32 cr3;
	u32 kstack;
	u32 stack;
	u32 start;
	u32 end;

	struct context *context;
	struct trapframe *tf;

	enum procstate state;
	struct proc *parent;
	TAILQ_ENTRY(proc) entries;
};

void switch_to(struct context **, struct context *);
void enqueue(struct proc *);
void dequeue();
void yield();
struct proc *kthread_create(void(*)());
void init_sched();
void initsys();

#endif
