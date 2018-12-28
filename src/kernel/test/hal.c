/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

#if defined(__i386__)

/**
 * @brief Divides one number by another.
 *
 * @param a First operand.
 * @param b Second operand.
 *
 * @returns The fist operand divided by the second one.
 */
PRIVATE unsigned divide(unsigned a, unsigned b)
{
	return (a/b);
}

#endif

/**
 * The hal_test_driver() function runs API, fault injection and
 * stress tests on the HAL.
 *
 * @author Pedro Henrique Penna
 */
PUBLIC void hal_test_driver(void)
{
	kprintf("[hal] running unit tests");

#if defined(__i386__)
	if (0)
	{
		kprintf("[test][hal][api] divide by zero [%d]",
			(divide(16, 0) == (unsigned)-1) ? "passed" : "FAILED"
		);
	}
#endif
}
