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

#ifndef _TRUNIX_FS_H
#define _TRUNIX_FS_H

#include <trunix/types.h>
#include <sys/types.h>

#define DIRSIZ 128
#define ROOT_INO 2

enum major_ids { 
	CONSOLE,
	FRAMEBUFFER,
	MEMORY,
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
	dev_t major;
	dev_t minor;
	enum {V_FILE, V_LINK, V_CHAR, V_BLK, V_DIR, V_FIFO} type;
	u32 size;
	u32 id;
	u32 addr;
	long m_time;
};

struct file_operations {
	int (*open)(struct inode *, struct file *);
	int (*close)(struct inode *, struct file *);
	ssize_t (*read)(struct file *, char *, size_t, loff_t *);
	ssize_t (*write)(struct file *, const char *, size_t, loff_t *);
	int (*mmap)(struct file *, struct vm_region *);
	int (*ioctl)(struct inode *, struct file *, int, void *);
};
extern const struct file_operations *cdev_ops[]; 

int sys_open(const char*, int, ...);
struct file *file_open(const char *);

#endif
