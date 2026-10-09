#include <sys/syscall.h>

.text
.globl fork
fork:
	movl $SYS_fork, %eax
	int $0x80
	ret
