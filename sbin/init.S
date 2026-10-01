.text
path:
	.skip 1

.globl _start
_start:
	jmp _start

	movl $2, %eax
	int $0x80
	testl %eax, %eax
	js 3f
	jz 2f

1: /* parent */
	jmp 1b

2: /* child */
/*
	movl $8, %eax
	movl $path, %ebx
	movl $0, %ecx
	movl $0, %edx
	int $0x80
*/
0:
	jmp 0b

3: /* fork failed */
	jmp 3b
