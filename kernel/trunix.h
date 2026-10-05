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

#ifndef _TRUNIX_H
#define _TRUNIX_H

#include <sys/multiboot.h>
#include <sys/cdefs.h>

#include "file.h"

#define kmain __k_unpaged_kmain

#define MAXMEMMAP 32

#define cli() __asm__ volatile("cli")
#define sti() __asm__ volatile("sti")
#define hlt() __asm__ volatile("hlt")

struct task_state_segment {
	u32 prev_tss;
	u32 esp0;
	u32 ss0;
	u32 esp1;
	u32 ss1;
	u32 esp2;
	u32 ss2;
	u32 cr3;
	u32 eip;
	u32 eflags;
	u32 eax;
	u32 ecx;
	u32 edx;
	u32 ebx;
	u32 esp;
	u32 ebp;
	u32 esi;
	u32 edi;
	u32 es;
	u32 cs;
	u32 ss;
	u32 ds;
	u32 fs;
	u32 gs;
	u32 ldt;
	u16 trap;
	u16 iomap_base;
};
extern struct task_state_segment tss;

struct kinfo {
	struct multiboot_info mbi;
	struct multiboot_mmap_entry memmap[MAXMEMMAP];
	int mmap_n;
	u32 mmap_len;
	u32 mem_high_phys;
	u32 free_pde_start;

	u32 initrd_start;
	u32 initrd_end;

	unsigned int ino_n;
	struct inode *ino_tbl;
	struct dirent *dir_tbl;
};

void panic();
void load_initrd();
char *itoa(int, unsigned);

void init_serial();
u8 read_serial();
void write_serial(u8);
void debug(const char *, ...);

void init_gdt();
void init_tss();
void init_idt();

u32 read_cr4();
u32 read_cr2();
u32 read_cr1();
u32 read_cr0();
u32 read_eax();
u32 read_ebx();
u32 read_ecx();
u32 read_edx();
write_cr4(u32);
write_cr3(u32);
write_cr2(u32);
write_cr1(u32);
write_cr0(u32);
write_eax(u32);
write_ebx(u32);
write_ecx(u32);
write_edx(u32);

#endif
