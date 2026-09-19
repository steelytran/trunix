/*
 * context switching
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

#define ESP 0
#define ESP0 4
#define CR3 8
#define STATE 12
#define NEXT 13

#define PREV_TASK 20
#define NEXT_TASK 24

.text
/* void switch_to(struct task *prev, struct task *next); */
.globl switch_to
switch_to:
	pushl %ebx
	pushl %esi
	pushl %edi
	pushl %ebp

	/* save prev task sp */
	movl PREV_TASK(%esp), %esi
	movl %esp, ESP(%esi)

	/* next task sp */
	movl NEXT_TASK(%esp), %edi
	movl ESP(%edi), %esp

1:
	popl %ebp
	popl %edi
	popl %esi
	popl %ebx

	sti
	ret
