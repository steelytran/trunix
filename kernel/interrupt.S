/*
 * interrupts
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
#include <sys/kthread.h>

.data
.extern kbdstate

.globl keybuffer
keybuffer:
	.fill 12

.globl keysp
keysp:
	.word 0

.text
/* divide by zero */
.globl INT_0x00
INT_0x00:
	iret

/* debug exception */
.globl INT_0x01
INT_0x01:
	iret

/* NMI interrupt */
.globl INT_0x02
INT_0x02:
	iret

/* breakpoint */
.globl INT_0x03
INT_0x03:
	iret

/* overflow */
.globl INT_0x04
INT_0x04:
	iret

/* BOUND range exceeded */
.globl INT_0x05
INT_0x05:
	iret

/* invalid opcode */
.globl INT_0x06
INT_0x06:
	cli
	hlt
	jmp INT_0x06

/* math coprocessor not available */
.globl INT_0x07
INT_0x07:
	iret

/* double fault */
.globl INT_0x08
INT_0x08:
	cli
	hlt
	jmp INT_0x08

/* coprocessor segment overrun */
.globl INT_0x09
INT_0x09:
	iret

/* invalid TSS */
.globl INT_0x0A
INT_0x0A:
	cli
	hlt
	jmp INT_0x0A

/* segment not present */
.globl INT_0x0B
INT_0x0B:
	iret

/* stack segment fault */
.globl INT_0x0C
INT_0x0C:
	cli
	hlt
	jmp INT_0x0C

/* general protection fault*/
.globl INT_0x0D
INT_0x0D:
1:	cli
	hlt
	jmp 1b

/* page fault */
.globl INT_0x0E
INT_0x0E:
	popl %eax
1:	cli
	hlt
	jmp 1b

/* x87 FPU error */
.globl INT_0x10
INT_0x10:
	iret

/* alignment check */
.globl INT_0x11
INT_0x11:
	iret

/* machine check */
.globl INT_0x12
INT_0x12:
	iret

/* SIMD floating point exception */
.globl INT_0x13
INT_0x13:
	iret

/* virtualization exception */
.globl INT_0x14
INT_0x14:
	iret

/* control protection exception */
.globl INT_0x15
INT_0x15:
	iret

/* programmable interval timer */
.globl INT_0x20
INT_0x20:
	pushal

	movb $0x20, %al
	outb %al, $0x20

	call schedule

	popal
	iret

/* ps2 keyboard */
.globl INT_0x21
INT_0x21:
	pushl %eax
	pushl %ebx

	inb $0x60, %al
	movb %al, %ah

	/* PIC end of interrupt */
	movb $0x20, %al
	outb %al, $0x20

	/* key released */
	testb $0x80, %ah
	jnz 1f

	/* scancode */
	andb $0x7f, %ah

	incl keysp
	movl keysp, %ebx
	movb %ah, keybuffer(%ebx)

	movl kbdstate, %ebx
	movl $READY, (%ebx)

1:
	popl %ebx
	popl %eax
	iret
