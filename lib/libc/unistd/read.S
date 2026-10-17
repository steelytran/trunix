#include <sys/syscall.h>

.text
.globl read
write:
	movl $SYS_read, %eax
	movl 12(%esp), %edx
	movl 8(%esp), %ecx
	movl 4(%esp), %ebx
	int $0x80

	ret
