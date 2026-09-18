/*
 * linker script
 * Copyright (C) 2026  spenna
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

ENTRY(__k_unpaged_TRUNIX)

_kernel_physical_base = 0x00100000;
_kernel_virt_base = 0xC0100000;
_kernel_offset = (_kernel_virt_base - _kernel_physical_base);

__k_unpaged__kernel_physical_base = _kernel_physical_base;
__k_unpaged__kernel_virt_base = _kernel_virt_base;
__k_unpaged__kernel_offset = _kernel_offset;

SECTIONS
{
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
