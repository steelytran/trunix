/*
 * initialization for kernel, mapping to virtual memory.
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

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include <sys/multiboot.h>
#include <sys/trunix.h>
#include <sys/mman.h>
#include <sys/tty.h>

extern uint32_t _kernel_physical_base;
extern uint32_t _kernel_unpaged_end;

struct kinfo k;

struct kinfo *
init_trunix(multiboot_info_t *mb_info, uint32_t magic)
{
	int m;
	multiboot_memory_map_t *mmap;

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
		panic();

	if (!(mb_info->flags >> 6 & 1))
		panic();

	k.mbi = *mb_info;

	for (m = 0; m < mb_info->mmap_length;
	     m += mmap->size + sizeof(mmap->size)) {

		mmap = (multiboot_memory_map_t *)(mb_info->mmap_addr + m);

		if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
			add_memmap(&k,
			    mmap->addr,
			    mmap->len);

	}

	cut_memmap(&k,
	    (uintptr_t)&_kernel_physical_base,
	    (uintptr_t)&_kernel_unpaged_end);

	/* setup paging */
	pg_clear();
	pg_identity();
	pg_enable();

	return &k;
}
