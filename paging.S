/*
 * lower level memory management - assembly helper routines
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
.globl flush_tlb
flush_tlb:
	movl %cr3, %ecx
	movl %ecx, %cr3
	ret

.globl vm_enable_paging
vm_enable_paging:

	movl 4(%esp), %ecx
	movl %ecx, %cr3

	movl %cr3, %ecx
	andl $~0x80000000, %ecx
	movl %ecx, %cr3

	movl %cr4, %ecx
	andl $~(0x80 | 0x10), %ecx
	movl %ecx, %cr4

	/* Page size extensions */
	movl %cr4, %ecx
	orl $0x00000010, %ecx
	movl %ecx, %cr4

	movl %cr0, %ecx
	orl $0x80000000, %ecx
	movl %ecx, %cr0

	movl %cr0, %ecx
	orl $0x00010000, %ecx
	movl %ecx, %cr0

	movl %cr4, %ecx
	orl $0x80, %ecx
	movl %ecx, %cr4

	movl 4(%esp), %eax
	ret
