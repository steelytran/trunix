/*
 * memory driver interface
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
#include <trunix/fs.h>
#include <sys/types.h>
#include <stddef.h>
#include <string.h>

int memory_open(struct inode *, struct file *);
int memory_close(struct inode *, struct file *);
ssize_t memory_read(struct file *file, char *buf, size_t, loff_t *);
ssize_t memory_write(struct file *, const char *, size_t, loff_t *);
ssize_t null_read(struct file *file, char *buf, size_t, loff_t *);
ssize_t null_write(struct file *, const char *, size_t, loff_t *);
ssize_t zero_read(struct file *file, char *buf, size_t, loff_t *);
ssize_t zero_write(struct file *, const char *, size_t, loff_t *);

int
memory_open(struct inode *inode, struct file *file)
{
	struct file *fb;

	return 0;
}

int
memory_close(struct inode *inode, struct file *file)
{
	return -1;
}

ssize_t
memory_read(struct file *file, char *buf, size_t sz, loff_t *off)
{
	return -1;
}

ssize_t
memory_write(struct file *file, const char *buf, size_t sz, loff_t *off)
{
	return 0;
}

/*
 * /dev/null
 */

ssize_t
null_read(struct file *file, char *buf, size_t sz, loff_t *off)
{
	return -1;
}

ssize_t
null_write(struct file *file, const char *buf, size_t sz, loff_t *off)
{
	return sz;
}

/*
 * /dev/zero
 */

ssize_t
zero_read(struct file *file, char *buf, size_t sz, loff_t *off)
{
	memset(buf, 0, sz);
	return sz;
}

ssize_t
zero_write(struct file *file, const char *buf, size_t sz, loff_t *off)
{
	return sz;
}
