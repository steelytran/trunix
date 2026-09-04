ENTRY(_start)

SECTIONS
{
 	. = 1M; /* offset */
 	_kernel_start = .;
	.multiboot.data :
	{
		*(.multiboot.data)
	}
	.multiboot.text :
	{
		*(.multiboot.text)
	}

	. += 0xC0000000;
	.text ALIGN(4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.multiboot)
		*(.text)
	}

	.rodata ALIGN(4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}

	.data ALIGN(4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}

	.bss ALIGN(4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
		*(.boot_stack)
	}
	_kernel_end = .;
}
