#include <sys/syscall.h>

.text
.globl _exit
_exit:
	movl $SYS_exit, %eax
	movl 4(%esp), %ecx

	int $0x80
	ret
