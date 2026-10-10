/*
 * framebuffer driver
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

#include <trunix/vm.h>
#include <trunix/fs.h>
#include <sys/multiboot.h>
#include <stddef.h>
#include <string.h>

#include "fbdev.h"

extern struct kinfo k;

int
fbdev_open(struct inode *inode, struct file *file)
{
	struct fbdev *fb;
	struct multiboot_info mbi = k.mbi;

	if (file->data == NULL) {
		fb = kmalloc(sizeof(struct fbdev));
		*fb = (struct fbdev) {
			.addr = (u32)mbi.framebuffer_addr,
			.pitch = mbi.framebuffer_pitch,
			.width = mbi.framebuffer_width,
			.height = mbi.framebuffer_height,
			.bpp = mbi.framebuffer_bpp,
			.type = mbi.framebuffer_type
		};

		switch (fb->type) {
		case (MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED):
			fb->palette_addr = mbi.framebuffer_palette_addr;
			fb->palette_num_colors = mbi.framebuffer_palette_num_colors;
			break;
		case (MULTIBOOT_FRAMEBUFFER_TYPE_RGB):
			fb->red_field_position = mbi.framebuffer_red_field_position;
			fb->red_mask_size = mbi.framebuffer_red_mask_size;
			fb->green_field_position = mbi.framebuffer_green_field_position;
			fb->green_mask_size = mbi.framebuffer_green_mask_size;
			fb->blue_field_position = mbi.framebuffer_blue_field_position;
			fb->blue_mask_size = mbi.framebuffer_blue_mask_size;
		}

		file->data = fb;
	}
	return 0;
}

int
fbdev_close(struct inode *inode, struct file *file)
{
	return -1;
}

int
fbdev_read(struct file *file, char *buf, u32 sz, u32 *off)
{
	return -1;
}

int
fbdev_write(struct file *file, const char *buf, u32 sz, u32 *off)
{
	if (file->data == NULL)
		return -1;

	memcpy(((struct fbdev *)file->data)->addr, buf, sz);
}

int
fbdev_mmap(struct file *file, struct vm_region *mem)
{
	if (file->data == NULL)
		return -1;

	mem->pa = ((struct fbdev *)file->data)->addr;
	return 0;
}
