.text
.globl _start
_start:
	pushl %ecx /* envp */
	pushl %edx /* argv */
	pushl %eax /* argc */
	xorl %eax, %eax

	call main

	pushl %eax
	call _exit
	hlt
