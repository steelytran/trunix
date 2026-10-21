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

.extern printk

.data
panic_msg:	.asciz "KERNEL PANIC!\n"
pf_msg:		.asciz "Page fault: 0b%b\nAddress: 0x%x\n"
inv_op_msg:	.asciz "Invalid opcode"
inv_tss_msg:	.asciz "Invalid TSS\n"
gpf_msg:	.asciz "General protection fault\n"
num:		.asciz "0x%x: %d\n"

.bss
.globl keybuf
keybuf:
	.skip 64

.globl w_ndx
w_ndx:
	.skip 1

.globl r_ndx
r_ndx:
	.skip 1

.text
/* divide by zero */
.globl INT_0x00
INT_0x00:
	iret

/* printk exception */
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
	pushl $panic_msg
	call printk
	addl $4, %esp

	pushl $inv_op_msg
	call printk

1:
	cli
	hlt
	jmp 1b

/* math coprocessor not available */
.globl INT_0x07
INT_0x07:
	iret

/* double fault */
.globl INT_0x08
INT_0x08:
	cli
	popl %eax
	hlt
	jmp INT_0x08

/* coprocessor segment overrun */
.globl INT_0x09
INT_0x09:
	iret

/* invalid TSS */
.globl INT_0x0A
INT_0x0A:
	pushl $panic_msg
	call printk
	addl $4, %esp

	pushl $inv_tss_msg
	call printk

1:	cli
	hlt
	jmp 1b

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
	pushl $panic_msg
	call printk
	addl $4, %esp

	pushl $gpf_msg
	call printk

1:	cli
	hlt
	jmp 1b

/* page fault */
.globl INT_0x0E
INT_0x0E:
	popl %eax
	movl %cr2, %ecx

	pushl $panic_msg
	call printk
	addl $4, %esp

	pushl %ecx
	pushl %eax
	pushl $pf_msg
	call printk

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
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs
	pushal

	movb $0x20, %al
	outb %al, $0x20

	call yield

	popal
	popl %gs
	popl %fs
	popl %es
	popl %ds
	iret

/* ps2 keyboard */
.globl INT_0x21
INT_0x21:
	pushl %eax
	pushl %ecx
	pushl %edx
	pushl %edi

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

	movl $keybuf, %edi
	movzbl w_ndx, %ecx

	movb %ah, (%edi, %ecx)

	cmpb $64, %cl
	jb 0f

	movb $0, w_ndx
	jmp 1f
0:
	incb w_ndx

1:
	popl %edi
	popl %edx
	popl %ecx
	popl %eax
	iret

/* syscall */
.globl INT_0x80
INT_0x80:
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs
	pushal

	call *tbl_syscall(, %eax, 4)
	movl %eax, (7 * 4)(%esp)

end_syscall:
	popal
	popl %gs
	popl %fs
	popl %es
	popl %ds
	iret

/* for syscalls that take no arguments */
#define fork sys_fork

tbl_syscall:
/* 0 */ .long restart
/* 1 */ .long exit 
/* 2 */ .long fork 
/* 3 */ .long read 
/* 4 */ .long write
/* 5 */ .long open 
/* 6 */ .long close
/* 7 */ .long creat
/* 8 */ .long execve
/* 9 */ .long mmap 
/* 10 */ .long wait 

/* unimplemented syscalls go here */
restart:
	ret

exit:
	pushl %ecx
	call sys_exit
	addl $4, %esp
	ret

read:
	pushl %edx
	pushl %ecx
	pushl %ebx
	call sys_read
	addl $3 * 4, %esp
	ret

write:
	pushl %edx
	pushl %ecx
	pushl %ebx
	call sys_write
	addl $3 * 4, %esp
	ret

open:
	pushl %edx
	pushl %ecx
	pushl %ebx
	call sys_open
	addl $3 * 4, %esp
	ret

close:
	ret

creat:
	ret

execve:
	pushl %edx
	pushl %ecx
	pushl %ebx
	call sys_execve
	addl $3 * 4, %esp
	ret

mmap:
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushl %ebp
	pushl %esi
	pushl %edi
	call sys_mmap
	addl $6 * 4, %esp
	ret

wait:
	pushl %ecx
	call sys_wait
	addl $4, %esp
	ret
