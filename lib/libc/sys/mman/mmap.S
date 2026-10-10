#include <sys/syscall.h>

.text
.globl mmap
mmap:
	movl $SYS_mmap, %eax
	movl 24(%esp), %ecx
	movl 20(%esp), %edx
	movl 16(%esp), %ebx
	movl 12(%esp), %ebp
	movl 8(%esp), %esi
	movl 4(%esp), %edi
	int $0x80
	ret
