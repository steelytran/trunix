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

#include <sys/file.h>
#include <sys/multiboot.h>
#include <stdint.h>
#include <stddef.h>

#define kmain __k_unpaged_kmain

#define MAXMEMMAP 32

#define cli() __asm__ volatile("cli");
#define sti() __asm__ volatile("sti");

struct kinfo {
	multiboot_info_t mbi;
	multiboot_memory_map_t memmap[MAXMEMMAP];
	int mmap_n;
	size_t mmap_len;
	uint32_t mem_high_phys;
	uintptr_t free_pde_start;
};

void panic(void);
void load_initrd(struct kinfo *);
char *itoa(int, unsigned int);

void init_gdt(void);
void init_tss(void);
void init_idt(void);

uint32_t read_cr4(void);
uint32_t read_cr3(void);
uint32_t read_cr2(void);
uint32_t read_cr1(void);
uint32_t read_cr0(void);
uint32_t read_eax(void);
uint32_t read_ebx(void);
uint32_t read_ecx(void);
uint32_t read_edx(void);
void write_cr4(uint32_t);
void write_cr3(uint32_t);
void write_cr2(uint32_t);
void write_cr1(uint32_t);
void write_cr0(uint32_t);
void write_eax(uint32_t);
void write_ebx(uint32_t);
void write_ecx(uint32_t);
void write_edx(uint32_t);

#endif
