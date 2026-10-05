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

#include <sys/queue.h>
#include <sys/multiboot.h>
#include <sys/cdefs.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "vm.h"
#include "trunix.h"

u32 *kpagedir;

LIST_HEAD(memlist, block);
static struct memlist freelist = LIST_HEAD_INITIALIZER(freelist);

struct block {
	unsigned int sz;
	LIST_ENTRY(block) entry;
};

void init_mem(struct kinfo *);
u32 *copykvm();
void alloc_pt(u32 *, u32, u32, u32, int);
void *alloc_mem(u32);
void free_mem(void *, u32);


void
free_mem(void *addr, u32 sz)
{
	struct block *p, *n, *tmp;
	unsigned int page_n = sz >> 12;

	if (addr == NULL)
		panic();

	addr = (void *)pg_rounddown(addr);
	sz = pg_roundup(sz);

	memset(addr, 0x69, sz);
	n = (struct block *)addr;
	n->sz = page_n;

	LIST_FOREACH_SAFE(p, &freelist, entry, tmp) {
		if (p + (p->sz << 12) == n) {
			n = p;
			n->sz += page_n;
			continue;
		}

		if (n + (n->sz << 12) == p) {
			LIST_REMOVE(p, entry);
			n->sz += p->sz;
		}
	}

	LIST_FOREACH_SAFE(p, &freelist, entry, tmp) {
		if (p->sz >= n->sz)
			LIST_INSERT_BEFORE(p, n, entry);
		else if(tmp == NULL)
			LIST_INSERT_AFTER(p, n, entry);
		else
			continue;

		break;
	}
}

void *
alloc_mem(u32 sz)
{
	struct block *p, *s;
	void *addr;
	unsigned int page_n = sz >> 12;

	LIST_FOREACH(p, &freelist, entry)
		if (p->sz >= page_n)
			break;

	if (p == NULL)
		return NULL;

	addr = p;
	sz = pg_roundup(sz);

	if (p->sz > (3 * page_n) / 2) {
		s = (struct block *)((char *)p + sz);
		s->sz = p->sz - page_n;
		LIST_INSERT_AFTER(p, s, entry);
	}

	LIST_REMOVE(p, entry);

	return addr;
}

void
init_mem(struct kinfo *k)
{
	int m = 0;
	struct block *p, *n, *tmp;

	LIST_INIT(&freelist);

	for (; m < k->mmap_n; ++m) {
		if (k->memmap[m].type != MULTIBOOT_MEMORY_AVAILABLE)
			continue;

		p = (struct block *)phys2virt(k->memmap[m].addr);
		p->sz = (k->memmap[m].len >> 12);

		if (LIST_EMPTY(&freelist))
			LIST_INSERT_HEAD(&freelist, p, entry);
		else {
			LIST_FOREACH_SAFE(n, &freelist, entry, tmp)
				if (n->sz > p->sz || tmp == NULL)
					break;

			LIST_INSERT_AFTER(n, p, entry);
		}
	}

	kpagedir = phys2virt(read_cr3());
	return;
}

u32 *
copykvm()
{
	u32 *pd = alloc_mem(0x1000);
	memcpy(pd, kpagedir, 0x1000);
	return pd;
}

void
alloc_pt(u32 *pd, u32 v_start, u32 len, u32 frame, int flags)
{
	u32 *pt;
	int pte, pde;
	u32 v_end = v_start + len;

	v_start = pg_rounddown(v_start);
	frame = pg_rounddown(frame);

	for (; v_start < v_end; v_start += 0x1000) {
		pde = v_start >> 22;
		pte = (v_start >> 12) & 0x3FF;

		if (!(pd[pde] & 1)) {
			pt = alloc_mem(0x1000);
			pd[pde] = (virt2phys(pt) & 0xFFFFF000) | flags;
		} else {
			pt = phys2virt(pd[pde] & 0xFFFFF000);
			pd[pde] |= flags;
		}

		pt[pte] = (frame & 0xFFFFF000) | flags;
		frame += 0x1000;
	}
}
