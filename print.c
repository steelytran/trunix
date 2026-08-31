#include <stdarg.h>
#include <stdint.h>

#include "debug.h"
#include "tty.h"

static void
printdec(int n)
{
	int i = 0;
	char buffer[12];
	unsigned int un;

	if (n < 0) {
		putchar('-');
		un = (unsigned int)(-n);
	} else
		un = (unsigned int)n;

	if (un == 0) {
		putchar('0');
		return;
	}

	while (un > 0) {
		buffer[i++] = (un % 10) + '0';
		un /= 10;
	} 

	while (i > 0) {
		--i;
		putchar(buffer[i]);
	}
}

static void
printhex(unsigned int n, int ucase)
{
	int i = 0;
	int d;
	char buffer[8];
	const char *hex;

	const static char hex_u[] = "0123456789ABCDEF";
	const static char hex_l[] = "0123456789abcdef";

	hex = (ucase ? hex_u : hex_l);

	if (n == 0) {
		putchar('0');
		return;
	}

	while (n > 0) {
		buffer[i++] = hex[n % 16];
		n /= 16;
	} 

	while (i > 0) {
		--i;
		putchar(buffer[i]);
	}
}

void
printk(const char *format, ...)
{
	va_list args;

	va_start(args, format);

	for (; *format != '\0'; ++format) {
		if (*format == '%') {
			++format;
			if (*format == '\0')
				break;

			if (*format == 'd') {
				printdec(va_arg(args, int));
				continue;
			} if (*format == 'x') {
				printhex(va_arg(args, int), 0);
				continue;
			} if (*format == 'X') {
				printhex(va_arg(args, int), 1);
				continue;
			}
		}
		putchar(*format);
	}
	va_end(args);
}
