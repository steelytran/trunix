/*
 * virtual memory management
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
#include <string.h>
#include <stddef.h>
#include <assert.h>

#include "vm.h"
#include "trunix.h"

u32 *kpagedir;

struct block {
	u32 sz;
	LIST_ENTRY(block) entry;
};
LIST_HEAD(memlist, block);
static struct memlist freelist = LIST_HEAD_INITIALIZER(freelist);

struct sblock {
	u32 sz;
	SLIST_ENTRY(sblock) entry;
};
SLIST_HEAD(slob, sblock);
static struct slob heap = SLIST_HEAD_INITIALIZER(heap);

void init_mem(struct kinfo *);
u32 *copykvm();
void alloc_pt(u32 *, u32, u32, u32, int);
void *alloc_pages(void *, u32);
void free_mem(void *, u32);
void *kmalloc(u32);
void kfree(void *);

void *
alloc_pages(void *addr, u32 sz)
{
	struct block *p, *s;

	if (addr == NULL) {
		LIST_FOREACH(p, &freelist, entry)
			if (p->sz >= sz)
				goto found;

		return NULL;

found:
		addr = p;
	} else
		panic("until mapping specific addresses is implemented");

	sz = pg_roundup(sz);

	LIST_REMOVE(p, entry);
	/* reinsert split block */
	if (p->sz > sz)
		free_pages((u8 *)p + sz, p->sz - sz);

	return addr;
}

void
free_pages(void *addr, u32 sz)
{
	struct block *p, *n, *tmp;

	assert(addr != NULL);

	addr = (void *)pg_rounddown(addr);
	sz = pg_roundup(sz);

	memset(addr, 0x69, sz);
	n = (struct block *)addr;
	n->sz = sz;

	LIST_FOREACH_SAFE(p, &freelist, entry, tmp) {
		if (p->sz >= n->sz)
			LIST_INSERT_BEFORE(p, n, entry);
		else if (tmp == NULL)
			LIST_INSERT_AFTER(p, n, entry);
		else
			continue;

		break;
	}
}

void *
kmalloc(u32 sz)
{
	struct sblock *p, *s;
	u16 *hdr;

	sz += sizeof(*hdr);
	sz = roundup(sz, 8);

	assert(sz < 0x1000);

	SLIST_FOREACH(p, &heap, entry)
		if (p->sz >= sz)
			goto found;

	return NULL;

found:
	if (p->sz > sz) {
		s = (u8 *)p + sz;
		s->sz = p->sz - sz;
		SLIST_INSERT_AFTER(p, s, entry);
	}

	SLIST_REMOVE(&heap, p, sblock, entry);

	hdr = p;
	*hdr = sz;

	return hdr + 1;
}

void
kfree(void *a)
{
	struct sblock *p, *n, *tmp;
	p = (u16 *)a - 1;
	p->sz = *(u16 *)p;

	n = SLIST_FIRST(&heap);
	if (p < n) {
		if ((u8 *)p + p->sz == n) {
			SLIST_REMOVE(&heap, n, sblock, entry);
			p->sz += tmp->sz;
		}
		SLIST_INSERT_HEAD(&heap, p, entry);
		return;
	}

	SLIST_FOREACH_SAFE(n, &heap, entry, tmp)
		if (n < p && p < tmp)
			break;

	if ((u8 *)n + n->sz == p) {
		n->sz += p->sz;
		p = n;
	} else
		SLIST_INSERT_AFTER(n, p, entry);

	if ((u8 *)p + p->sz == tmp) {
		SLIST_REMOVE(&heap, tmp, sblock, entry);
		p->sz += tmp->sz;
	}
}

void
init_mem(struct kinfo *k)
{
	int m = 0;
	struct block *p, *n, *tmp;
	struct sblock *sb;

	LIST_INIT(&freelist);

	for (; m < k->mmap_n; ++m) {
		if (k->memmap[m].type != MULTIBOOT_MEMORY_AVAILABLE)
			continue;

		p = (struct block *)p2v(k->memmap[m].addr);
		p->sz = k->memmap[m].len;

		if (LIST_EMPTY(&freelist))
			LIST_INSERT_HEAD(&freelist, p, entry);
		else {
			LIST_FOREACH_SAFE(n, &freelist, entry, tmp)
				if (n->sz > p->sz || tmp == NULL)
					break;

			LIST_INSERT_AFTER(n, p, entry);
		}
	}

	/* fixed kernel heap size of 64 KiB for now */
	sb = alloc_pages(NULL, 0x10000);
	sb->sz = 0x10000;

	SLIST_INIT(&heap);
	SLIST_INSERT_HEAD(&heap, sb, entry);

	kpagedir = p2v(read_cr3());
	return;
}

u32 *
copykvm()
{
	u32 *pd = alloc_pages(NULL, 0x1000);

	memsetl(pd, 0, 1024);

	memcpyl(pd + (KERNEL_OFFSET >> 22),
	    kpagedir + (KERNEL_OFFSET >> 22),
	    1024 - (KERNEL_OFFSET >> 22));

	return pd;
}

void
alloc_pt(u32 *pd, u32 va, u32 pa, u32 len, int flags)
{
	u32 *pt;
	int pte, pde;

	va = pg_rounddown(va);
	pa = pg_rounddown(pa);
	len = pg_roundup(len);

	for (; len > 0; len -= 0x1000) {
		pde = va >> 22;
		pte = (va >> 12) & 0x3FF;

		if (!(pd[pde] & 1)) {
			pt = alloc_pages(NULL, 0x1000);
			memsetl(pt, 0, 1024);
			pd[pde] = (v2p(pt) & 0xFFFFF000) | flags;
		} else {
			pt = p2v(pd[pde] & 0xFFFFF000);
			pd[pde] |= flags;
		}

		pt[pte] = (pa & 0xFFFFF000) | flags;

		pa += 0x1000;
		va += 0x1000;
	}
}
