#define ASM_FILE
#include <multiboot.h>

#define FLAGS MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO

.section .multiboot
	.align 4
	.long MULTIBOOT_HEADER_MAGIC
	.long FLAGS
	.long -(MULTIBOOT_HEADER_MAGIC + FLAGS) /* checksum */

.section .bss
.align 16
	.skip 0x4000
stack:

.section .text
.globl TRUNIX
TRUNIX:
	mov $stack, %esp

	pushl %eax
	pushl %ebx

	call init_trunix

	cli
1:
	hlt
	jmp 1b
