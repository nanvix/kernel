/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

#include <nanvix/const.h>
#include <nanvix/klib.h>

/**
 * @brief Dumps the contents of a memory area.
 * 
 * @details Dumps the contents of the memory area pointed to by @p s.
 * 
 * @param s Target memory area.
 * @param n Number of bytes to dump.
 */
PUBLIC void kmemdump(const void *s, size_t n)
{
	const unsigned *p  = s;
	
	/* Dump memory area in chunks. */
	for (size_t i = 0; i < n; i += 16, p += 4)
	{
		/* Do not print zero lines. */
		if (*(p + 0) || *(p + 1) || *(p + 2) || *(p + 3))
		{
			kprintf("[%x]: %x %x %x %x",
				i, *(p + 0), *(p + 1), *(p + 2), *(p + 3));
		}
	}
}
