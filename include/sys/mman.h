/*
 * memory management
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

#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#define __NEED_size_t
#include <trunix/types.h>

#define PROT_READ 0x01
#define PROT_WRITE 0x02
/* #define PROT_EXEC 0x04*/
#define PROT_NONE 0x00

#define MAP_ANONYMOUS 0x01
#define MAP_ANON MAP_ANONYMOUS
#define MAP_FIXED 0x02
#define MAP_PRIVATE 0x04
#define MAP_SHARED 0x08

void *mmap(void *, size_t, int, int, int, long);
void munmap(void *, size_t);

#endif
