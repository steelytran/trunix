/*
 * standard I/O
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

#ifndef _STDIO_H
#define _STDIO_H

#include <stddef.h>
#include <stdarg.h>
#include <sys/types.h>

#define BUFSIZ 1024
#define FILENAME_MAX 100
#define FOPEN_MAX 8
#define EOF -1

#define UNGET 4

enum mode {
	_IOFBF,
	_IOLBF,
	_IONBF
};

typedef struct __IO_FILE FILE;
struct __IO_FILE {
	int _fd;
	unsigned char *_buf;
	unsigned char *_ptr;
	unsigned char *_end;
	size_t _sz;
	int (*_close)(FILE *);
	size_t (*_read)(FILE *, char *, size_t);
	size_t (*_write)(FILE *, const char *, size_t);
	off_t (*_seek)(FILE *, off_t, int);
	enum mode _mode;
};

extern FILE __stdin;
extern FILE __stdout;
extern FILE __stderr;

#define stdin ((FILE *)&__stdin)
#define stdout ((FILE *)&__stdout)
#define stderr ((FILE *)&__stderr)

#define putchar(c) fputc(c, stdout)
#define putc(c, s) ((c) != '\n' && (s)->_ptr < (s)->_end) ? \
	*(s)->_ptr++ = (c) : __overflow((s), (c))

int printf(const char *, ...);
int fprintf(FILE *, const char *, ...);
int vfprintf(FILE *, const char *, va_list);
int unputc(int, FILE *);
int fputc(int, FILE *);
int fflush(FILE *);
int __overflow(FILE *, int);

#endif
