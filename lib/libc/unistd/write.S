#include <sys/syscall.h>

.text
.globl write
write:
	movl $SYS_write, %eax
	movl 12(%esp), %edx
	movl 8(%esp), %ecx
	movl 4(%esp), %ebx
	int $0x80

	ret
