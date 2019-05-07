/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2018 Davidson Francis     <davidsondfgl@gmail.com>
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

#include <nanvix/syscall.h>
#include <stdint.h>

/**
 * @todo TODO provide a detailed description for this function.
 */
PUBLIC int nanvix_perf_query(int event)
{
	int ret;

	ret = syscall1(
		NR_perf_query,
		(word_t) event
	);

	/* System call failed. */
	if (ret < 0)
	{
		errno = -ret;
		return (-1);
	}

	return (ret);
}

/**
 * @todo TODO provide a detailed description for this function.
 */
PUBLIC int nanvix_perf_start(int perf, int event)
{
	int ret;

	ret = syscall2(
		NR_perf_start,
		(word_t) perf,
		(word_t) event
	);

	/* System call failed. */
	if (ret < 0)
	{
		errno = -ret;
		return (-1);
	}

	return (ret);
}

/**
 * @todo TODO provide a detailed description for this function.
 */
PUBLIC int nanvix_perf_stop(int perf)
{
	int ret;

	ret = syscall1(
		NR_perf_stop,
		(word_t) perf
	);

	/* System call failed. */
	if (ret < 0)
	{
		errno = -ret;
		return (-1);
	}

	return (ret);
}

/**
 * @todo TODO provide a detailed description for this function.
 */
PUBLIC uint64_t nanvix_perf_read(int perf)
{
	uint32_t ret;

	ret = syscall1(
		NR_perf_read,
		(word_t) perf
	);

	return (ret);
}