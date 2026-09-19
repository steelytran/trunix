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

#include <trunix/trunix.h>
#include <trunix/tty.h>
#include <sys/kthread.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>

#include "kbd.h"

struct kinfo k;

/*
 * kernel main
 */
void
kmain(struct kinfo *kernel_info)
{
	uint32_t *fs;
	int i;

	init_gdt();
	init_tss();
	init_idt();

	memcpy(&k, kernel_info, sizeof(struct kinfo));

	pg_clear_identity();
	init_mem(&k);

	cls();

	//load_initrd(&k);

	kthread_init();
	init_kbd();
	sched_init();
}
