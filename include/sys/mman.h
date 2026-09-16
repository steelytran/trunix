#ifndef _MMAN_H
#define _MMAN_H

#include <stdint.h>
#include <stddef.h>
#include <sys/multiboot.h>

#define KERNEL_OFFSET 0xC0000000

#ifdef ASM_FILE
#define virt2phys(x) ((x) - KERNEL_OFFSET)
#define phys2virt(x) ((x) + KERNEL_OFFSET)
#else
#define virt2phys(x) ((uintptr_t)(x) & ~KERNEL_OFFSET)
#define phys2virt(x) ((uintptr_t)(x) | KERNEL_OFFSET)
#endif

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

#endif
