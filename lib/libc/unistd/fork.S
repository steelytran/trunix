#include <sys/syscall.h>

.text
.globl fork
fork:
	movl $SYS_fork, %eax
	addl $4, %esp
	int $0x80
	ret
