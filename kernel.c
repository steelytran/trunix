#include <stdarg.h>
#include <stdint.h>

#include <trunix/debug.h>
#include <trunix/tty.h>
#include <multiboot.h>
#include <string.h>

extern void init_descriptor_tables(void);

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;

struct memory_map_entry {
	uintptr_t	addr;
	size_t		len;
};

struct memory_map_entry memory_map[32];

void
kernel_main(multiboot_info_t *mbd, uint32_t magic)
{
	int i, j;
	multiboot_memory_map_t *mmmt;

	cls();

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		printk("Invalid magic number\n");
		return;
	} if (!(mbd->flags >> 6 & 1)) {
		printk("Invalid memory map passed by bootloader\n");
		return;
	}

	for (i = j = 0; i < mbd->mmap_length;
	     i += mmmt->size + sizeof(mmmt->size)) {

		mmmt = (multiboot_memory_map_t *)( mbd->mmap_addr + i);
		if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {

			if (&_kernel_start >= mmmt->addr &&
			    &_kernel_start < mmmt->addr + mmmt->len) {
				memory_map[j].addr = (uintptr_t)&_kernel_end;
				memory_map[j].len =
				    (size_t)(mmmt->addr + mmmt->len) -
				    (size_t)&_kernel_end;
			} else {
				memory_map[j].addr = (uintptr_t)mmmt->addr;
				memory_map[j].len = (size_t)mmmt->len;
			}

			printk("Address: 0x%x, Length: 0x%x\n",
				memory_map[j].addr,
				memory_map[j].len
			);
			++j;
		}
	}

	init_descriptor_tables();
	return;
}
