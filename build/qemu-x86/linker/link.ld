/*
 * MIT License
 *
 * Copyright(c) 2011-2019 The Maintainers of Nanvix
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

OUTPUT_FORMAT("elf32-i386")
ENTRY(start)

SECTIONS
{
	. = 0x100000;

	KSTART = .;

	/* Bootstrap section. */
	.bootstrap :
	{
		*boot*.o *(.bootstrap)
	}

	. += 0xc0000000;

	/* Kernel code section. */
	.text ALIGN(4096) : AT(ADDR(.text) - 0xc0000000)
   {
       *(.text)
       *(.rodata)
   }

   /* Initialized kernel data section. */
   .data ALIGN(4096) : AT(ADDR(.data) - 0xc0000000)
   {
       *(.data)
   }

   /* Uninitialized kernel data section. */
   .bss : AT(ADDR(.bss) - 0xc0000000)
   {
       *(.bss)
   }

   . =ALIGN(4096);

   KDATA_END = .;

   /* Discarded. */
   /DISCARD/ :
   {
        *(.comment)
        *(.note)
   }
}
