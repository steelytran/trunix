/*
 * asm utilities for c
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

.macro ARG_LONG_GET FUNC REG
.globl \FUNC
\FUNC:
	movl \REG, %eax
	ret
.endm

.macro ARG_LONG_SET FUNC REG
.globl \FUNC
\FUNC:
	movl 4(%esp), %eax
	movl %eax, \REG
	jmp 1f
1:
	ret
.endm

.text
	ARG_LONG_GET read_cr4, %cr4
	ARG_LONG_GET read_cr3, %cr3
	ARG_LONG_GET read_cr2, %cr2
	ARG_LONG_GET read_cr1, %cr1
	ARG_LONG_GET read_cr0, %cr0

	ARG_LONG_GET read_eax, %eax
	ARG_LONG_GET read_ebx, %ebx
	ARG_LONG_GET read_ecx, %ecx
	ARG_LONG_GET read_edx, %edx

	ARG_LONG_GET read_esp, %esp

	ARG_LONG_SET write_cr4, %cr4
	ARG_LONG_SET write_cr3, %cr3
	ARG_LONG_SET write_cr2, %cr2
	ARG_LONG_SET write_cr1, %cr1
	ARG_LONG_SET write_cr0, %cr0

	ARG_LONG_SET write_eax, %eax
	ARG_LONG_SET write_ebx, %ebx
	ARG_LONG_SET write_ecx, %ecx
	ARG_LONG_SET write_edx, %edx

.globl panic
panic:
1:
	cli
	hlt
	jmp 1b
