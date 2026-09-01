ENTRY(_start)

SECTIONS
{
	. = 1M; /* offset */

	.text : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	_kernel_start = .;

	.rodata : ALIGN(4K)
	{
		*(.rodata)
	}

	.data : ALIGN(4K)
	{
		*(.data)
	}

	.bss : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

	_kernel_end = .;
}
