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

#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <sys/multiboot.h>
#include <trunix/trunix.h>
#include <stdint.h>
#include <stddef.h>

#define KERNEL_OFFSET 0xC0000000

#define PROT_NONE 	0x0
#define PROT_READ 	0x1
#define PROT_WRITE 	0x3
/* #define PROT_EXEC */

#define MAP_SHARED 	0x0001
#define MAP_PRIVATE 	0x0002
#define MAP_FIXED 	0x0010
#define MAP_ANON 	0x0020
#define MAP_ANONYMOUS	MAP_ANON
#define MAP_STACK	0x20000

#ifdef ASM_FILE
#define virt2phys(x) ((x) - KERNEL_OFFSET)
#define phys2virt(x) ((x) + KERNEL_OFFSET)
#else
#define virt2phys(x) ((uintptr_t)(x) & ~KERNEL_OFFSET)
#define phys2virt(x) ((uintptr_t)(x) | KERNEL_OFFSET)
#endif

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

uint32_t *pt_alloc(uint32_t *p);

void flush_tlb(void);
uint32_t vm_enable_paging(uint32_t *);
void write_cr3(uintptr_t);

void *mmap(void *, size_t, int, int, int, off_t);
void munmap(void *, size_t);
void init_mem(struct kinfo *);

#endif
