.section .text
.globl memcpy
memcpy:
	pushl %ecx
	pushl %esi
	pushl %edi

	movl 16(%esp), %edi
	movl 20(%esp), %esi
	movl 24(%esp), %ecx
	rep movsb

	movl 20(%esp), %eax

	popl %edi
	popl %esi
	popl %ecx
	ret 

.globl memset
memset:
	pushl %ecx
	pushl %edi

	movl 12(%esp), %edi
	movb 16(%esp), %al
	movl 20(%esp), %ecx
	rep stosb

	movl 12(%esp), %eax

	popl %edi
	popl %ecx
	ret 

.globl memsetl
memsetl:
	pushl %ecx
	pushl %edi

	movl 12(%esp), %edi
	movl 16(%esp), %eax
	movl 20(%esp), %ecx
	rep stosl

	movl 12(%esp), %eax

	popl %edi
	popl %ecx
	ret 
