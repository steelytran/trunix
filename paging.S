.globl vm_enable_paging
vm_enable_paging:

	movl 4(%esp), %ecx
	movl %ecx, %cr3

	movl %cr3, %ecx
	andl $~0x80000000, %ecx
	movl %ecx, %cr3

	movl %cr4, %ecx
	andl $~(0x80 | 0x10), %ecx
	movl %ecx, %cr4

	/* Page size extensions */
	movl %cr4, %ecx
	orl $0x00000010, %ecx
	movl %ecx, %cr4

	movl %cr0, %ecx
	orl $0x80000000, %ecx
	movl %ecx, %cr0

	movl %cr0, %ecx
	orl $0x00010000, %ecx
	movl %ecx, %cr0

	movl %cr4, %ecx
	orl $0x80, %ecx
	movl %ecx, %cr4

	movl 4(%esp), %eax
	ret
