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

#include <nanvix/hal/hal.h>
#include <nanvix/kernel/mm.h>
#include <posix/errno.h>

/**
 * Gets performance statistics of the kernel. Retrived statistics are
 * stored into the userspace buffer pointed to by @p buffer. If @p
 * buffer is a null pointer, then performance counters are started to
 * watch the performance event @p perf.
 */
PUBLIC int kernel_stats(uint64_t *buffer, int perf)
{
	int ret = 0;

	/* Start counter. */
	if (buffer == NULL)
	{
		if (perf < 0)
			return (-EINVAL);

		ret = perf_start(1, perf);
	}

	/* Stop counter. */
	else
	{
		/* Bad buffer. */
		if (!mm_check_area(VADDR(buffer), sizeof(uint64_t), UMEM_AREA))
			return (-EFAULT);

		/* Save statistic. */
		if ((ret = perf_stop(1)) == 0)
			*buffer = perf_read(1);
	}

	return (ret);
}
