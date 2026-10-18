/*
 * dynamic memory allocation
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

#include <trunix/vm.h>
#include <sys/mman.h>
#include <sys/queue.h>
#include <stdlib.h>

struct sblock {
	size_t sz;
	SLIST_ENTRY(sblock) entry;
};

SLIST_HEAD(slob, sblock);
static struct slob heap = SLIST_HEAD_INITIALIZER(heap);

void *
malloc(size_t sz)
{
	struct sblock *p, *s;
	size_t *hdr;

	sz += sizeof(*hdr);
	sz = roundup(sz, sizeof(*hdr));

	/* first allocation */
	if (SLIST_EMPTY(&heap)) {
		SLIST_INIT(&heap);
		s = mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_ANON, -1, 0);
		p = (unsigned char *)s + sz;
		p->sz = sz - pg_roundup(sz);
		SLIST_INSERT_HEAD(&heap, p, entry);

		hdr = s;
		*hdr = sz;
		return hdr + 1;
	}

	/* search for free blocks */
	SLIST_FOREACH(p, &heap, entry)
		if (p->sz >= sz)
			goto found;

#if 0
	/* allocate new page */
	SLIST_INIT(&heap);
	s = mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_ANON, -1, 0);
	p = (unsigned char *)s + sz;
	p->sz = sz - pg_roundup(sz);
	SLIST_INSERT_HEAD(&heap, p, entry);

	hdr = s;
	return hdr + 1;
#else
	return NULL;
#endif

found:
	if (p->sz > sz) {
		s = (unsigned char *)p + sz;
		s->sz = p->sz - sz;
		SLIST_INSERT_AFTER(p, s, entry);
	}

	SLIST_REMOVE(&heap, p, sblock, entry);

	hdr = p;
	*hdr = sz;

	return hdr + 1;
}

void
free(void *a)
{
	struct sblock *p, *n, *tmp;
	p = (unsigned short *)a - 1;
	p->sz = *(unsigned short *)p;

	n = SLIST_FIRST(&heap);
	if (p < n) {
		if ((unsigned char *)p + p->sz == n) {
			SLIST_REMOVE(&heap, n, sblock, entry);
			p->sz += tmp->sz;
		}
		SLIST_INSERT_HEAD(&heap, p, entry);
		return;
	}

	SLIST_FOREACH_SAFE(n, &heap, entry, tmp)
		;
	/* mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_ANON, -1, 0); */
}
