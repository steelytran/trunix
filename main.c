/*
 * The trunix operating system.
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

#include <stdio.h>
#include <string.h>

#include <sys/trunix.h>
#include <sys/tty.h>

struct kinfo *kernel_info;

/*
 * kernel main
 */
void
kmain(struct kinfo *k)
{
	init_gdt();
	init_tss();
	init_idt();

	memcpy(kernel_info, k, sizeof(struct kinfo));

	cls();

	printf("Welcome to Trunix!\n");
	return;
}
