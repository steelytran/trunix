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

#include <trunix/trunix.h>
#include <trunix/vm.h>
#include <sys/multiboot.h>
#include <sys/cdefs.h>
#include <stddef.h>

#define FLAGS MULTIBOOT_INFO_MEM_MAP | MULTIBOOT_INFO_FRAMEBUFFER_INFO

struct kinfo k;

struct kinfo *
init_trunix(struct multiboot_info *mb_info, u32 magic)
{
	u32 m;
	struct multiboot_mmap_entry *mmap;
	struct multiboot_mod_list *fs;

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
		return NULL;

	if (!(mb_info->flags & FLAGS))
		return NULL;

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

	fs = (struct multiboot_mod_list *)mb_info->mods_addr;
	cut_memmap(&k,
	    k.initrd_start = fs->mod_start,
	    k.initrd_end = fs->mod_end);

	/* setup paging */
	pg_clear();
	pg_identity(&k);
	pg_enable();

	return &k;
}
