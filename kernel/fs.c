/*
 * initial ramdisk and vfs
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
#include <trunix/trunix.h>
#include <trunix/vm.h>
#include <trunix/fs.h>
#include <trunix/proc.h>
#include <sys/cdefs.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

#include "../drivers/video/console/console.h"
#include "../drivers/video/fbdev/fbdev.h"

extern struct kinfo k;

const struct file_operations *cdevs[] = {
	[CONSOLE] = &console_ops,
	[FRAMEBUFFER] = &fbdev_ops,
};

static int
oct2int(unsigned char *str, unsigned len)
{
	int n = 0;
	unsigned char *c = str;

	while (len-- > 0) {
		if (*c == '\0')
			break;

		n <<= 3;
		n += *c - '0';
		++c;
	}

	return n;
}

void
load_initrd()
{
	u32 len = k.initrd_end - k.initrd_start;
	void *tar = (void *)p2v(k.initrd_start);
	u8 *p = tar;
	unsigned int i = ROOT_INO;

	struct inode *in = alloc_pages(0x2000);
	struct dirent *dir = alloc_pages(0x2000);

	k.ino_tbl = in;
	k.dir_tbl = dir;

	for (; p < (u8 *)tar + len; p += 512) {
		if (memcmp("ustar\00000", &p[257], 8) != 0)
			continue;

		dir[i].inode = i;
		strlcpy(dir[i].name, &p[1], DIRSIZ);

		in[i].mode = oct2int(&p[100], 6);
		in[i].uid = oct2int(&p[108], 6);
		in[i].gid = oct2int(&p[116], 6);
		in[i].size = oct2int(&p[124], 11);
		in[i].m_time = oct2int(&p[136], 11);

		switch (p[156]) {
		case '0': /* FALLTHROUGH */
		case '\0': /* FALLTHROUGH */
		case '1':
			in[i].type = V_FILE;
			break;
		case '2':
			in[i].type = V_LINK;
			break;
		case '3':
			in[i].type = V_CHAR;
			break;
		case '4':
			in[i].type = V_BLK;
			break;
		case '5':
			in[i].type = V_DIR;
			break;
		case '6':
			in[i].type = V_FIFO;
			break;
		}

		in[i].major = oct2int(&p[329], 6);

		if (in[i].size > 0)
			in[i].addr = &p[512];
		else
			in[i].addr = 0;

		++i;
	}
	k.ino_n = i;
}

struct file *
file_open(const char *path)
{
	int i;
	struct file *f;

	for (i = ROOT_INO; i < k.ino_n; ++i)
		if (strcmp(path, k.dir_tbl[i].name) == 0)
			goto found;

	return NULL;

found:
	f = kmalloc(sizeof(struct file));
	assert(f != NULL);
	f->inode = &k.ino_tbl[k.dir_tbl[i].inode];

	(*cdevs[f->inode->major]->open)(f->inode, f);
	return f;
}

int
sys_open(const char* path, int oflag, ...)
{
	int fd, i;
	struct file *f;
	struct proc *p;

	p = getproc();
	assert(p != NULL);

	for (i = ROOT_INO; i < k.ino_n; ++i)
		if (strcmp(path, k.dir_tbl[i].name) == 0)
			goto found;

	return -1;

found:
	for (fd = 0; p->ofile[fd] != NULL; ++fd)
		if (fd >= MAXFILES)
			return -1;

	f = kmalloc(sizeof(struct file));
	p->ofile[fd] = f;
	f->inode = &k.ino_tbl[k.dir_tbl[i].inode];

	(*cdevs[f->inode->major]->open)(f->inode, f);

	return fd;
}

int
sys_read(int fd, void *buf, size_t count)
{
	struct file *f;
	struct proc *p;

	p = getproc();
	f = p->ofile[fd];

	if (f == NULL)
		return -1;

	(*cdevs[f->inode->major]->read)(f, buf, count, 0);

	return 0;
}

int
sys_write(int fd, const void *buf, size_t count)
{
	struct file *f;
	struct proc *p;

	p = getproc();
	f = p->ofile[fd];

	if (f == NULL)
		return -1;

	(*cdevs[f->inode->major]->write)(f, buf, count, 0);

	return 0;
}
