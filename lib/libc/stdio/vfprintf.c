/*
 * formatted print - vfprintf
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
#include <stdarg.h>

extern char *itoa(int, unsigned);

int
vfprintf(FILE *stream, const char *fmt, va_list args)
{
	int ret;
	char *c;
	int base;
	for (; *fmt != '\0'; ++fmt) {
		if (*fmt == '%') {
			++fmt;
			if (*fmt == '\0')
				break;
			if (*fmt == '%') {
				*stream->_ptr++ = *fmt;
				continue;
			} if (*fmt == 's') {
				c = va_arg(args, char *);
				goto flush;
			} if (*fmt == 'c') {
				*stream->_ptr++ = va_arg(args, char);
				continue;
			}

			switch (*fmt) {
			case 'X':
			case 'x':
				base = 16;
				break;
			case 'd':
				base = 10;
				break;
			case 'o':
				base = 8;
				break;
			case 'b':
				base = 2;
				break;
			}

			c = itoa(va_arg(args, int), base);

flush:
			while (*c != '\0')
				*stream->_ptr++ = *c++;

			continue;
		}
		*stream->_ptr++ = (*fmt);
	}

	ret = stream->_ptr - stream->_buf;
	fflush(stream);

	return ret;
}
