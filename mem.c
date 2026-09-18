/*
 * physical page frame allocation
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

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <sys/trunix.h>
#include <sys/mman.h>

#define pde2pt(pde) (uint32_t *)(0xFFC00000 + ((pde) << 12))

extern uint32_t _kernel_physical_base;
extern uint32_t _kernel_unpaged_end;
extern uint32_t _kernel_virt_base;
extern uint32_t _kernel_size;

uint32_t kernel_physical_start = (uint32_t)&_kernel_physical_base;
uint32_t kernel_physical_end = (uint32_t)&_kernel_unpaged_end;
uint32_t kernel_virt_start = (uint32_t)&_kernel_virt_base;
uint32_t kernel_len = (uint32_t)&_kernel_size;

static uint32_t pagedir[1024] __attribute__((aligned(0x1000)));
uint32_t* virtpagedir;

void pg_clear(void);
void pg_identity(void);
void pg_clear_identity(void);
void pg_enable(void);
void pg_map(uint32_t, uint32_t, uint32_t, int);
void pg_free(uint32_t, uint32_t);

uint32_t *pt_alloc(uint32_t *p);

void add_memmap(struct kinfo *, uint64_t, uint64_t);
void cut_memmap(struct kinfo *, uintptr_t, uintptr_t);

uint32_t pg_roundup(uint32_t);
uint32_t pg_rounddown(uint32_t);

/*
 * round value up to page boundaries.
 */
uint32_t
pg_roundup(uint32_t x)
{
	uint32_t p = x % 0x1000;

	if (p == 0)
		return x;

	return x + 0x1000 - p;
}

/*
 * round value down to page boundaries.
 */
uint32_t
pg_rounddown(uint32_t x)
{
	uint32_t p = x % 0x1000;

	if (p == 0)
		return x;

	return x - p;
}

/*
 * add memory map entry 
 */
void
add_memmap(struct kinfo *k, uint64_t addr, uint64_t len)
{
	int m;
	uint32_t highmark;
	if (len == 0)
		return;

	addr = pg_roundup(addr);
	len = pg_rounddown(len);

	for (m = 0; m < MAXMEMMAP; ++m) {
		if (k->memmap[m].len)
			continue;

		memcpy(&k->memmap[m].addr, &addr, sizeof(uint64_t));
		memcpy(&k->memmap[m].len, &len, sizeof(uint64_t));
		k->memmap[m].type = MULTIBOOT_MEMORY_AVAILABLE;

		if (m >= k->mmap_n)
			k->mmap_n = m + 1;

		highmark = addr + len;
		if (highmark > k->mem_high_phys)
			k->mem_high_phys = highmark;

		return;
	}

	panic(); /* no available memmap slot */
}

void
cut_memmap(struct kinfo *k, uintptr_t start, uintptr_t end)
{
	int m;
	uintptr_t cut_start, cut_end;
	uintptr_t mme_start, mme_end;

	start = pg_rounddown(start);
	end = pg_roundup(end);

	for (m = 0; m < k->mmap_n; ++m) {
		cut_start = start;
		cut_end = end;

		mme_start = (uintptr_t)k->memmap[m].addr;
		mme_end = (uintptr_t)(k->memmap[m].addr + k->memmap[m].len);

		if (cut_start < mme_start)
			cut_start = mme_start;
		if (cut_end > mme_end)
			cut_end = mme_end;
		if (cut_start >= cut_end)
			continue;

		/* remove overlap */
		k->memmap[m].addr = k->memmap[m].len = 0;
		if (cut_start > mme_start)
			add_memmap(k, mme_start, cut_start - mme_start);
		if (cut_end < mme_end)
			add_memmap(k, cut_end, mme_end - cut_end);
	}
}

void
pg_clear(void)
{
	memsetl(pagedir, 0x00000002, sizeof(pagedir));
	return;
}

/*
 * identity map from the start of
 * memory to the end of the kernel.
 */
void
pg_identity(void)
{
	uint32_t i;
	uint32_t *pt, ph = 0;
	pt = pt_alloc(&ph);

	for (i = 0; i < 1024; ++i) {
		/* r/w, present*/
		pt[i] = (i << 12) | 0b00000011;
	}

	pt[1023] = 0xB8000 | 0b00000011;

	pagedir[0] = (uint32_t)ph | 0b00000011;
	pagedir[768] = (uint32_t)ph | 0b00000011;
	pagedir[1023] = (uint32_t)pagedir | 0b00000011;
}

void
pg_clear_identity(void)
{
	int i;
	uint32_t *pt;

	pagedir[0] = 0b00000010;
	pt = pde2pt(768);

	for (i = 0; i < (kernel_physical_start >> 12); ++i)
		/* r/w, not present*/
		pt[i] = 0b00000010;

	virtpagedir = (uint32_t *)0xFFFFF000;
	flush_tlb();
}

void
pg_enable(void)
{
	vm_enable_paging(pagedir);
}


void
pg_map(uint32_t p_addr, uint32_t v_start, uint32_t v_end, int prot)
{
	uint32_t *pt;
	uint32_t frame, ph;
	int pte, pde;

	v_start = pg_rounddown(v_start);
	p_addr = pg_rounddown(p_addr);

	for (; v_start < v_end; v_start += 0x1000) {
		frame = p_addr;

		pde = v_start >> 22;
		pte = (v_start >> 12) & 0x3FF;

		if (!(virtpagedir[pde] & 1)) {
			pt = pt_alloc(&ph);
			virtpagedir[pde] = (ph & 0xFFFFF000) | prot;
		} else
			pt = pde2pt(pde);

		pt[pte] = (frame & 0xFFFFF000) | prot;
		p_addr += 0x1000;
	}

	flush_tlb();
}

void
pg_free(uint32_t start, uint32_t end)
{
	uint32_t *pt;
	int pte, pde;

	start = pg_rounddown(start);

	for (; start < end; start += 0x1000) {
		pde = start >> 22;
		pt = pde2pt(pde);
		pte = (start >> 12) & 0x3FF;
		pt[pte] = 0b00000010;
	}

	flush_tlb();
}

uint32_t *
pt_alloc(uint32_t *p)
{
	uint32_t *t;
	static uint32_t pagetable[6][1024] __attribute__((aligned(0x1000)));
	static int used_pt_n = 0;

	if (used_pt_n >= 6)
		panic();

	t = pagetable[used_pt_n++];
	*p = virt2phys(t);

	return t;
}
