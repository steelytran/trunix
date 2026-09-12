/*
 * descriptor tables
 * Copyright (C) 2026  spenna
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#define ASM_FILE

.macro IDT_ENTRY INT
	.org idt + (\INT * 8)
.endm

.macro SET_IDT_BASE INT ADDR
	movl \ADDR, %eax

	/* base 0:15 */
	movw %ax, (idt + (\INT * 8))

	/* base 16:31 */
	shrl $16, %eax
	movw %ax, (idt + (\INT * 8) + 6)
.endm

.data
gdt:
/*
 * GDT entry format
 * limit 0:15 
 * base 0:15 
 * base 16:23 
 * access 
 * flags; limit 16:19 
 * base 24:31 
 */

/* null descriptor */
	.fill 1, 8, 0

/* kernel space code segment */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0x9A
	.byte	0xCF
	.byte	0x00

/* kernel space data segment */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0x92
	.byte	0xCF
	.byte	0x00

/* user space code segment */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0xFA
	.byte	0xCF
	.byte	0x00

/* user space data segment */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0xF2
	.byte	0xCF
	.byte	0x00

/* task state segment */
	.word	0x2068
	.skip	2
	.skip	1
	.byte	0x89
	.byte	0x00
	.skip	1
gdt_end:

gdtr:
	.word gdt_end - gdt - 1 /* limit */
	.long gdt /* base */

iopb:
	.fill 0x2000, 1, 0	/* UNSAFE!! THIS IS ONLY TEMPORARY FOR TESTING */
	.byte 0xFF		/* PERMITS EVERY PORT */

tss:
	.skip 0x68 

idt:
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

idtp:
	.word (256 * 8) - 1
	.long idt

.text
.globl init_gdt
init_gdt:
/*
 * initialise global descriptor table
 */
	cli

	lgdt gdtr
	ljmpl $0x08, $.reload_cs
.reload_cs:
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss

	sti
	ret

.globl init_tss
init_tss:
	cli

	movl $tss, %eax

	/* base 0:15 */
	movw %ax, (gdt + 0x28 + 2)

	/* base 16:23 */
	shrl $16, %eax
	movb %al, (gdt + 0x28 + 4)

	/* base 24:31 */
	movb %ah, (gdt + 0x28 + 7)

	movw $0x28, %ax
	ltr %ax

	movw $(iopb - tss), %ax
	movw %ax, (tss + 0x66)

	sti
	ret

.globl init_idt
init_idt:
	cli

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

	lidt idtp

/*
 * setup 8259 PIC to offset hardware IRQs,
 * so that they do not conflict with
 * reserved i386 protected mode cpu IRQs.
 */
	movb $0x11, %al
	outb %al, $0x20
	outb %al, $0xA0

/* vector offset */
	movb $0x20, %al
	outb %al, $0x21
	movb $0x28, %al
	outb %al, $0xA1

/* cascade */
	movb $0x04, %al
	outb %al, $0x21
	movb $0x02, %al
	outb %al, $0xA1

/* 8086 mode (and not 8080) */
	movb $0x01, %al
	outb %al, $0x21
	outb %al, $0xA1

/* mask */
	movb $0xFD, %al
	outb %al, $0x21
	movb $0xFF, %al
	outb %al, $0xA1

	sti
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
	hlt

/* coprocessor segment overrun */
INT_0x09:
	iret

/* invalid TSS */
INT_0x0A:
1:	cli
	hlt
	jmp 1b

/* segment not present */
INT_0x0B:
	iret

/* stack segment fault */
INT_0x0C:
1:	cli
	hlt
	jmp 1b

/* general protection fault*/
INT_0x0D:
1:	cli
	hlt
	jmp 1b

/* page fault */
INT_0x0E:
	popl %eax
1:	cli
	hlt
	jmp 1b

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
