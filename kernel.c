#include <stdarg.h>
#include <stdint.h>

#include <trunix/debug.h>
#include <trunix/tty.h>
#include <multiboot.h>
#include <string.h>

extern void init_descriptor_tables(void);

void
kernel_main(multiboot_info_t *mbd, uint32_t magic)
{
	int i;
	multiboot_memory_map_t *mmmt;
	size_t available_mem = 0;

	cls();

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		printk("Invalid magic number\n");
		return;
	} if (!(mbd->flags >> 6 & 1)) {
		printk("Invalid memory map passed by bootloader\n");
		return;
	}

	for (i = 0; i < mbd->mmap_length; i += sizeof(multiboot_memory_map_t)) {
		mmmt = (multiboot_memory_map_t *)( mbd->mmap_addr + i);

		printk("Start Addr: 0x%x | Length: 0x%x | Size: 0x%x | Type: %d\n",
			(uint32_t)mmmt->addr,
			(uint32_t)mmmt->len,
			mmmt->size,
			mmmt->type
		);

		if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE)
			available_mem += (size_t)mmmt->len;
	}

	printk("\n%d KiB of available memory.\n", available_mem >> 10);
	init_descriptor_tables();
	return;
}
