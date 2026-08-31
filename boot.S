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
stack_bottom:
	.skip 0x4000
stack_top:

.section .text
.globl _start
.type _start, @function
_start:
	mov $stack_top, %esp

	pushl %eax
	pushl %ebx

	call kernel_main

	cli
1:
	hlt
	jmp 1b

.size _start, . - _start
