.section .data
string:
	.asciz "asdfasdf\nbl;sadflk\nhello"

.section .text
.globl kernel_main
kernel_main:

	pushl $string
	call printk
	addl $4, %esp
	ret
