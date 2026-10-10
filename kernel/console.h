/*
 * console driver
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

#ifndef _CONSOLE_H
#define _CONSOLE_H

#include "file.h"

struct console {
	const void *buf;
};

int console_open(struct inode *, struct file *);
int console_close(struct inode *, struct file *);
int console_read(struct file *file, char *buf, u32, u32 *);
int console_write(struct file *, const char *, u32, u32 *);

#endif
