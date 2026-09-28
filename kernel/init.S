.text
.globl init
init:
	jmp init

	movl $2, %eax
	int $0x80

	cmpl $1, %eax
	je 1f
	jg 2f

0:
	jmp 0b

1:
	jmp 1b

2:
	jmp 2b
