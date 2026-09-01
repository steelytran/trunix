ENTRY(_start)

SECTIONS
{
 	. = 1M; /* offset */
 	_kernel_start = .;

	.text : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

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
	_bitmap = .;
	. = . + 128K;
	_mem_start = ALIGN(4K);
}
