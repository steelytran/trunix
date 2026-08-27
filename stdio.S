.set VGA_WIDTH, 80
.set VGA_HEIGHT, 25

.section .data

tty_row:
	.long 1

tty_column:
	.long 50

.section .text

.globl printk
printk:
	pushl %eax
	pushl %edx
	pushl %esi
	pushl %edi

	movl 20(%esp), %esi
	cld
1:
	movl tty_row, %eax
	imull $VGA_WIDTH, %eax
	addl tty_column, %eax
	shll $1, %eax

	movl $0xB8000, %edi
	addl %eax, %edi

	movb $0b00001111, %ah

2:
	lodsb
	testb %al, %al /* null termination */
	jz 3f

	cmpb $'\n', %al
	je newline

	stosw /* word, first byte is color / blink bit */
	incl tty_column

	cmpl $VGA_WIDTH, tty_column
	jb 2b

newline:
	incl tty_row
	movl $0, tty_column
	jmp 1b

/* move cursor to end of output */
3:
	movw $0x03D4, %dx
	movb $0x0F, %al
	outb %al, %dx

	incw %dx /* crt data register: 0x03D5 */
	movl %edi, %eax
	subl $0xB8000, %eax
	shrl $1, %eax
	outb %al, %dx
	
	decw %dx
	movb $0x0E, %al
	outb %al, %dx

	incw %dx
	movb %ah, %al
	outb %al, %dx

	popl %edi
	popl %esi
	popl %edx
	popl %eax
	ret
