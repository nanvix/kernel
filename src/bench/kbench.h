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

#ifndef _KBENCH_H_
#define _KBENCH_H_

	#include <stdint.h>

	/**
	 * @brief Number of benchmark iterations.
	 */
	#define NITERATIONS 30

	/**
	 * @brief Iterations to skip on warmup.
	 */
	#define SKIP 10

	/**
	 * @brief Casts something to a uint32_t.
	 */
	#define UINT32(x) ((uint32_t)((x) & 0xffffffff))

	/**
	 * @brief Alias for kprintf().
	 */
	#define printf(fmt, ...) kprintf(fmt, ##__VA_ARGS__)

	/**
	 * @brief Alias for KASSERT().
	 */
	#undef assert
	#define assert(x) KASSERT(x)

	/**
	 * @brief Performance event.
	 */
	struct perf_event
	{
		const char *name; /**< Event name.   */
		int num;          /**< Event number. */
	};

	/**
	 * @brief Performance events.
	 */
	extern struct perf_event perf_events[8];

#endif /* _KBENCH_H_ */
