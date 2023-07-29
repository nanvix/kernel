/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

OUTPUT_FORMAT("elf32-i386")
ENTRY(_do_start)

/*
 * Page Size (4 KB)
 */
PAGE_SIZE = 0x1000;

/*
 * Boot Address
 */
BOOT_ADDR = 0x100000;

SECTIONS
{
	. = BOOT_ADDR;

	/* Bootstrap section. */
	.bootstrap :
	{
		__BOOTSTRAP_START = .;
		*(.bootstrap)
		__BOOTSTRAP_END = .;
	}

	/* Text section. */
	.text : ALIGN(PAGE_SIZE)
	{
		__TEXT_START = .;
		*(.text*)
		__TEXT_END = .;
	}

	/* Initialized data section. */
	.data : ALIGN(PAGE_SIZE)
	{
		__DATA_START = .;
		*(.data*)
		__DATA_END = .;
	}

	/* Uninitialized data section. */
	.bss :
	{
		__BSS_START = .;
		*(.bss*)
		__BSS_END = .;
	}

	/* Read-only data section. */
	.rodata : ALIGN(PAGE_SIZE)
	{
		__RODATA_START = .;
		*(.rodata*)
		__RODATA_END = .;
	}

	/* Discarded. */
	/DISCARD/ :
	{
		*(.comment)
		*(.note)
	}
}
