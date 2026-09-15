ENTRY(__k_unpaged_TRUNIX)

_kernel_physical_base = 0x00100000;
_kernel_virt_base = 0xC0100000;
_kernel_offset = (_kernel_virt_base - _kernel_physical_base);

__k_unpaged__kernel_physical_base = _kernel_physical_base;
__k_unpaged__kernel_virt_base = _kernel_virt_base;
__k_unpaged__kernel_offset = _kernel_offset;

SECTIONS
{
	/*
	 * kernel's physical address is
	 * offset by 4MiB to align with
	 * page directory entires, makes
	 * for easier identity mapping.
	 */
 	. = _kernel_physical_base;
	__k_unpaged__kernel_unpaged_start = .;

	.unpaged_text : { unpaged_*.o(.text) *(.multiboot)}
	.unpaged_data ALIGN(4K) : { unpaged_*.o(.data .rodata*) }
	.unpaged_bss ALIGN(4K) : { unpaged_*.o(.bss COMMON) }

	__k_unpaged__kernel_unpaged_end = .;
	_kernel_unpaged_end =
	    __k_unpaged__kernel_unpaged_end - _kernel_offset;

	. += _kernel_offset;

	.text ALIGN(4K) : AT(ADDR(.text) - _kernel_offset)
	{
		*(.text)
	}

	.rodata ALIGN(4K) : AT(ADDR(.rodata) - _kernel_offset)
	{
		*(.rodata)
	}

	.data ALIGN(4K) : AT(ADDR(.data) - _kernel_offset)
	{
		*(.data)
	}

	.bss ALIGN(4K) : AT(ADDR(.bss) - _kernel_offset)
	{
		*(COMMON)
		*(.bss)
	}
	__k_unpaged__kernel_size = . - _kernel_virt_base;
	_kernel_size = __k_unpaged__kernel_size;
}
