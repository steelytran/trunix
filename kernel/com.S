/*
 * serial driver
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
#define COM1 0x3F8

.text
.globl init_serial
init_serial:
	movw $(COM1 + 1), %dx
	xorb %al, %al
	outb %al, %dx

	movw $(COM1 + 3), %dx
	movb $0x80, %al
	outb %al, %dx

	movw $(COM1), %dx
	movb $12, %al
	outb %al, %dx

	movw $(COM1 + 1), %dx
	xorb %al, %al
	outb %al, %dx

	movw $(COM1 + 3), %dx
	movb $0b00000011, %al
	outb %al, %dx

	movw $(COM1 + 2), %dx
	movb $0b11000111, %al
	outb %al, %dx

	movw $(COM1 + 4), %dx
	movb $0b00001011, %al
	outb %al, %dx

	/* test in loopback mode */
	movw $(COM1 + 4), %dx
	movb $0b00011110, %al
	outb %al, %dx

	movw $(COM1), %dx
	movb $0x69, %al
	outb %al, %dx

	movw $(COM1), %dx
	inb %dx, %al

	cmpb $0x69, %al
	je 1f

	call panic

1:
	movw $(COM1 + 4), %dx
	movb $0b00001011, %al
	outb %al, %dx
	ret

.globl read_serial
read_serial:
	movw $(COM1 + 5), %dx
1:
	inb %dx, %al
	testb $0b00000001, %al
	jz 1b

	movw $(COM1), %dx
	inb %dx, %al
	ret

.globl write_serial
write_serial:
	movw $(COM1 + 5), %dx
1:
	inb %dx, %al
	testb $0b00100000, %al
	jz 1b

	movw $(COM1), %dx
	movl 4(%esp), %eax
	outb %al, %dx
	ret
