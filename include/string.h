/*
 * string
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

#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

void *memcpy(void *restrict dst, const void *restrict src, size_t n);
void *memcpyl(void *restrict dst, const void *restrict src, size_t n);
void *memcpyw(void *restrict dst, const void *restrict src, size_t n);
void *memset(void *b, int c, size_t len);
void *memsetw(void *b, int c, size_t len);
void *memsetl(void *b, int c, size_t len);

int memcmp(const void *, const void *, size_t);

int strcmp(const char *, const char *);
size_t strlen(const char *);

char *strncpy(char *, const char *, size_t);

#endif
