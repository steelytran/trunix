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

#ifndef _FILE_H
#define _FILE_H

#include <sys/cdefs.h>

#define MAXMEMMAP 32
#define DIRSIZ 128
#define ROOT_INO 2

enum major_ids { 
	CONSOLE
};

struct file {
	enum {FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE} type;
	int ref;
	struct inode *ip;
	int off;
	unsigned int major;
};

struct devrw {
	int (*read)(int, u32, int);
	int (*write)(int, u32, int);
};

extern struct devrw devrw[];

struct dirent {
	unsigned int ino_id;
	char name[DIRSIZ];
};

struct inode {
	unsigned short mode;
	unsigned short nlink;
	short uid;
	short gid;
	u32 size;
	long a_time, m_time, s_time;
	unsigned int major, minor;
	unsigned int id;
	enum {VFILE, VLINK, VCHAR, VBLK, VDIR, VFIFO} type;
	u32 addr;
};

#endif
