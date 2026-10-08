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

#include <sys/cdefs.h>
#include <string.h>

#include "trunix.h"
#include "vm.h"

static u32 pd[1024] __aligned(0x1000);
static u32 *pagedir = pd;

extern u32 vm_enable_paging(u32 *);

void pg_clear();
void pg_identity();
void pg_clear_identity();
void pg_enable();

void add_memmap(struct kinfo *, u64, u64);
void cut_memmap(struct kinfo *, u32, u32);

/*
 * add memory map entry 
 */
void 
add_memmap(struct kinfo *k, u64 addr, u64 len)
{
	int m;
	u32 highmark;
	if (len == 0)
		return;

	addr = pg_roundup(addr);
	len = pg_rounddown(len);

	for (m = 0; m < MAXMEMMAP; ++m) {
		if (k->memmap[m].len)
			continue;

		memcpy(&k->memmap[m].addr, &addr, sizeof(u64));
		memcpy(&k->memmap[m].len, &len, sizeof(u64));
		k->memmap[m].type = MULTIBOOT_MEMORY_AVAILABLE;

		if (m >= k->mmap_n)
			k->mmap_n = m + 1;

		highmark = addr + len;
		if (highmark > k->mem_high_phys)
			k->mem_high_phys = highmark;

		return;
	}

	panic("no available memmap slot");
}

void
cut_memmap(struct kinfo *k, u32 start, u32 end)
{
	int m;
	u32 cut_start, cut_end;
	u32 mme_start, mme_end;

	start = pg_rounddown(start);
	end = pg_roundup(end);

	for (m = 0; m < k->mmap_n; ++m) {
		cut_start = start;
		cut_end = end;

		mme_start = k->memmap[m].addr;
		mme_end = (k->memmap[m].addr + k->memmap[m].len);

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
pg_clear()
{
	memsetl(pagedir, 0, 1024);
}

/*
 * identity map from the start of
 * memory to the end of the kernel.
 */
void
pg_identity()
{
	u32 i;

	for (i = 0; i < 1024; ++i) {
		pagedir[i] = (i << 22) | PG_PSE | PG_RW | PG_P;

		if (i >= (u32)_kernel_offset >> 22)
			pagedir[i] =
			    (i << 22) + (u32)_kernel_offset
			    | PG_G | PG_PSE | PG_RW | PG_P;
	}
}

void
pg_clear_identity()
{
	u32 i;

	pagedir = p2v(read_cr3());

	for (i = 0; i < 1024; ++i) {
		if (i > ((u32)_kernel_physical_base >> 22) ||
		    i < ((u32)_kernel_offset + (u32)_kernel_unpaged_end) >> 22)
			continue;

		pagedir[i++] = 0;
	}
}

void
pg_enable()
{
	vm_enable_paging(pagedir);
}
