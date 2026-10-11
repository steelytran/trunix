/*
 * console driver
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

#ifndef _CONSOLE_H
#define _CONSOLE_H

#define __NEED_u8
#define __NEED_u32
#include <trunix/types.h>

#include <sys/types.h>
#include <stddef.h>

struct fbdev;
struct file;
struct inode;

extern const u8 misc_fixed_8x13B[128][13]; 
extern const u8 unifont_8x16[128][16];

struct console {
	const void *buf;
	struct fbdev *fb;
};

int console_open(struct inode *, struct file *);
int console_close(struct inode *, struct file *);
ssize_t console_read(struct file *file, char *buf, size_t, loff_t *);
ssize_t console_write(struct file *, const char *, size_t, loff_t *);


#endif
