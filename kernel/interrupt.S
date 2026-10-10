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
	iret

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

1:
	popl %ebx
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

	jmp *tbl_syscall(, %eax, 4)

end_syscall:
	popal
	popl %gs
	popl %fs
	popl %es
	popl %ds
	iret

tbl_syscall:
	.long restart /* 0 */
	.long exit /* 1 */
	.long fork /* 2 */
	.long read /* 3 */
	.long write /* 4 */
	.long open /* 5 */
	.long close /* 6 */
	.long creat /* 7 */
	.long execve /* 8 */
	.long mmap /* 9 */

/*
 * temporary for unimplemented syscalls
 */
restart:
	jmp end_syscall

exit:
	jmp end_syscall

fork:
	call sys_fork
	movl %eax, (7 * 4)(%esp)
	jmp end_syscall

read:
	jmp end_syscall

write:
	pushl %edx
	pushl %ecx
	pushl %ebx
	call sys_write
	addl $3 * 4, %esp
	movl %eax, (7 * 4)(%esp)
	jmp end_syscall

open:
	pushl %edx
	pushl %ecx
	pushl %ebx
	call sys_open
	addl $3 * 4, %esp
	movl %eax, (7 * 4)(%esp)
	jmp end_syscall

close:
	jmp end_syscall

creat:
	jmp end_syscall

execve:
	pushl %edx
	pushl %ecx
	pushl %ebx
	call sys_execve
	addl $3 * 4, %esp
	movl %eax, (7 * 4)(%esp)
	jmp end_syscall

mmap:
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushl %ebp
	pushl %esi
	pushl %edi
	call sys_mmap
	addl $6 * 4, %esp
	movl %eax, (7 * 4)(%esp)
	jmp end_syscall
