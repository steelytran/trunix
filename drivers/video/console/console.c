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

#include <trunix/trunix.h>
#include <trunix/vm.h>
#include <trunix/fs.h>
#include <sys/types.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "console.h"
#include "../fbdev/fbdev.h"

extern struct kinfo k;

int console_open(struct inode *, struct file *);
int console_close(struct inode *, struct file *);
ssize_t console_read(struct file *file, char *buf, size_t, loff_t *);
ssize_t console_write(struct file *, const char *, size_t, loff_t *);

int
console_open(struct inode *inode, struct file *file)
{
	struct file *fb;

	file->data = kmalloc(sizeof(struct console));
	((struct console *)file->data)->buf = alloc_pages(NULL, 0x1000);
	fb = file_open("/dev/fbdev");
	assert(fb != NULL);
	((struct console *)file->data)->fb = fb->data;

	return 0;
}

int
console_close(struct inode *inode, struct file *file)
{
	return -1;
}

ssize_t
console_read(struct file *file, char *buf, size_t sz, loff_t *off)
{
	return -1;
}

ssize_t
console_write(struct file *file, const char *buf, size_t sz, loff_t *off)
{
	int i;
	u8 c;
	u32 x, y;
	u32 *pixel;
	u8 *screen;
	struct fbdev *fb;
	int ydx = 0;
	int xdx = 0;

	if (file->data == NULL)
		return -1;

	fb = ((struct console *)file->data)->fb;
	screen = fb->addr;

	for (i = 0; i < sz; ++i) {
		c = buf[i];

		for (y = 0; y < 16; ++y) 
		for (x = (xdx * 8); x < (xdx + 1) * 8; ++x)
			if (unifont_8x16[c][y] & (1 << (7 - (x % 8)))) {
				pixel = &screen[y * fb->pitch + x * (fb->bpp / 8)];
				*pixel = 0xFFFFFFFF;
			}
		++xdx;
	}

	return 0;
}
