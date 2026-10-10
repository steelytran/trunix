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

#include <trunix/vm.h>
#include <trunix/fs.h>
#include <stddef.h>

#include "console.h"

extern struct kinfo k;
extern char VGA_TEXT[];

int
console_open(struct inode *inode, struct file *file)
{
	if (file->data == NULL) {
		file->data = kmalloc(sizeof(struct console));
		((struct console *)file->data)->buf = alloc_pages(NULL, 0x1000);
	}

	return 0;
}

int
console_close(struct inode *inode, struct file *file)
{
	return -1;
}

int
console_read(struct file *file, char *buf, u32 sz, u32 *off)
{
	return -1;
}

int
console_write(struct file *file, const char *buf, u32 sz, u32 *off)
{
	return -1;
}
