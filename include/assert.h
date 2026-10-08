/*
 * assertations
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

#ifndef _ASSERT_H
#define _ASSERT_H

#include <sys/cdefs.h>

#ifdef NDEBUG
#define assert(x) (void)(0)
#else
#define assert(x) ((x) ? (void)0 : __assert_fail(#x, __FILE__, __LINE__, __func__))
#endif


__dead void __assert_fail (const char *, const char *, int, const char *);

#endif

