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

#include <stddef.h>

/**
 * The strcmp() function returns an integer greater than, equal to, or
 * less than zero, accordingly as the string pointed to by @p s1 is
 * greater than, equal to, or less than the string pointed to by @p
 * s2.
 */
int strncmp(const char *restrict s1, const char *restrict s2, size_t n)
{
	/* Compare strings. */
	while (n > 0)
	{
		/* Strings differ. */
		if (*s1 != *s2)
			return ((*(unsigned char *) s1 - *(unsigned char *) s2));

		/* End of string. */
		else if (*s1 == '\0')
			return (0);

		s1++;
		s2++;
		n--;
	}

	return (0);
}
