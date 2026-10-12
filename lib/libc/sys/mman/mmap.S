#include <sys/syscall.h>

.text
.globl mmap
mmap:
	pushl %ebp
	pushl %ebx
	pushl %esi
	pushl %edi

	movl $SYS_mmap, %eax
	movl 40(%esp), %ecx
	movl 36(%esp), %edx
	movl 32(%esp), %ebx
	movl 28(%esp), %ebp
	movl 24(%esp), %esi
	movl 20(%esp), %edi
	int $0x80

	popl %edi
	popl %esi
	popl %ebx
	popl %ebp
	ret
