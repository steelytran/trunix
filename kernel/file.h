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

#define DIRSIZ 128
#define ROOT_INO 2
#define MAXFILES 16

enum major_ids { 
	CONSOLE,
	FRAMEBUFFER,
};

struct dirent {
	u32 inode;
	char name[DIRSIZ];
};

struct file {
	struct file_operations *ops;
	struct inode *inode;
	void *data;
};

struct inode {
	short uid;
	short gid;
	u16 mode;
	u16 nlink;
	enum major_ids major;
	enum {V_FILE, V_LINK, V_CHAR, V_BLK, V_DIR, V_FIFO} type;
	u32 size;
	u32 id;
	u32 addr;
	long m_time;
};

struct file_operations {
	int (*open) (struct inode *, struct file *);
	int (*close) (struct inode *, struct file *);
	int (*read) (struct file *, char *, u32, u32 *);
	int (*write) (struct file *, const char *, u32, u32 *);
};

int sys_open(const char* path, int oflag, ...);

#endif
