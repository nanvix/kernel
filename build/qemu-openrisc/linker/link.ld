/*
 * MIT License
 *
 * Copyright(c) 2011-2020 The Maintainers of Nanvix
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

OUTPUT_FORMAT("elf32-or1k")
ENTRY(_do_start)

BOOT_ADDR = 0;

SECTIONS
{
	. = BOOT_ADDR;

	EXCEPTIONS = .;

	KERNEL_CODE_START = .;

	/* Kernel code section. */
	.bootstrap : AT(ADDR(.bootstrap))
	{
		__BOOTSTRAP_START = .;
		*hooks*.o
		*boot_code*.o
		__BOOTSTRAP_END = .;
	}

	. = ALIGN(8192);

	/* Kernel code section. */
	.text : AT(ADDR(.text))
	{
		__TEXT_START = .;
		*(.text)
		__TEXT_END = .;
	}

	KERNEL_CODE_END = .;

	. = ALIGN(8192);

	KERNEL_DATA_START = .;

	/* Initialized kernel data section. */
	.data ALIGN(8192) : AT(ADDR(.data))
	{
		__DATA_START = .;
		*(.rodata)
		*(.data)
		__DATA_END = .;
	}

	/* Uninitialized kernel data section. */
	.bss : AT(ADDR(.bss))
	{
		__BSS_START = .;
		*boot_data*.o
		*(.bss)
		__BSS_END = .;
	}

	KERNEL_DATA_END = .;

	. =ALIGN(8192);

	/* Discarded. */
	/DISCARD/ :
	{
		*(.comment)
		*(.note)
	}
}
