.text
.globl init
init:
	movl $2, %eax
	int $0x80
	testl %eax, %eax
	js 2f
	jz 1f

0: /* fork failed */
	jmp 0b

1: /* child */
	jmp 1b

2: /* parent */
	jmp 2b
