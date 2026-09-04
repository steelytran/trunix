#define ASM_FILE
#include "multiboot.h"

#define FLAGS MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO

.section .multiboot.data, "aw"
	.align 4
	.long MULTIBOOT_HEADER_MAGIC
	.long FLAGS
	.long -(MULTIBOOT_HEADER_MAGIC + FLAGS) /* checksum */

.section .boot_stack, "aw", @nobits
stack_bottom:
	.skip 0x4000
stack_top:

.section .bss, "aw", @nobits
	.align 0x1000
boot_page_directory:
	.align 0x1000
boot_page_table:
	.skip 0x1000

.section .multiboot.text, "a"
.globl _start
.type _start, @function
_start:
	movl $0x00000000, %esi
	movl $(boot_page_table - 0xC0000000), %edi
	movl $1023, %ecx

/*
 * map from start of physical memory to end of kernel
 */
1:
	cmpl $(_kernel_end - 0xC0000000), %esi
	jge 2f

	movl %esi, %edx
	orl $0x03, %edx /* present, writable */
	movl %edx, (%edi)

	addl $0x1000, %esi
	addl $4, %edi
	loop 1b

2:
	/* identity mapping */
	movl $(boot_page_table - 0xC0000000 + 0x03), boot_page_directory - 0xC0000000
	movl $(boot_page_table - 0xC0000000 + 0x03), boot_page_directory - 0xC0000000 + 768 * 4

	movl $(boot_page_directory - 0xC0000000), %ecx
	movl %ecx, %cr3

	movl %cr0, %ecx
	orl $0x80010000, %ecx
	movl %ecx, %cr0

	lea 3f, %ecx
	jmp *%ecx

.section .text
3:
	movl $0x00000002, boot_page_directory

	movl %cr3, %ecx
	movl %ecx, %cr3

	movl $stack_top, %esp

	pushl %eax
	pushl %ebx

	call kernel_main

	cli
1:
	hlt
	jmp 1b
