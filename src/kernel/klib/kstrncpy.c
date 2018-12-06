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

/**
 * @brief Copies part of a string.
 * 
 * @param str1 Target string.
 * @param str2 Source string.
 * @param n    Number of characters to be copied.
 * 
 * @returns A pointer to the target string.
 *
 * @warning This function does not handle cache coherency.
 */
PUBLIC char *kstrncpy(char *str1, const char *str2, size_t n)
{
	char *p1;       /* Indexes str1. */
	const char *p2; /* Indexes str2. */
	
	p1 = str1;
	p2 = str2;
	
	/* Copy string. */
	while (n > 0)
	{
		if (*p2 == '\0')
			break;
			
		*p1++ = *p2++;
		n--;
	}
	
	/* Fill with null bytes. */
	while (n > 0)
	{
		 *p1++ = '\0';
		 n--;
	}
	
	return (str1);
}
