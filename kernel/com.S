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
	/* disable interrupts */
	movw $(COM1 + 1), %edx
	xorb %al, %al
	outb %al, %dx

	/* enable divisor latch access bit */
	movw $(COM1 + 3), %edx
	movb $0x80, %al
	outb %al, %dx

	/* 115200 / 12 = 9600 baud*/
	movw $(COM1), %edx
	movb $12, %al
	outb %al, %dx

	/* divisor high byte */
	movw $(COM1 + 1), %edx
	xorb %al, %al
	outb %al, %dx

	/* 8 bits for data, no parity bits, 1 stop bit */
	movw $(COM1 + 3), %edx
	movb $0b00000011, %al
	outb %al, %dx

	ret
