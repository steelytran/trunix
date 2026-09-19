#include <sys/types.h>
#include <trunix/file.h>
#include <trunix/trunix.h>
#include <sys/kthread.h>
#include <stdio.h>
#include <stdint.h>

#include "kbd.h"

extern uint8_t keybuffer[12];
extern unsigned short keysp;

enum taskstate* kbdstate;

struct con {
	char buf[128];
	unsigned short r;
	unsigned short w;
	unsigned short e;
};

ssize_t
consolewrite(int fd_dst, uintptr_t con_src, ssize_t n)
{

}

ssize_t
consoleread(int fd_src, uintptr_t con_dst, ssize_t n)
{

}

void
init_console(void)
{
	devrw[CONSOLE].read = consoleread;
	devrw[CONSOLE].write = consolewrite;
}

static void
kbd_bh(void)
{
	for (;;) {
		cli();
		for (; keysp > 0; --keysp) {
			if (keybuffer[keysp] == 0)
				continue;

			putchar(normalmap[keybuffer[keysp]]);
			keybuffer[keysp] = 0;
		}
		*kbdstate = BLOCKED;
		sti();
	}
}

void
init_kbd(void)
{
	kthread_create(kbd_bh);
	kbdstate = &curthread->next->state;
}
