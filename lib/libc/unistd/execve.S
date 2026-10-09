#include <sys/syscall.h>

.text
.globl execve
execve:
	movl $SYS_execve, %eax
	movl 12(%esp), %edx
	movl 8(%esp), %ecx
	movl 4(%esp), %ebx
	int $0x80

	ret
