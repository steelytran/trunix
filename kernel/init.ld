ENTRY(main)
SECTIONS
{
	. = 0x400000;

	.text : {
		*(.text)
		*(.text*)
	}

	.rodata : {
		*(.rodata)
		*(.rodata*)
	}

	.bss : {
		*(.bss)
		*(.bss*)
		*(COMMON)
	}
}
