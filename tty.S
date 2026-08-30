.section .text
.globl movecursor
movecursor:
	pushl %eax
	pushl %edx

	movw $0x03D4, %dx
	movb $0x0F, %al
	outb %al, %dx

	incw %dx
	movl 12(%esp), %eax
	outb %al, %dx
	
	decw %dx
	movb $0x0E, %al
	outb %al, %dx

	incw %dx
	movb %ah, %al
	outb %al, %dx

	popl %edx
	popl %eax
	ret
