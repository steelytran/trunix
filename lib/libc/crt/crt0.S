.text
.globl _start
_start:
	pushal
1:
	jmp 1b
	call main
