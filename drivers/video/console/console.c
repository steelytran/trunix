/*
 * fbcon driver
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

/* hopefully a very temporary solution... */
#define FONT unifont_8x16
#define FONT_W 8
#define FONT_H 16
#define SCALE 2
#define CELL_W (FONT_W * SCALE)
#define CELL_H (FONT_H * SCALE)

#define COORD(fb, x, y)\
    (((u8 *)(fb)->addr) + ((y) * (fb)->pitch + (x) * ((fb)->bpp / 8)))

extern struct kinfo k;

struct fbdev *fb;

int fbcon_open(struct inode *, struct file *);
int fbcon_close(struct inode *, struct file *);
ssize_t fbcon_read(struct file *file, char *buf, size_t, loff_t *);
ssize_t fbcon_write(struct file *, const char *, size_t, loff_t *);

static void
fbconchar(int xpos, int ypos, char c)
{
	int i;
	u32 x, y;
	u8 *scr;
	u32 *pixel;
	int px, py;
	int sx, sy;
	int rows, cols;

	scr = fb->addr;

	for (y = 0; y < FONT_H; ++y)
	for (x = 0; x < FONT_W; ++x) {
		px = SCALE * x + xpos * CELL_W;
		py = SCALE * y + ypos * CELL_H;

		for (sx = px; sx < px + SCALE; ++sx)
		for (sy = py; sy < py + SCALE; ++sy) {
			pixel = COORD(fb, sx, sy);

			if (FONT[c][y] & (1 << ((FONT_W - 1) - (x % FONT_W))))
				*pixel = ~0;
			else
				*pixel = 0;
		}
	}
}

int
fbcon_open(struct inode *inode, struct file *file)
{
	struct file *fb;

	file->data = kmalloc(sizeof(struct console));
	((struct console *)file->data)->buf = alloc_pages(0x1000);
	fb = file_open("/dev/fb");
	assert(fb != NULL);
	((struct console *)file->data)->fb = fb->data;

	return 0;
}

int
fbcon_close(struct inode *inode, struct file *file)
{
	return -1;
}

ssize_t
fbcon_read(struct file *file, char *buf, size_t sz, loff_t *off)
{
	return -1;
}

ssize_t
fbcon_write(struct file *file, const char *buf, size_t sz, loff_t *off)
{
	int i;
	int rows, cols;
	static int x = 0;
	static int y = 0;

	if (file->data == NULL)
		return -1;

	fb = ((struct console *)file->data)->fb;

	cols = fb->width / CELL_W;
	rows = fb->height / CELL_H;

	for (i = 0; i < sz; ++i) {
		if (x > cols || buf[i] == '\n') {
			++y;
			x = 0;
		} else if (buf[i] == '\b' && x > 0)
			--x;
		else
			fbconchar(x++, y, buf[i]);
	}

	return 0;
}
