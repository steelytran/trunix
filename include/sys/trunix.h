#ifndef TRUNIX_H
#define TRUNIX_H

#include <stdint.h>
#include <stddef.h>
#include <sys/multiboot.h>

#define kmain __k_unpaged_kmain

#define KERNEL_OFFSET 0xC0000000

#ifdef ASM_FILE
#define virt2phys(x) ((x) - KERNEL_OFFSET)
#define phys2virt(x) ((x) + KERNEL_OFFSET)
#else
#define virt2phys(x) ((uintptr_t)(x) & ~KERNEL_OFFSET)
#define phys2virt(x) ((uintptr_t)(x) | KERNEL_OFFSET)
#endif

#define MAXMEMMAP 32

struct kinfo {
	multiboot_info_t mbi;
	multiboot_memory_map_t memmap[MAXMEMMAP];
	int mmap_n;
	size_t mmap_len;
	uint32_t mem_high_phys;
	uintptr_t free_pde_start;
};

struct tss_entry {
	uint32_t prev_tss; 
	uint32_t esp0;     
	uint32_t ss0;      
	uint32_t esp1; 
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iopb;
}__attribute__((packed));

void panic(void);

uint32_t pg_roundup(uint32_t);
uint32_t pg_rounddown(uint32_t);
void add_memmap(struct kinfo *, uint64_t, uint64_t);
void cut_memmap(struct kinfo *, uintptr_t, uintptr_t);

void pg_clear(void);
void pg_identity(void);
void pg_clear_identity(void);
void pg_enable(void);
void pg_map(uint32_t, uint32_t, uint32_t);
void pg_free(uint32_t, uint32_t);

uint32_t *pt_alloc(uint32_t *p);

void flush_tlb(void);
uint32_t vm_enable_paging(uint32_t *);
void write_cr3(uintptr_t);

void *alloc_mem(unsigned int);
void free_mem(void *, size_t);
void init_mem(struct kinfo *);

void init_gdt(void);
void init_tss(void);
void init_idt(void);

#endif
