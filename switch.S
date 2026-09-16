#define ASM_FILE

#define ESP 0
#define ESP0 4
#define CR3 8
#define STATE 12
#define NEXT 13

#define PREV_TASK 20
#define NEXT_TASK 24

.text
/* void switch_to(struct thread *prev, struct thread *next); */
.globl switch_to
switch_to:
	pushl %ebx
	pushl %esi
	pushl %edi
	pushl %ebp

	/* save prev task sp */
	movl PREV_TASK(%esp), %esi
	movl %esp, ESP(%esi)

	/* next task sp */
	movl NEXT_TASK(%esp), %edi
	movl ESP(%edi), %esp

1:
	popl %ebp
	popl %edi
	popl %esi
	popl %ebx

	sti
	ret
