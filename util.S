#define ASM_FILE

.macro ARG_LONG_GET FUNC REG
.globl \FUNC
\FUNC:
	movl \REG, %eax
	ret
.endm

.macro ARG_LONG_SET FUNC REG
.globl \FUNC
\FUNC:
	movl 4(%esp), %eax
	movl %eax, \REG
	jmp 1f
1:
	ret
.endm

	ARG_LONG_GET read_cr4, %cr4
	ARG_LONG_GET read_cr3, %cr3
	ARG_LONG_GET read_cr2, %cr2
	ARG_LONG_GET read_cr1, %cr1
	ARG_LONG_GET read_cr0, %cr0

	ARG_LONG_GET read_eax, %eax
	ARG_LONG_GET read_ebx, %ebx
	ARG_LONG_GET read_ecx, %ecx
	ARG_LONG_GET read_edx, %edx

	ARG_LONG_GET read_esp, %esp

	ARG_LONG_SET write_cr4, %cr4
	ARG_LONG_SET write_cr3, %cr3
	ARG_LONG_SET write_cr2, %cr2
	ARG_LONG_SET write_cr1, %cr1
	ARG_LONG_SET write_cr0, %cr0

	ARG_LONG_SET write_eax, %eax
	ARG_LONG_SET write_ebx, %ebx
	ARG_LONG_SET write_ecx, %ecx
	ARG_LONG_SET write_edx, %edx

.text
.globl panic
panic:
1:
	cli
	hlt
	jmp 1b
