/*
 * keyboard driver
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

#include <trunix/drivers.h>

extern struct kinfo k;


static int kbd_open(struct inode *, struct file *);
static int kbd_close(struct inode *, struct file *);
static ssize_t kbd_read(struct file *file, char *buf, size_t, loff_t *);
static ssize_t kbd_write(struct file *, const char *, size_t, loff_t *);

const struct file_operations keyboard_ops = {
	.open = kbd_open,
	.close = kbd_close,
	.read = kbd_read,
	.write = kbd_write,
};

static int
kbd_open(struct inode *inode, struct file *file)
{

}

static int
kbd_close(struct inode *inode, struct file *file)
{
	return -1;
}

static ssize_t
kbd_read(struct file *file, char *buf, size_t sz, loff_t *off)
{
	return -1;
}

static ssize_t
kbd_write(struct file *file, const char *buf, size_t sz, loff_t *off)
{
	return 0;
}
