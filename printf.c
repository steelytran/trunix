/*
 * formatted print
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

#include <stdarg.h>
#include <stdint.h>

#include <stdio.h>
#include <sys/tty.h>

char *
itoa(int n, unsigned int base)
{
	int j, i = 0;
	char c;
	static char s[33];
	unsigned int un;
	const static char hex[] = "0123456789abcdef";

	if (n == 0) {
		s[0] = '0';
		s[1] = '\0';
		return s;
	}

	if (n < 0 && base == 10) {
		s[i++] = '-';
		un = (unsigned int)(-n);
		j = 1;
	} else {
		un = (unsigned int)n;
		j = 0;
	}

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
printf(const char *fmt, ...)
{
	char *c;
	va_list args;
	int base;

	va_start(args, fmt);

	for (; *fmt != '\0'; ++fmt) {
		if (*fmt == '%') {
			++fmt;
			if (*fmt == '\0')
				break;
			if (*fmt == '%') {
				putchar(*fmt);
				continue;
			} if (*fmt == 's') {
				c = va_arg(args, char *);
				while (*c)
					putchar(*c++);

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

			while (*c != '\0')
				putchar(*c++);

			continue;
		}
		putchar(*fmt);
	}

	va_end(args);
	return 0;
}
