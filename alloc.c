/*
 * memory allocation
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

#include <sys/mman.h>
#include <sys/multiboot.h>
#include <sys/trunix.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* bitmap manipulation macros */
#define bit_set(x, n)	((x) |= (1 << (n)))
#define bit_unset(x, n)	((x) &= ~(1 << (n)))
#define bit_isset(x, n)	((x) & (1 << (n)))

static uint32_t page_bitmap[0x100000 / 32];
static unsigned int last_page;

void *mmap(void *, size_t, int, int, int, off_t);
void munmap(void *, size_t);
void init_mem(struct kinfo *);

static inline uintptr_t alloc_pages(int, int, int);
static inline void free_pages(int, int);

static inline void
pg_bitmap_alloc(uintptr_t addr)
{
	unsigned int i = addr >> 5;
	unsigned int order = addr % 32;

	bit_set(page_bitmap[i], order);
}

static inline void
pg_bitmap_free(uintptr_t addr)
{
	unsigned int i = addr >> 5;
	unsigned int order = addr % 32;

	bit_unset(page_bitmap[i], order);
}

static inline int
pg_bitmap_isused(unsigned int pg)
{
	unsigned int i = pg >> 5;
	unsigned int order = pg % 32;

	return bit_isset(page_bitmap[i], order);
}

static inline void
free_pages(int base, int page_n)
{
	int i;
	for (i = base; i < base + page_n; ++i)
		pg_bitmap_free(i);
}

static inline uintptr_t
alloc_pages(int base, int page_n, int prot)
{
	int i;
	uintptr_t p_base, v_start, v_end;

	for (i = base; i < base + page_n; ++i)
		pg_bitmap_alloc(i);

	p_base = base << 12;
	v_start = phys2virt(p_base);
	v_end = phys2virt(i << 12);
	pg_map(p_base, v_start, v_end, prot);

	return v_start;
}

void *
mmap(void *addr, size_t len, int prot, int flags, int fd, off_t off)
{
	uint32_t i;
	unsigned int page_n, free = 0;

	if (len == 0)
		goto err;

	if (prot > (PROT_READ | PROT_WRITE) || prot < 0)
		goto err;

	page_n = pg_roundup(len) >> 12;

	if (addr != NULL)
		return (void *)
		alloc_pages(((int)addr + off) >> 12, page_n, prot);

	for (i = 0; i <= last_page; ++i) {
		if (pg_bitmap_isused(i)) {
			free = 0;
			continue;
		} if (++free == page_n)
			return (void *)
			alloc_pages(i + 1 - page_n, page_n, prot);
	}

err:
	return NULL;
}

void
munmap(void *addr, size_t len)
{
	int page_n = len >> 12;
	unsigned int pg = virt2phys(addr) >> 12;

	pg_free((uintptr_t)addr, (uint32_t)addr + len);

	if (page_n >= 0)
		free_pages(pg, page_n);

	return;
}

void
init_mem(struct kinfo *k)
{
	int m;
	uintptr_t i;

	memsetl(page_bitmap, ~0, 0x100000 >> 5);

	for (m = 0; m < k->mmap_n; ++m) {
		if (k->memmap[m].type != MULTIBOOT_MEMORY_AVAILABLE)
			continue;

		for (i = k->memmap[m].addr;
		    i <= k->memmap[m].addr + k->memmap[m].len;
		    i += 0x1000)
			pg_bitmap_free(i >> 12);
	}

	last_page = i >> 12;

	return;
}
