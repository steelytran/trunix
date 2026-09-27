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

#define ASM_FILE

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

	cld
	jmp 2f

1:
	movl %ecx, %eax
	decl %eax
	addl %eax, %edi
	addl %eax, %esi
	std

2:
	rep movsb

	movl 12(%esp), %eax

	popl %edi
	popl %esi
	ret 

.globl memcpy
memcpy:
	pushl %esi
	pushl %edi

	movl 12(%esp), %edi
	movl 16(%esp), %esi
	movl 20(%esp), %ecx
	cld
	rep movsb

	movl 20(%esp), %eax

	popl %edi
	popl %esi
	ret 

.globl memcpyw
memcpyw:
	pushl %esi
	pushl %edi

	movl 12(%esp), %edi
	movl 16(%esp), %esi
	movl 20(%esp), %ecx
	cld
	rep movsw

	movl 20(%esp), %eax

	popl %edi
	popl %esi
	ret 

.globl memcpyl
memcpyl:
	pushl %esi
	pushl %edi

	movl 12(%esp), %edi
	movl 16(%esp), %esi
	movl 20(%esp), %ecx
	cld
	rep movsl

	movl 20(%esp), %eax

	popl %edi
	popl %esi
	ret 


.globl memset
memset:
	pushl %edi

	movl 8(%esp), %edi
	movb 12(%esp), %al
	movl 16(%esp), %ecx
	cld
	rep stosb

	movl 12(%esp), %eax

	popl %edi
	ret 

.globl memsetw
memsetw:
	pushl %edi

	movl 8(%esp), %edi
	movw 12(%esp), %ax
	movl 16(%esp), %ecx
	cld
	rep stosw

	movl 12(%esp), %eax

	popl %edi
	ret 

.globl memsetl
memsetl:
	pushl %edi

	movl 8(%esp), %edi
	movl 12(%esp), %eax
	movl 16(%esp), %ecx
	cld
	rep stosl

	movl 12(%esp), %eax

	popl %edi
	ret 
