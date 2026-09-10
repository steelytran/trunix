#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include <multiboot.h>
#include <sys/trunix.h>
#include <sys/tty.h>

struct kinfo ki;

struct kinfo *
init_trunix(multiboot_info_t *mb_info, uint32_t magic)
{
	int i = 0;
	multiboot_uint32_t m;
	multiboot_memory_map_t *mb_mmap;

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
		return NULL;

	if (!(mb_info->flags >> 6 & 1))
		return NULL;

	ki.mbi = *mb_info;

	for (m = 0; m < mb_info->mmap_length;
	     m += mb_mmap->size + sizeof(mb_mmap->size)) {

		mb_mmap = (multiboot_memory_map_t *)(mb_info->mmap_addr + m);

		if (mb_mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
			ki.memmap[i++] = *mb_mmap;

	}

	cls();
	for (i = 0; ki.memmap[i].type == 1; ++i)
		printf("addr: 0x%x, len: 0x%x\n",
		    (uint32_t)ki.memmap[i].addr,
		    (uint32_t)ki.memmap[i].len);

	/* setup paging */
	pg_clear();

	return &ki;
}
