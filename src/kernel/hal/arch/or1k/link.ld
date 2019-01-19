/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2017-2018 Davidson Francis     <davidsondfgl@gmail.com>
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
ENTRY(start)

SECTIONS
{
	EXCEPTIONS = .;

	KSTART_CODE = .;

	/* Bootstrap section. */
	.bootstrap :
	{
		hal/arch/or1k/hooks.o
		hal/arch/or1k/boot_code.o
	}

	/* Kernel code section. */
	.text ALIGN(8192) : AT(ADDR(.text))
	{
		*(.text)
		*(.rodata)
	}

	KSTART_DATA = ALIGN(8192);

	/* Initialized kernel data section. */
	.data ALIGN(8192) : AT(ADDR(.data))
	{
		hal/arch/or1k/boot_data.o
		*(.data)
	}

	/* Uninitialized kernel data section. */
	.bss : AT(ADDR(.bss))
	{
		*(.bss)
	}

	. =ALIGN(8192);

	KDATA_END = .;

	/* Discarded. */
	/DISCARD/ :
	{
		*(.comment)
		*(.note)
	}
}
