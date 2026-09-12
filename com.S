/* WIP */
#define ASM_FILE
#define COM1 0x3F8

.text
.globl init_serial
init_serial:
	/* disable interrupts */
	movw $(COM1 + 1), %edx
	xorb %al, %al
	outb %al, %dx

	/* enable divisor latch access bit */
	movw $(COM1 + 3), %edx
	movb $0x80, %al
	outb %al, %dx

	/* 115200 / 12 = 9600 baud*/
	movw $(COM1), %edx
	movb $12, %al
	outb %al, %dx

	/* divisor high byte */
	movw $(COM1 + 1), %edx
	xorb %al, %al
	outb %al, %dx

	/* 8 bits for data, no parity bits, 1 stop bit */
	movw $(COM1 + 3), %edx
	movb $0b00000011, %al
	outb %al, %dx

	ret
