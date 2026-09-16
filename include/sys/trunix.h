#ifndef _TRUNIX_H
#define _TRUNIX_H

#include <stdint.h>
#include <stddef.h>
#include <sys/multiboot.h>

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

void init_gdt(void);
void init_tss(void);
void init_idt(void);

#endif
