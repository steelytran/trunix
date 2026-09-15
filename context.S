#define ASM_FILE

#define ESP 0
#define ESP0 4
#define CR3 8
#define STATE 12
#define NEXT 13

.extern current_task_tcb
.extern tss

.text
.globl switch_to
switch_to:
	pushl %esi
	pushl %edi

	/* save current task esp */
	movl current_task_tcb, %esi
	movl %esp, ESP(%esi)

	movl 12(%esp), %edi
	movl %edi, current_task_tcb

	movl ESP(%edi), %esp
	movl ESP0(%edi), %edx
	movl CR3(%edi), %eax

	movl %edx, (tss + 4)

	/* only switch virtual address if different */
	movl %cr3, %ecx
	cmpl %eax, %ecx
	je 1f

	movl %eax, %cr3

1:
	popl %edi
	popl %esi
	ret
