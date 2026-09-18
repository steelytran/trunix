/*
 * the trunix operating system.
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

#ifndef _SYS_TRUNIX_H
#define _SYS_TRUNIX_H

#include <stdint.h>
#include <stddef.h>
#include <sys/multiboot.h>

#define kmain __k_unpaged_kmain

#define MAXMEMMAP 32
#define ROOT_INO 2

#define cli() __asm__ volatile("cli");
#define sti() __asm__ volatile("sti");

struct kinfo {
	multiboot_info_t mbi;
	multiboot_memory_map_t memmap[MAXMEMMAP];
	int mmap_n;
	size_t mmap_len;
	uint32_t mem_high_phys;
	uintptr_t free_pde_start;

	uintptr_t initrd_addr;
	size_t initrd_len;

	unsigned int ino_n;
	struct inode *ino_table;
	struct dirent *dir_table;
};

struct dirent {
	unsigned int ino_id;
#define DIRSIZ 128
	char name[DIRSIZ];
};

struct inode {
	unsigned short mode;
	unsigned short nlink;
	short uid;
	short gid;
	size_t size;
	long a_time, m_time, s_time;
	unsigned int major, minor;
	unsigned int id;
	enum {FILE, LINK, CHAR, BLK, DIR, FIFO} type;
	uintptr_t addr;
};

void panic(void);

void ustar2fs(struct kinfo *);

char *itoa(int, unsigned int);

void init_gdt(void);
void init_tss(void);
void init_idt(void);

#endif
