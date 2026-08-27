/* multiboot flags */
.set ALIGN, 1 << 0
.set MEMINFO, 1 << 1
.set FLAGS, ALIGN | MEMINFO
.set MAGIC, 0x1BADB002 /* multiboot 1 signature */

.section .multiboot
	.align 4
	.long MAGIC
	.long FLAGS
	.long -(MAGIC + FLAGS) /* checksum */

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
	call kernel_main

	cli
1:
	hlt
	jmp 1b

.size _start, . - _start
