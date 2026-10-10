/*
 * memory manager
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

#ifndef _TRUNIX_VM_H
#define _TRUNIX_VM_H

#include <sys/multiboot.h>
#include <sys/cdefs.h>
#include <sys/queue.h>

#include "trunix.h"

#define v2p(x) ((u32)(x) & ~KERNEL_OFFSET)
#define p2v(x) ((u32)(x) | KERNEL_OFFSET)
#define GETMEMMAP(x) (&memtab[(x)->pid - 1])
#define KERNEL_OFFSET ((u32)_kernel_offset)
#define pde2pt(pde) (u32 *)(0xFFC00000 + ((pde) << 12))
#define pg_roundup(x) roundup((x), 0x1000)
#define pg_rounddown(x) rounddown((x), 0x1000)

#define PG_P 0x0001
#define PG_RW 0x0002
#define PG_US 0x0004
#define PG_PSE 0x0080
#define PG_G 0x0100

extern char _kernel_physical_base[];
extern char _kernel_unpaged_end[];
extern char _kernel_virt_base[];
extern char _kernel_size[];
extern char _kernel_offset[];

struct vm_region {
	u32 va;
	u32 pa;
	u32 len;
	SLIST_ENTRY(vm_region) entry;
};

struct proc_mmap {
        struct vm_region *slh_first;
	u32 sz;
};
extern struct proc_mmap *memtab;

void add_memmap(struct kinfo *, u64, u64);
void cut_memmap(struct kinfo *, u32, u32);

void pg_clear();
void pg_identity(struct kinfo *);
void pg_clear_identity();
void pg_enable();

void flush_tlb();

void init_mem(struct kinfo *k);
void alloc_pt(u32 *, u32, u32, u32, int);

void *alloc_pages(void *, u32);
void free_pages(void *, u32);
void *kmalloc(u32);
void kfree(void *);

u32 *copykvm();

/*
 * round value up to page boundaries.
 */
static inline u32
roundup(u32 x, u32 a)
{
	u32 p = x % a;

	if (p == 0)
		return x;

	return x + a - p;
}

/*
 * round value down to page boundaries.
 */
static inline u32
rounddown(u32 x, u32 a)
{
	u32 p = x % a;

	if (p == 0)
		return x;

	return x - p;
}


#endif
