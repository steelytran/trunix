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

static char *
itoa(int n, int base, int ucase)
{
	int j, i = 0;
	char c;
	static char s[33];
	unsigned int un;
	const char *hex;
	const static char lower[] = "0123456789abcdef";
	const static char upper[] = "0123456789ABCDEF";

	if (n == 0) {
		s[0] = '0';
		s[1] = '\0';
		return s;
	}

	if (base < 0) {
		base = -base;
		if (n > 0)
			goto usgn;

		s[i++] = '-';
		un = (unsigned int)(-n);
		j = 1;
	} else {
usgn:
		un = (unsigned int)n;
		j = 0;
	}

	if (ucase)
		hex = upper;
	else
		hex = lower;

	while (un > 0) {
		s[i++] = hex[un % base];
		un /= base;
	} 

	s[i] = '\0';

	for (--i; j < i; --i, ++j) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}

	return s;
}

int
vfprintf(FILE *stream, const char *fmt, va_list args)
{
	int ret;
	int base, ucase;
	char *c;
	unsigned char *start = stream->_ptr;

	for (; *fmt != '\0'; ++fmt) {
		if (*fmt == '%') {
			++fmt;
			if (*fmt == '\0') {
				*stream->_ptr++ = '%';
				break;
			}

			switch (*fmt) {
			case '%':
				*stream->_ptr++ = *fmt;
				continue;
			case 'c':
				*stream->_ptr++ = (unsigned char)va_arg(args, int);
				continue;
			case 'n':
				*(int *)va_arg(args, int *) = stream->_ptr - start;
				continue;
			case 's':
				c = va_arg(args, char *);
				goto flush;
			case 'p':
				*stream->_ptr++ = '0';
				*stream->_ptr++ = 'x';
				c = itoa(va_arg(args, void *), 16, 0);
				goto flush;
			case 'X':
				ucase = 1;
				base = 16;
				break;
			case 'x':
				ucase = 0;
				base = 16;
				break;
			case 'u':
				base = 10;
				break;
			case 'i':/* FALLTHROUGH */
			case 'd':
				base = -10;
				break;
			case 'o':
				base = 8;
				break;
			case 'b':
				base = 2;
				break;
			}

			c = itoa(va_arg(args, int), base, ucase);

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
