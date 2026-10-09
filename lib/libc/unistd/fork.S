#include "../syscall.h"

.text
.globl fork
fork:
	movl $SYS_FORK, %eax
	int $0x80
	ret
