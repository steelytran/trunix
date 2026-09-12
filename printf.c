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

static char *
itoa(int n, unsigned int base)
{
	int j, i = 0;
	char c;
	static char s[33];
	unsigned int un;
	const static char hex[] = "0123456789abcdef";

	if (n == 0) {
		s[0] = '0';
		return s;
	}

	if (n < 0) {
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
printf(const char *format, ...)
{
	char *c;
	va_list args;

	va_start(args, format);

	for (; *format != '\0'; ++format) {
		if (*format == '%') {
			++format;
			if (*format == '\0')
				break;
			if (*format == '%') {
				putchar(*format);
				continue;
			}

			else if (*format == 'x')
				c = itoa(va_arg(args, int), 16);
			else if (*format == 'd')
				c = itoa(va_arg(args, int), 10);
			else if (*format == 'b')
				c = itoa(va_arg(args, int), 2);

			while (*c)
				putchar(*c++);

			continue;
		}
		putchar(*format);
	}

	va_end(args);
	return 0;
}
