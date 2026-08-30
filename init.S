.section .rodata

.globl global_descriptor_table
global_descriptor_table:
/* null descriptor */
		.fill 2, 4, 0

/* kernel space code segment */
	.word	0xFFFF	/* limit 0:15 */
	.word	0x0000	/* base 0:15 */
	.byte	0x00	/* base 16:23 */
	.byte	0x9A	/* access */
	.byte	0xCF	/* flags; limit 16:19 */
	.byte	0x00	/* base 24:31 */

/* kernel space data segment */
	.word	0xFFFF	/* limit 0:15 */
	.word	0x0000	/* base 0:15 */
	.byte	0x00	/* base 16:23 */
	.byte	0x92	/* access */
	.byte	0xCF	/* flags; limit 16:19 */
	.byte	0x00	/* base 24:31 */

/* user space code segment */
	.word	0xFFFF	/* limit 0:15 */
	.word	0x0000	/* base 0:15 */
	.byte	0x00	/* base 16:23 */
	.byte	0xFA	/* access */
	.byte	0xCF	/* flags; limit 16:19 */
	.byte	0x00	/* base 24:31 */

/* user space data segment */
	.word	0xFFFF	/* limit 0:15 */
	.word	0x0000	/* base 0:15 */
	.byte	0x00	/* base 16:23 */
	.byte	0xF2	/* access */
	.byte	0xCF	/* flags; limit 16:19 */
	.byte	0x00	/* base 24:31 */

/* task state segment */
	.word	0x0067	/* limit 0:15 */
	.skip	2	/* base 0:15 */
	.skip	1	/* base 16:23 */
	.byte	0x89	/* access */
	.byte	0x00	/* flags; limit 16:19 */
	.skip	1	/* base 24:31 */
global_descriptor_table_end:

.globl gdtr
gdtr:
	.word global_descriptor_table_end - global_descriptor_table - 1 /* limit */
	.long global_descriptor_table /* base */

interrupt_descriptor_table:
	.word	/* offset 0:15 */
	.word	/* segment */
	.byte
	.byte
	.word	/* offset 16:31 */

.section .bss
.lcomm task_state_segment, 0x68

.section .text
.globl init_kernel
init_kernel:
	/* clear vga display */
	movw $0b0000111100000000, %ax
	movl $0xB8000, %edi
	movl $(80 * 25), %ecx
	rep stosw

	movl task_state_segment, %eax

	/* base 0:15 */
	movw %ax, (global_descriptor_table + 0x28 + 2)

	/* base 16:23 */
	shrl $16, %eax
	movb %al, (global_descriptor_table + 0x28 + 4)

	/* base 24:31 */
	movb %ah, (global_descriptor_table + 0x28 + 7)

	movw $0x28, %ax
	ltr %ax

	call kernel_main

	ret
