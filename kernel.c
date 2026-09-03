#include <stdarg.h>
#include <stdint.h>

#include <trunix/debug.h>
#include <trunix/tty.h>
#include <trunix/trunix.h>
#include <trunix/io.h>
#include <multiboot.h>
#include <string.h>

extern void init_descriptor_tables(void);
extern void load_page_directory(uint32_t *);
extern void enable_paging(void);

extern uint32_t _kernel_start;
extern uint32_t _kernel_end;
extern uint32_t _mem_start;

uintptr_t kernel_start = (uintptr_t)&_kernel_start;
uintptr_t kernel_end = (uintptr_t)&_kernel_end;
uintptr_t mem_start = (uintptr_t)&_mem_start;

uint32_t *bitmap = &_kernel_end;
uint32_t *page_directory = NULL;
uint32_t *page_table = NULL;

int page_n;
size_t bitmap_len;

struct memory_map_entry {
	uintptr_t	addr;
	size_t		len;
};

struct memory_map_entry memory_map[32];

void
panic(const char * s)
{
	printk(s);
	__asm__ volatile("hlt");
}

void *
alloc_pages(unsigned int n)
{
	int i, pages;
	for (i = pages = 0; i < page_n; ++i)  {
		if (!(bitmap[i >> 5] & (1 << (i & 31)))) {
			if (++pages == n) {
				for (; pages > 0; --i, --pages)
					bitmap[i >> 5] |= (1 << (i & 31));

				return (void *)(mem_start + (++i << 12));
			}
		} else pages = 0;
	}
	return NULL;
}

void
free_pages(void *p, unsigned int n)
{
	int i;
	unsigned int p_bit = ((uintptr_t)p - mem_start) >> 12;

	for (i = p_bit; i < p_bit + n; ++i)
		bitmap[i >> 5] &= ~(1 << (i & 31));
}

void
kernel_main(multiboot_info_t *mbd, uint32_t magic)
{
	int i, j;
	multiboot_memory_map_t *mmmt;
	cls();

	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
		panic("Invalid magic number\n");
	if (!(mbd->flags >> 6 & 1))
		panic("Invalid memory map passed by bootloader\n");

	for (i = j = 0; i < mbd->mmap_length;
	     i += mmmt->size + sizeof(mmmt->size)) {

		mmmt = (multiboot_memory_map_t *)(mbd->mmap_addr + i);
		if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {

			if (kernel_start >= mmmt->addr &&
			    kernel_start < mmmt->addr + mmmt->len) {

				memory_map[j].addr = (uintptr_t)&_mem_start;
				memory_map[j].len =
				    (size_t)(mmmt->addr + mmmt->len) -
				    (size_t)&_mem_start;
			} else {
				memory_map[j].addr = mmmt->addr;
				memory_map[j].len = (size_t)mmmt->len;

			}
			++j;

		}

	}

	init_descriptor_tables();

	page_n = (memory_map[1].len >> 12);
	bitmap_len = page_n >> 5;
	memsetl(bitmap, 0, bitmap_len);

	if ((page_directory = alloc_pages(1)) == NULL) {
		panic("KERNEL PANIC: could not allocate page directory.\n");
		return;
	} if ((page_table = alloc_pages(1)) == NULL) {
		panic("KERNEL PANIC: could not allocate page table.\n");
		return;
	}

	memsetl(page_directory, 0x00000002, 1024);

	for (i = 0; i < 1024; ++i)
		page_table[i] = (i << 12) | 3;

	page_directory[0] = ((uintptr_t)page_table) | 3;
	page_directory[1023] = ((uintptr_t)page_directory) | 3;

	load_page_directory(page_directory);
	enable_paging();

	return;
}
