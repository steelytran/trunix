/*
 * basic vga text mode driver
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

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

.extern _VGA_TEXT

.text
.globl cls
cls:
	pushl %ecx
	pushl %edi

	movl $_VGA_TEXT, %edi
	movw $(15 << 8), %ax
	movl $(VGA_WIDTH * VGA_HEIGHT), %ecx
	rep stosw

	pushl $0
	call movecursor
	addl $4, %esp

	popl %edi
	popl %ecx
	ret 

.globl movecursor
movecursor:
	pushl %eax
	pushl %edx

	movw $0x03D4, %dx
	movb $0x0F, %al
	outb %al, %dx

	incw %dx
	movl 12(%esp), %eax
	outb %al, %dx
	
	decw %dx
	movb $0x0E, %al
	outb %al, %dx

	incw %dx
	movb %ah, %al
	outb %al, %dx

	popl %edx
	popl %eax
	ret

.globl putchar
putchar:
	pushl %eax
	cmpb $'\n', 8(%esp)
	je newline

	movb $0b00001111, %ah
	movb 8(%esp), %al

	pushl %ebx
	imull $VGA_WIDTH, tty_row, %ebx
	addl tty_column, %ebx
	movw %ax, _VGA_TEXT(, %ebx, 2)

	incl tty_column
	cmpl $VGA_WIDTH, tty_column
	jne 1f

	movl $0, tty_column

	incl tty_row
	cmpl $VGA_HEIGHT, tty_row
	jne 2f

	movl $0, tty_row

2:
	imull $VGA_WIDTH, tty_row, %ebx

	pushl %ebx
	call movecursor
	addl $4, %esp

	popl %ebx
	jmp end
1:
	incl %ebx
	pushl %ebx
	call movecursor
	addl $4, %esp

	popl %ebx
	jmp end

newline:
	movl $0, tty_column

	incl tty_row
	cmpl $VGA_HEIGHT, tty_row
	jne 1f

	movl $0, tty_row

1:
	imull $VGA_WIDTH, tty_row, %eax

	pushl %eax
	call movecursor
	addl $4, %esp

end:
	popl %eax
	ret

.data
tty_row:
	.long 0
tty_column:
	.long 0
