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
#include <sys/cdefs.h>

#include "trunix.h"

#define KERNEL_OFFSET ((u32)_kernel_offset)

#define pde2pt(pde) (u32 *)(0xFFC00000 + ((pde) << 12))

#define PG_P 0x0001
#define PG_RW 0x0002
#define PG_US 0x0004
#define PG_PSE 0x0080
#define PG_G 0x0100

#ifdef ASM_FILE
#define virt2phys(x) ((x) - KERNEL_OFFSET)
#define phys2virt(x) ((x) + KERNEL_OFFSET)
#else
#define virt2phys(x) ((u32)(x) & ~KERNEL_OFFSET)
#define phys2virt(x) ((u32)(x) | KERNEL_OFFSET)
#endif

extern char _kernel_physical_base[];
extern char _kernel_unpaged_end[];
extern char _kernel_virt_base[];
extern char _kernel_size[];
extern char _kernel_offset[];

u32 pg_roundup();
u32 pg_rounddown();
add_memmap();
cut_memmap();

pg_clear();
pg_identity();
pg_clear_identity();
pg_enable();
pg_map();
pg_free();

flush_tlb();
u32 vm_enable_paging();

alloc_mem();
free_mem();
init_mem();
alloc_pt();

copykvm();

#endif
