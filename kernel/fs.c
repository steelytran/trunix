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

#include <trunix/trunix.h>
#include <trunix/vm.h>
#include <trunix/fs.h>
#include <trunix/proc.h>
#include <sys/cdefs.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

#include "../drivers/char/console/console.h"
#include "../drivers/char/fbdev/fbdev.h"

extern struct kinfo k;

const struct file_operations console_ops = {
	.open = console_open,
	.close = console_close,
	.read = console_read,
	.write = console_write,
};

const struct file_operations fbdev_ops = {
	.open = fbdev_open,
	.close = fbdev_close,
	.read = fbdev_read,
	.write = fbdev_write,
	.mmap = fbdev_mmap,
};

const struct file_operations *cdev_ops[] = {
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

	struct inode *in = alloc_pages(NULL, 0x2000);
	struct dirent *dir = alloc_pages(NULL, 0x2000);

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

		in[i].major = oct2int(&p[329], 7);
		/* in[i].minor = oct2int(&p[337], 7); */

		if (in[i].size > 0)
			in[i].addr = &p[512];
		else
			in[i].addr = 0;


		++i;
	}

	dir[i].inode = i;
	strlcpy(dir[i].name, "/dev/console", DIRSIZ);
	in[i].type = V_CHAR;
	in[i].major = CONSOLE;

	++i;
	dir[i].inode = i;
	strlcpy(dir[i].name, "/dev/fbdev", DIRSIZ);
	in[i].type = V_CHAR;
	in[i].major = FRAMEBUFFER;

	k.ino_n = i + 1;
}

int
sys_open(const char* path, int oflag, ...)
{
	int fd, i;
	struct file *f;
	struct proc *p = getproc();

	assert(p != NULL);

	for (i = ROOT_INO; i < k.ino_n; ++i)
		if (strcmp(path, k.dir_tbl[i].name) == 0)
			goto found;

	return -1;

found:
	for (fd = 0; p->ofile[fd] != NULL; ++fd)
		;

	if (fd == MAXFILES)
		return -1;

	f = kmalloc(sizeof(struct file));
	p->ofile[fd] = f;
	f->inode = &k.ino_tbl[k.dir_tbl[i].inode];

	(*cdev_ops[f->inode->major]->open)(f->inode, f);

	return fd;
}

int
sys_write(int fd, const void *buf, size_t count)
{
	struct file *f;
	struct proc *p = getproc();

	f = p->ofile[fd];

	if (f == NULL)
		return -1;

	(*cdev_ops[f->inode->major]->write)(f, buf, count, 0);

	return 0;
}
