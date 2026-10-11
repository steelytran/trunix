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

#ifndef _FBDEV_H
#define _FBDEV_H

#define __NEED_u8
#define __NEED_u16
#define __NEED_u32
#include <trunix/types.h>

#include <trunix/fs.h>
#include <sys/types.h>

struct fbdev {
	u32 addr;
	u32 pitch;
	u32 width;
	u32 height;
	u8 bpp;
	u8 type;
	union {
		struct {
			u32 palette_addr;
			u16 palette_num_colors;
		};
		struct {
			u8 red_field_position;
			u8 red_mask_size;
			u8 green_field_position;
			u8 green_mask_size;
			u8 blue_field_position;
			u8 blue_mask_size;
		};
	};
};

int fbdev_open(struct inode *, struct file *);
int fbdev_close(struct inode *, struct file *);
ssize_t fbdev_read(struct file *file, char *buf, size_t, loff_t *);
ssize_t fbdev_write(struct file *, const char *, size_t, loff_t *);
int fbdev_mmap(struct file *, struct vm_region *);

#endif
