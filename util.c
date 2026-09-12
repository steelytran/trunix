#include <sys/trunix.h>

void
panic(void)
{
	__asm__ volatile(
		"cli\n\t"
		"hlt"
		:::
	);
}

