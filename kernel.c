#include <stdarg.h>

#include "debug.h"

void
kernel_main(void)
{
	printk("kernel_main: 0x%X\n", (unsigned int)&kernel_main);
	printk("printk: 0x%X", (unsigned int)&printk);
	return;
}
