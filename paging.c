#include <stdint.h>
#include <string.h>

#include <sys/trunix.h>

extern uint32_t _kernel_physical_base;
extern uint32_t _kernel_virt_base;
extern uint32_t _kernel_size;

uint32_t kernel_physical_start = (uint32_t)&_kernel_physical_base;
uint32_t kernel_virt_start = (uint32_t)&_kernel_virt_base;
uint32_t kernel_len = (uint32_t)&_kernel_size;

uint32_t pagedir[1024] __attribute__((aligned(0x1000)));

void
pg_clear(void)
{
	memset(pagedir, 0, sizeof(pagedir));
	return;
}

void
pg_identity(void)
{
	uint32_t i;

	for (i = 0; i < 1024; ++i)
		/* present, r/w, user, 4MiB pages */
		pagedir[i] = (i << 22) | 0b10000111;
}

int
pg_mapkernel(void)
{
	int pde = kernel_virt_start >> 22; /* aligned to 4MiB pde entry */
	uint32_t m;
	uint32_t kernel_start = kernel_physical_start;

	for (m = 0; m < kernel_len; m += 1024 * 4096) {
		/* present, r/w, 4MiB pages */
		pagedir[pde++] = kernel_start | 0b10000011;
		kernel_start += 1024 * 4096;
	}

	return pde;
}

void
pg_enable(void)
{
	vm_enable_paging(pagedir);
}
