#ifndef TRUNIX_H
#define TRUNIX_H

#include <multiboot.h>
#include <stdint.h>

#define kmain __k_unpaged_kmain

struct kinfo {
	multiboot_info_t	mbi;
	multiboot_memory_map_t	memmap[32];
	uintptr_t		free_pde_start;
};

void panic(const char *);

void pg_clear(void);
void pg_identity(void);
int pg_mapkernel(void);
void pg_enable(void);
uint32_t vm_enable_paging(uint32_t *);

void init_gdt(void);
void init_tss(void);
void init_idt(void);

#endif
