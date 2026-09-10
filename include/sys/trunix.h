#ifndef TRUNIX_H
#define TRUNIX_H

#include <multiboot.h>

struct kinfo {
	multiboot_info_t	mbi;
	multiboot_memory_map_t	memmap[32];
};

void panic(const char *);

void pg_clear(void);

#endif
