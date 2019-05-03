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
#include <stdint.h>
#include "test.h"

/**
 * @brief Launch verbose tests?
 */
#define TEST_PERF_VERBOSE 1

/*============================================================================*
 * Read Performance Monitor                                                   *
 *============================================================================*/

/**
 * @brief API Test: Read Performance Monitor
 */
void test_nanvix_perf_api_read(void)
{
#if (CORE_HAS_PERF)
	uint64_t cycles;

	/* Query performance monitoring capabilities. */
	test_assert(PERF_EVENTS_MAX >= 1);
	test_assert(nanvix_perf_query(PERF_CYCLES) == 0);

	/* Start performance monitors. */
	test_assert(nanvix_perf_start(0, PERF_CYCLES) == 0);

	/* Stop performance monitors. */
	test_assert(nanvix_perf_stop(0) == 0);

	/* Dump performance information. */
	cycles = nanvix_perf_read(0);

	UNUSED(cycles);

#endif
}
