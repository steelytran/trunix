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

#include "trunix.h"
#include "vm.h"

static uint32_t pd[1024] __attribute__((aligned(0x1000)));
static uint32_t *pagedir = pd;

void pg_clear(void);
void pg_identity(void);
void pg_clear_identity(void);
void pg_enable(void);
void pg_map(uint32_t, uint32_t, uint32_t, int);
void pg_free(uint32_t, uint32_t);

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
	memsetl(pagedir, 0, 1024);
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

	for (i = 0; i < 1024; ++i) {
		pagedir[i] = (i << 22) | PG_PSE | PG_RW | PG_P;

		if (i >= (uint32_t)_kernel_offset >> 22)
			pagedir[i] =
			    (i << 22) + (uint32_t)_kernel_offset
			    | PG_G | PG_PSE | PG_RW | PG_P;
	}
}

void
pg_clear_identity(void)
{
	uint32_t i = 0;

	pagedir = (uint32_t *)phys2virt(read_cr3());
	while (i < ((uint32_t)_kernel_offset >> 22))
		pagedir[i++] = 0;
}

void
pg_enable(void)
{
	vm_enable_paging(pagedir);
}
