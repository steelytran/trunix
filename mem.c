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

#include <sys/trunix.h>

extern uint32_t _kernel_physical_base;
extern uint32_t _kernel_unpaged_end;
extern uint32_t _kernel_virt_base;
extern uint32_t _kernel_size;

uint32_t kernel_physical_start = (uint32_t)&_kernel_physical_base;
uint32_t kernel_physical_end = (uint32_t)&_kernel_unpaged_end;
uint32_t kernel_virt_start = (uint32_t)&_kernel_virt_base;
uint32_t kernel_len = (uint32_t)&_kernel_size;

static uint32_t pagedir[1024] __attribute__((aligned(0x1000)));

void pg_clear(void);
void pg_identity(void);
int pg_mapkernel(void);
void pg_enable(void);
void pg_map(uint32_t, uint32_t, uint32_t);
uint32_t *alloc_pagetable(uint32_t *p);
uintptr_t alloc_page(struct kinfo *);
void add_memmap(struct kinfo *, uint64_t, uint64_t);
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
pg_clear(void)
{
	memset(pagedir, 0, sizeof(pagedir));
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
	uint32_t k_pde_end = kernel_physical_end >> 22;

	for (i = 0; i <= k_pde_end; ++i)
		/* present, r/w, 4MiB pages */
		pagedir[i] = (i << 22) | 0b10000011;
}

/*
 * map the kernel to virtual memory
 */
int
pg_mapkernel(void)
{
	int pde = kernel_virt_start >> 22; /* aligned to 4MiB pde entry */
	uint32_t m;
	uint32_t kernel_start = kernel_physical_start;

	for (m = 0; m < kernel_len; m += 1024 * 4096) {
		/* present, r/w, 4MiB pages */
		pagedir[pde++] = kernel_start | 0b10000011;
		kernel_start += 1024 * 4096;
	}

	return pde;
}

void
pg_enable(void)
{
	vm_enable_paging(pagedir);
}


void
pg_map(uint32_t p_addr, uint32_t v_start, uint32_t v_end)
{
	static int mapped_pde = -1;
	static uint32_t *pt = NULL;
	uint32_t frame, ph;
	int pte, pde;

	v_start = pg_rounddown(v_start);
	p_addr = pg_rounddown(p_addr);

	for (; v_start < v_end; v_start += 0x1000) {
		frame = p_addr;

		pde = v_start >> 22;
		pte = v_start >> 12;

		if (mapped_pde < pde) {
			pt = alloc_pagetable(&ph);
			pagedir[pde] = (ph & 0xFFFFF000) | 0b00000111;
			mapped_pde = pde;
		}
		pt[pte] = (frame & 0xFFFFF000) | 0b00000111;
		p_addr += 0x1000;
	}
}


uint32_t *
alloc_pagetable(uint32_t *p)
{
	uint32_t *t;
	static uint32_t pagetable[5][1024] __attribute__((aligned(0x1000)));
	static int used_pt_n = 0;

	if (used_pt_n >= 5)
		panic();

	t = pagetable[used_pt_n++];
	*p = virt2phys(t);

	return t;
}

uintptr_t
alloc_page(struct kinfo *k)
{
	int m;
	multiboot_memory_map_t *mmap;

	for (m = k->mmap_n - 1; m >= 0; --m) {
		mmap = &k->memmap[m];
		if (mmap->len > 0) {
			mmap->len -= 0x1000;
			return mmap->addr + mmap->len;
		}
	}
	panic();
}
