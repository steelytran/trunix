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

#include "trunix.h"
#include "proc.h"
#include "vm.h"
#include "kbd.h"

struct kinfo k;

test()
{
/*
 * for whatever reason, the first call
 * is skipped and instead switches
 * immediately, the rest are fine?
 */
	debug("asdfasdfasdfasdf\n");
	debug("asdfasdfasdfasdf\n");
	debug("asdfasdfasdfasdf\n");
	for (;;);
}

/*
 * kernel main
 */
kmain(kernel_info)
struct kinfo *kernel_info;
{
	init_gdt();
	init_tss();
	init_idt();

	init_serial();

	memcpy(&k, kernel_info, sizeof(struct kinfo));

	pg_clear_identity();
	init_mem(&k);

	load_initrd();

	initsys();
	enqueue(kthread_create(test));
	init_sched();
}
