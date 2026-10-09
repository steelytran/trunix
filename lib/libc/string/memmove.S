/*
 * libc string.h - asm
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

.text
.globl memmove
memmove:
	pushl %esi
	pushl %edi

	movl 12(%esp), %edi
	movl 16(%esp), %esi
	movl 20(%esp), %ecx

	cmpl %esi, %edi
	jg 1f

	movl %ecx, %eax
	decl %eax
	addl %eax, %edi
	addl %eax, %esi
	std

1:
	rep movsb

	movl 12(%esp), %eax

	cld
	popl %edi
	popl %esi
	ret 
