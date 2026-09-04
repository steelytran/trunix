.macro IDT_ENTRY INT
	.org interrupt_descriptor_table + (\INT * 8)
.endm

.macro SET_IDT_BASE INT ADDR
	movl \ADDR, %eax

	/* base 0:15 */
	movw %ax, (interrupt_descriptor_table + (\INT * 8))

	/* base 16:31 */
	shrl $16, %eax
	movw %ax, (interrupt_descriptor_table + (\INT * 8) + 6)
.endm

.section .rodata
test_string:
	.asciz "scancode: 0x%x\n"

gpf_string:		.asciz "KERNEL PANIC: GENERAL PROTECTION FAULT\n"
df_string:		.asciz "KERNEL PANIC: DOUBLE FAULT\n"
pf_string:		.asciz "KERNEL PANIC: PAGE FAULT 0x%x AT 0x%x\n"
invalid_tss_string:	.asciz "KERNEL PANIC: INVALID TASK STATE SEGMENT\n"

.section .data
idtp:
	.word (256 * 8) - 1
	.long interrupt_descriptor_table

.globl keystate
keystate:
	.fill 128, 1, 0

interrupt_descriptor_table:
/* divide by zero */
IDT_ENTRY 0x00
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

/* debug exception */
IDT_ENTRY 0x01
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

/* NMI interrupt */
IDT_ENTRY 0x02
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

/* breakpoint */
IDT_ENTRY 0x03
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

/* overflow */
IDT_ENTRY 0x04
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

/* BOUND range exceeded */
IDT_ENTRY 0x05
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

/* invalid opcode */
IDT_ENTRY 0x06
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

/* math coprocessor not available */
IDT_ENTRY 0x07
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

/* double fault */
IDT_ENTRY 0x08
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

/* coprocessor segment overrun */
IDT_ENTRY 0x09
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

/* invalid TSS */
IDT_ENTRY 0x0A
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

/* segment not present */
IDT_ENTRY 0x0B
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

/* stack segment fault */
IDT_ENTRY 0x0C
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

/* general protection fault */
IDT_ENTRY 0x0D
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

/* page fault */
IDT_ENTRY 0x0E
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

/* x87 FPU error */
IDT_ENTRY 0x10
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

/* alignment check */
IDT_ENTRY 0x11
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2


/* machine check */
IDT_ENTRY 0x12
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

/* SIMD floating point exception */
IDT_ENTRY 0x13
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2


/* virtualization exception */
IDT_ENTRY 0x14
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

/* control protection exception */
IDT_ENTRY 0x15
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

IDT_ENTRY 0x21
	.skip	2
	.word	0x0008
	.byte	0x00
	.byte	0x8E
	.skip	2

.section .text

.globl setup_idt
setup_idt:
	SET_IDT_BASE 0x00, $INT_0x00
	SET_IDT_BASE 0x01, $INT_0x01
	SET_IDT_BASE 0x02, $INT_0x02
	SET_IDT_BASE 0x03, $INT_0x03
	SET_IDT_BASE 0x04, $INT_0x04
	SET_IDT_BASE 0x05, $INT_0x05
	SET_IDT_BASE 0x06, $INT_0x06
	SET_IDT_BASE 0x07, $INT_0x07
	SET_IDT_BASE 0x08, $INT_0x08
	SET_IDT_BASE 0x09, $INT_0x09
	SET_IDT_BASE 0x0A, $INT_0x0A
	SET_IDT_BASE 0x0B, $INT_0x0B
	SET_IDT_BASE 0x0C, $INT_0x0C
	SET_IDT_BASE 0x0D, $INT_0x0D
	SET_IDT_BASE 0x0E, $INT_0x0E
	SET_IDT_BASE 0x10, $INT_0x10
	SET_IDT_BASE 0x11, $INT_0x11
	SET_IDT_BASE 0x12, $INT_0x12
	SET_IDT_BASE 0x13, $INT_0x13
	SET_IDT_BASE 0x14, $INT_0x14
	SET_IDT_BASE 0x15, $INT_0x15

	SET_IDT_BASE 0x21, $INT_0x21
	lidt idtp
	ret

/* divide by zero */
INT_0x00:
	iret

/* debug exception */
INT_0x01:
	iret

/* NMI interrupt */
INT_0x02:
	iret

/* breakpoint */
INT_0x03:
	iret

/* overflow */
INT_0x04:
	iret

/* BOUND range exceeded */
INT_0x05:
	iret

/* invalid opcode */
INT_0x06:
	iret

/* math coprocessor not available */
INT_0x07:
	iret

/* double fault */
INT_0x08:
	cli
	pushl $df_string
	call printk
	addl $4, %esp
	hlt

/* coprocessor segment overrun */
INT_0x09:
	iret

/* invalid TSS */
INT_0x0A:
	cli
	pushl $invalid_tss_string
	call printk
	addl $4, %esp
	hlt

/* segment not present */
INT_0x0B:
	iret

/* stack segment fault */
INT_0x0C:
	iret

/* general protection fault*/
INT_0x0D:
	cli
	pushl $gpf_string
	call printk
	addl $4, %esp
	hlt

/* page fault */
INT_0x0E:
	popl %eax
	movl %cr2, %ebx

	pushl %ebx
	pushl %eax
	pushl $pf_string
	call printk
	addl $8, %esp
	hlt

/* x87 FPU error */
INT_0x10:
	iret

/* alignment check */
INT_0x11:
	iret

/* machine check */
INT_0x12:
	iret

/* SIMD floating point exception */
INT_0x13:
	iret

/* virtualization exception */
INT_0x14:
	iret

/* control protection exception */
INT_0x15:
	iret

/*
 * EXTERNAL INTERRUPTS
 */

INT_0x21:
/*
	xorl %eax, %eax
	inb $0x60, %al

	pushl %eax
	pushl $test_string
	call printk
	addl $8, %esp

	movb $0x20, %al
	outb %al, $0x20

*/
	iret
