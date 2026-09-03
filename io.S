.section .text

.globl inb
inb:
	pushl %edx

	movw 8(%esp), %dx
	inb %dx, %al

	popl %edx
	ret

.globl inw
inw:
	pushl %edx

	movw 8(%esp), %dx
	inw %dx, %ax

	popl %edx
	ret

.globl inl
inl:
	pushl %edx

	movw 8(%esp), %dx
	inl %dx, %eax

	popl %edx
	ret

.globl outb
outb:
	pushl %edx

	movw 8(%esp), %dx
	movb 12(%esp), %al
	outb %al, %dx 

	popl %edx
	ret

.globl outw
outw:
	pushl %edx

	movw 8(%esp), %dx
	movw 12(%esp), %ax
	outw %ax, %dx 

	popl %edx
	ret

.globl outl
outl:
	pushl %edx

	movw 8(%esp), %dx
	movl 12(%esp), %eax
	outl %eax, %dx 

	popl %edx
	ret
