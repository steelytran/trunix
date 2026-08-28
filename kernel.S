.section .data
string:
	.asciz "asdfasdf\nbl;sadflk\nhello"

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
	.word	0xFFFF	/* limit 0:15 */
	.word	0x0000	/* base 0:15 */
	.byte	0x00	/* base 16:23 */
	.byte	0x89	/* access */
	.byte	0x00	/* flags; limit 16:19 */
	.byte	0x00	/* base 24:31 */
global_descriptor_table_end:

.globl gdtr
gdtr:
	.word global_descriptor_table_end - global_descriptor_table - 1 /* limit */
	.long global_descriptor_table /* base */

.section .bss
.lcomm task_state_segment, 0x68

.section .text
.globl kernel_main
kernel_main:
	movw $0b0000111100000000, %ax
	movl $0xB8000, %edi
	movl $(80 * 25), %ecx

	rep stosw
	pushl $string
	call printk
	addl $4, %esp
	ret
