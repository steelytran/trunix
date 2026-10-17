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

#include <trunix/drivers.h>
#include <trunix/fs.h>
#include <trunix/vm.h>
#include <trunix/trunix.h>
#include <trunix/proc.h>
#include <sys/queue.h>
#include <sys/multiboot.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

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
void *alloc_pages(u32);
void free_mem(void *, u32);
void *kmalloc(u32);
void kfree(void *);

void *sys_mmap(void *, size_t, int, int, int, off_t);
void sys_munmap(void *, size_t);

void *
alloc_pages(u32 sz)
{
	void *addr;
	struct block *p, *s;

	LIST_FOREACH(p, &freelist, entry)
		if (p->sz >= sz)
			goto found;

	return NULL;

found:
	addr = p;
	sz = pg_roundup(sz);

	LIST_REMOVE(p, entry);
	/* split block */
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
	sb = alloc_pages(0x10000);
	sb->sz = 0x10000;

	SLIST_INIT(&heap);
	SLIST_INSERT_HEAD(&heap, sb, entry);

	kpagedir = p2v(read_cr3());
	return;
}

u32 *
copykvm()
{
	u32 *pd = alloc_pages(0x1000);

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
			pt = alloc_pages(0x1000);
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

void *
sys_mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
	struct vm_region *mem;
	struct file *f;
	struct proc *p;
	u32 *pd;
	u32 m;
	u32 *page;

	if (len == 0)
		return NULL;

	p = getproc();

	len = pg_roundup(len);
	if (p->mmap_base + len >= MMAP_LIMIT)
		return NULL;

	mem = kmalloc(sizeof(struct vm_region));

	if (flags & MAP_ANON) {
		mem->pa = v2p(page = alloc_pages(len));
		memset(page, 0, pg_roundup(len));
	} else {
		f = p->ofile[fd];
		if (f == NULL)
			return -1;

		(*cdevs[f->inode->major]->mmap)(f, mem);
	}

	/*
	 * if addr != NULL
	 *	mem.va = nearest page
	 *	boundary to addr
	 * else
	 * 	find free region at
	 * 	least len bytes long
	 */

	mem->va = p->mmap_base;
	p->mmap_base += len;
	mem->len = len;

	pd = p2v(p->cr3);
	alloc_pt(pd,
	    mem->va,
	    mem->pa,
	    mem->len,
	    PG_US | PG_RW | PG_P);
	/*  PG_US | flags); */
	/*  
	 * FIX: value passed in
	 * flags is clobbered.
	 */

	SLIST_INSERT_HEAD(GETMEMMAP(p), mem, entry);

	return mem->va;
}

void
sys_munmap(void *addr, size_t len)
{
	return;
}
