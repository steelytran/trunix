#include <stdarg.h>
#include <stdint.h>

#include "debug.h"
#include "tty.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

void movecursor(int);

int tty_row = 0;
int tty_column = 0;
uint16_t* VGA = (uint16_t*)0xB8000;

void
putchar(char c)
{
	int offset;

	if (c == '\n') {
		tty_column = 0;

		if (++tty_row == VGA_HEIGHT)
			tty_row = 0;

		offset = tty_column + tty_row * VGA_WIDTH;
		movecursor(offset);
	} else {
		offset = tty_column + tty_row * VGA_WIDTH;
		VGA[offset] = (uint16_t)c | 15 << 8;

		if (++tty_column == VGA_WIDTH) {
			tty_column = 0;
			if (++tty_row == VGA_HEIGHT)
				tty_row = 0;

			offset = tty_column + tty_row * VGA_WIDTH;
			movecursor(offset);
		} else
			movecursor(offset + 1);
	}
}

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
