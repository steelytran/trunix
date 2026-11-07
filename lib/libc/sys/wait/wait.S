#include <sys/syscall.h>

.text
.globl wait
wait:
	movl $SYS_wait, %eax
	movl 4(%esp), %ecx
	int $0x80
	ret
