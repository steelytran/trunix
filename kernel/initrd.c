/*
 * initial ramdisk
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
#include <sys/mman.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

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
load_initrd(struct kinfo *k)
{
	void *node;

	unsigned int i = 2;
	void *tar = mmap((void *)k->initrd_addr,
	     k->initrd_len,
	     PROT_WRITE,
	     0, 0, 0
	);

	uint8_t *p = tar;
	size_t len = k->initrd_len;

	struct inode *in = mmap(NULL, 0x2000, PROT_WRITE, 0, 0, 0);
	struct tnode *tn = mmap(NULL, 0x2000, PROT_WRITE, 0, 0, 0);

	k->inodes = in;
	k->tnodes = tn;

	for (; p < (uint8_t *)tar + len; p += 512) {
		if (memcmp("ustar\00000", &p[257], 8) != 0)
			continue;

		tn[i].ino_id = i;
		strncpy(tn[i].name, &p[1], DIRSIZ);

		in[i].mode = oct2int(&p[100], 6);
		in[i].uid = oct2int(&p[108], 6);
		in[i].gid = oct2int(&p[116], 6);
		in[i].size = oct2int(&p[124], 11);
		in[i].m_time = oct2int(&p[136], 11);

		switch (p[156]) {
		case '0': /* FALLTHROUGH */
		case '1':
			in[i].type = VFILE;
			break;
		case '2':
			in[i].type = VLINK;
			break;
		case '3':
			in[i].type = VCHAR;
			break;
		case '4':
			in[i].type = VBLK;
			break;
		case '5':
			in[i].type = VDIR;
			break;
		case '6':
			in[i].type = VFIFO;
			break;
		}

		in[i].major = oct2int(&p[329], 7);
		in[i].minor = oct2int(&p[337], 7);

		if (in[i].size > 0)
			in[i].addr = (uintptr_t)&p[512];

		++i;
	}

	k->ino_n = i;
}
