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

			else if (*format == 'd')
				c = itoa(va_arg(args, int), 10);
			else if (*format == 'x')
				c = itoa(va_arg(args, int), 16);
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
