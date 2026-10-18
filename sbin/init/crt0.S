.text

.globl _start
_start:
	call main

	pushl %eax
	call _exit
