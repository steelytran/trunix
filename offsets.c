#include <stdio.h>
#include <stddef.h>

#include "multiboot.h"

int
main(void)
{
	printf("#define MMMT_SIZE_OFFSET %zu\n",
	    offsetof(struct multiboot_mmap_entry, size));

	printf("#define MMMT_ADDR_OFFSET %zu\n",
	    offsetof(struct multiboot_mmap_entry, addr));

	printf("#define MMMT_LEN_OFFSET %zu\n",
	    offsetof(struct multiboot_mmap_entry, len));

	printf("#define MMMT_TYPE_OFFSET %zu\n",
	    offsetof(struct multiboot_mmap_entry, type));


	printf("#define MBD_FLAGS_OFFSET %zu\n",
	    offsetof(struct multiboot_info, flags));

	printf("#define MBD_MMAP_LEN_OFFSET %zu\n",
	    offsetof(struct multiboot_info, mmap_length));

	printf("#define MBD_MMAP_ADDR_OFFSET %zu\n",
	    offsetof(struct multiboot_info, mmap_addr));

	return 0;
}
