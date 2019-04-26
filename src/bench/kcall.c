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

#include <nanvix.h>
#include "kbench.h"

/**
 * @brief Benchmarks a remote Kernel Call
 */
void benchmark_kcall_local(void)
{
	uint64_t reg;

	/*
	 * TODO: Query performance monitoring capabilities.
	 */

	for (size_t j = 0; j < ARRAY_LENGTH(perf_events); j++)
	{
		for (int i = SKIP; i < NITERATIONS + SKIP; i++)
		{
			nanvix_perf_start(0, perf_events[j].num);

				syscall0(NR_thread_get_id);

			nanvix_perf_stop(0);
			reg = nanvix_perf_read(0);

			if (i >= SKIP)
			{
				kprintf("[benchmarks][kcall_local] %d %s %d",
					i - SKIP,
					perf_events[j].name,
					UINT32(reg)
				);
			}
		}
	}
}

/**
 * @brief Benchmarks a remote Kernel Call
 */
void benchmark_kcall_remote(void)
{
	uint64_t reg;

	/*
	 * TODO: Query performance monitoring capabilities.
	 */

	for (size_t j = 0; j < ARRAY_LENGTH(perf_events); j++)
	{
		for (int i = SKIP; i < NITERATIONS + SKIP; i++)
		{
			nanvix_perf_start(0, perf_events[j].num);

				syscall0(NR_SYSCALLS);

			nanvix_perf_stop(0);
			reg = nanvix_perf_read(0);

			if (i >= SKIP)
			{
				kprintf("[benchmarks][kcall_remote] %d %s %d",
					i - SKIP,
					perf_events[j].name,
					UINT32(reg)
				);
			}
		}
	}
}
