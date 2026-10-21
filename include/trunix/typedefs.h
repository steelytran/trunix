/*
 * types
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

/*
 * builtin types
 */
#if defined(__NEED_NULL) && !defined(__DEFINED_NULL)
#define NULL ((void *)0)
#define __DEFINED_NULL
#endif

#if defined(__NEED_size_t) && !defined(__DEFINED_size_t)
typedef unsigned long size_t;
#define __DEFINED_size_t
#endif

#if defined(__NEED_va_list) && !defined(__DEFINED_va_list)
typedef __builtin_va_list va_list;
#define __DEFINED_va_list
#endif


/*
 * posix types
 */
#if defined(__NEED_ssize_t) && !defined(__DEFINED_ssize_t)
typedef long ssize_t;
#define __DEFINED_ssize_t
#endif

#if defined(__NEED_loff_t) && !defined(__DEFINED_loff_t)
typedef long long loff_t;
#define __DEFINED_loff_t
#endif

#if defined(__NEED_off_t) && !defined(__DEFINED_off_t)
typedef long off_t;
#define __DEFINED_off_t
#endif

#if defined(__NEED_pid_t) && !defined(__DEFINED_pid_t)
typedef int pid_t;
#define __DEFINED_pid_t
#endif

#if defined(__NEED_dev_t) && !defined(__DEFINED_dev_t)
typedef int dev_t;
#define __DEFINED_dev_t
#endif



#if defined(__NEED_u8) && !defined(__DEFINED_u8)
typedef unsigned char u8;
#define __DEFINED_u8
#endif

#if defined(__NEED_u16) && !defined(__DEFINED_u16)
typedef unsigned short u16;
#define __DEFINED_u16
#endif

#if defined(__NEED_u32) && !defined(__DEFINED_u32)
typedef unsigned int u32;
#define __DEFINED_u32
#endif

#if defined(__NEED_u64) && !defined(__DEFINED_u64)
typedef unsigned long long u64;
#define __DEFINED_u64
#endif
