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

#ifndef _TRUNIX_PROC_H
#define _TRUNIX_PROC_H

#include <trunix/fs.h>
#include <trunix/vm.h>
#include <sys/queue.h>
#include <sys/cdefs.h>

#define NOKTHREAD

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
/* unused by kernel thread switching */
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

	struct context *context;
	struct trapframe *tf;

	struct file *ofile[MAXFILES];

	enum {RUNNING, READY, SLEEPING, BLOCKED, DEAD} state;

	struct proc *parent;
	TAILQ_ENTRY(proc) entries;
};

void switch_to(struct context **, struct context *);
void enqueue(struct proc *);
void dequeue();
void yield();
void init_pmm();
void init_sched();
void initsys();
struct proc *getproc(void);

#ifndef NOKTHREAD
struct proc *kthread_create(void(*)());
#endif

#endif
