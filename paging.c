#include <stdint.h>
#include <string.h>

#include <sys/trunix.h>

uint32_t pagedir[1024] __attribute__((aligned(0x1000)));

void
pg_clear(void)
{
	memset(pagedir, 0, sizeof(pagedir));
	return;
}
