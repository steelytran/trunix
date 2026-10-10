#include <sys/syscall.h>

.text
.globl open
open:
	movl $SYS_open, %eax
	movl 12(%esp), %edx
	movl 8(%esp), %ecx
	movl 4(%esp), %ebx
	int $0x80

	ret
