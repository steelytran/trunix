/*
 * c string functions
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
.globl memcpy
memcpy:
	pushl %ecx
	pushl %esi
	pushl %edi

	movl 16(%esp), %edi
	movl 20(%esp), %esi
	movl 24(%esp), %ecx
	rep movsb

	movl 20(%esp), %eax

	popl %edi
	popl %esi
	popl %ecx
	ret 

.globl memcpyw
memcpyw:
	pushl %ecx
	pushl %esi
	pushl %edi

	movl 16(%esp), %edi
	movl 20(%esp), %esi
	movl 24(%esp), %ecx
	rep movsw

	movl 20(%esp), %eax

	popl %edi
	popl %esi
	popl %ecx
	ret 

.globl memcpyl
memcpyl:
	pushl %ecx
	pushl %esi
	pushl %edi

	movl 16(%esp), %edi
	movl 20(%esp), %esi
	movl 24(%esp), %ecx
	rep movsl

	movl 20(%esp), %eax

	popl %edi
	popl %esi
	popl %ecx
	ret 


.globl memset
memset:
	pushl %ecx
	pushl %edi

	movl 12(%esp), %edi
	movb 16(%esp), %al
	movl 20(%esp), %ecx
	rep stosb

	movl 12(%esp), %eax

	popl %edi
	popl %ecx
	ret 

.globl memsetw
memsetw:
	pushl %ecx
	pushl %edi

	movl 12(%esp), %edi
	movl 16(%esp), %eax
	movl 20(%esp), %ecx
	rep stosw

	movl 12(%esp), %eax

	popl %edi
	popl %ecx
	ret 

.globl memsetl
memsetl:
	pushl %ecx
	pushl %edi

	movl 12(%esp), %edi
	movl 16(%esp), %eax
	movl 20(%esp), %ecx
	rep stosl

	movl 12(%esp), %eax

	popl %edi
	popl %ecx
	ret 
