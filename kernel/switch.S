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

.text
/* void switch_to(struct proc *prev, struct proc *next); */
.globl switch_to
switch_to:
	movl 4(%esp), %eax /* old thread */
	movl 8(%esp), %edx /* new thread */

	pushl %ebx
	pushl %ebp
	pushl %esi
	pushl %edi

	movl %esp, (%eax)
	movl %edx, %esp

	popl %edi
	popl %esi
	popl %ebp
	popl %ebx
	ret

.globl trapret
trapret:
	popal
	popl %gs
	popl %fs
	popl %es
	popl %ds
	iret
