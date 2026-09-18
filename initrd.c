/*
 * virtual file system
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

#include <sys/trunix.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

unsigned long ino_cnt = 0;

struct inode *
dirlookup(struct inode *dir, char *name)
{
	struct dirent de;
	return NULL;
}

static int
oct2int(unsigned char *str, int len)
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
ustar2fs(struct kinfo *k)
{
	void *file;

	unsigned int i = 2;
	void *tar = mmap((void *)k->initrd_addr,
	     k->initrd_len,
	     PROT_WRITE,
	     0, 0, 0
	);

	uint8_t *p = tar;
	size_t len = k->initrd_len;

	struct inode *inodes = mmap(NULL, 0x2000, PROT_WRITE, 0, 0, 0);
	struct dirent *dirents = mmap(NULL, 0x2000, PROT_WRITE, 0, 0, 0);

	k->ino_table = inodes;
	k->dir_table = dirents;

	for (; p < (uint8_t *)tar + len; p += 512) {
		if (memcmp("ustar\00000", &p[257], 8) != 0)
			continue;

		dirents[i].ino_id = i;
		strncpy(dirents[i].name, &p[0], DIRSIZ);

		inodes[i].mode = oct2int(&p[100], 6);
		inodes[i].uid = oct2int(&p[108], 6);
		inodes[i].gid = oct2int(&p[116], 6);
		inodes[i].size = oct2int(&p[124], 11);

		switch (p[156]) {
		case '0': /* FALLTHROUGH */
		case '1':
			inodes[i].type = FILE;
			break;
		case '2':
			inodes[i].type = LINK;
			break;
		case '3':
			inodes[i].type = CHAR;
			break;
		case '4':
			inodes[i].type = BLK;
			break;
		case '5':
			inodes[i].type = DIR;
			break;
		case '6':
			inodes[i].type = FIFO;
			break;
		}

		inodes[i].major = oct2int(&p[329], 7);
		inodes[i].minor = oct2int(&p[337], 7);

		if (inodes[i].size > 0 && inodes[i].type == FILE)
			inodes[i].addr = (uintptr_t)&p[512];

		++i;
	}

	k->ino_n = i;
}
