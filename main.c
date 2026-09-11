#include <sys/trunix.h>
#include <sys/tty.h>
#include <stdio.h>

/*
 * kernel main
 */
void
kmain(struct kinfo *kernel_info)
{
	init_gdt();
	init_tss();
	init_idt();

	cls();
	printf("Welcome to Trunix!\n");

	return;
}
