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

#ifndef _VM_H
#define _VM_H

#include <sys/multiboot.h>
#include <stdint.h>
#include <stddef.h>

#include "trunix.h"

#define KERNEL_OFFSET ((uint32_t)_kernel_offset)

#define pde2pt(pde) (uint32_t *)(0xFFC00000 + ((pde) << 12))

#define PG_P 0x0001
#define PG_RW 0x0002
#define PG_US 0x0004
#define PG_PSE 0x0080
#define PG_G 0x0100

#ifdef ASM_FILE
#define virt2phys(x) ((x) - KERNEL_OFFSET)
#define phys2virt(x) ((x) + KERNEL_OFFSET)
#else
#define virt2phys(x) ((uintptr_t)(x) & ~KERNEL_OFFSET)
#define phys2virt(x) ((uintptr_t)(x) | KERNEL_OFFSET)
#endif

extern char _kernel_physical_base[];
extern char _kernel_unpaged_end[];
extern char _kernel_virt_base[];
extern char _kernel_size[];
extern char _kernel_offset[];

uint32_t pg_roundup(uint32_t);
uint32_t pg_rounddown(uint32_t);
void add_memmap(struct kinfo *, uint64_t, uint64_t);
void cut_memmap(struct kinfo *, uintptr_t, uintptr_t);

void pg_clear(void);
void pg_identity(void);
void pg_clear_identity(void);
void pg_enable(void);
void pg_map(uint32_t, uint32_t, uint32_t, int);
void pg_free(uint32_t, uint32_t);

void flush_tlb(void);
uint32_t vm_enable_paging(uint32_t *);

void *alloc_mem(size_t);
void free_mem(void *, size_t);
void init_mem(struct kinfo *);
void alloc_pt(uint32_t *, uint32_t, size_t, uint32_t, int);

uint32_t *copykvm(void);

static inline uint32_t
read_cr3(void)
{
	uint32_t cr3;
	__asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));

	return cr3;
}

#endif
