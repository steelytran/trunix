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

#include <sys/multiboot.h>
#include <sys/cdefs.h>
#include <stdio.h>

#include "trunix.h"
#include "vm.h"

struct kinfo k;

init_trunix(mb_info, magic)
struct multiboot_info *mb_info;
u32 magic;
{
	unsigned long m;
	struct multiboot_mmap_entry *mmap;
	struct multiboot_mod_list *fs;

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
		return -1;

	if (!(mb_info->flags >> 6 & 1))
		return -1;

	k.mbi = *mb_info;

	for (m = 0; m < mb_info->mmap_length;
	     m += mmap->size + sizeof(mmap->size)) {

		mmap = (struct multiboot_mmap_entry *)(mb_info->mmap_addr + m);

		if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
			add_memmap(&k,
			    mmap->addr,
			    mmap->len);

	}

	cut_memmap(&k,
	    (unsigned long)_kernel_physical_base,
	    (unsigned long)_kernel_unpaged_end);

/*
 * temporary solution to make null
 * pointers actually work, could put
 * something more useful here..
 */
	cut_memmap(&k, 0x0000, 0x1000);

	/* initrd */
	fs = (struct multiboot_mod_list *)mb_info->mods_addr;
	k.initrd_start = fs->mod_start;
	k.initrd_end = fs->mod_end;

	/* setup paging */
	pg_clear();
	pg_identity();
	pg_enable();

	return &k;
}
