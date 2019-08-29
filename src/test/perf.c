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

#if (CORE_HAS_PERF)

/**
 * @brief Launch verbose tests?
 */
#define TEST_PERF_VERBOSE 1

/*============================================================================*
 * Read Performance Monitor                                                   *
 *============================================================================*/

/**
 * @brief API Test: Query Performance Monitoring Capabilities
 */
void test_api_nanvix_perf_query(void)
{
	test_assert(PERF_EVENTS_MAX >= 1);
	test_assert(nanvix_perf_query(PERF_CYCLES) == 0);
}

/**
 * @brief API Test: Start/Stop Performance Monitor
 */
void test_api_nanvix_perf_start_stop(void)
{
	test_assert(nanvix_perf_start(0, PERF_CYCLES) == 0);
	test_assert(nanvix_perf_stop(0) == 0);
}

/**
 * @brief API Test: Read Performance Monitor
 */
void test_api_nanvix_perf_read(void)
{
	/* Read performance monitor. */
	test_assert(nanvix_perf_start(0, PERF_CYCLES) == 0);
	test_assert(nanvix_perf_stop(0) == 0);
	nanvix_perf_read(0);
}

/**
 * @brief API tests.
 */
static struct test perf_tests_api[] = {
	{ test_api_nanvix_perf_query,      "[test][perf][api] query performance monitoring capabilities [passed]\n" },
	{ test_api_nanvix_perf_start_stop, "[test][perf][api] start/stop performance monitor            [passed]\n" },
	{ test_api_nanvix_perf_read,       "[test][perf][api] read performance monitor                  [passed]\n" },
	{ NULL,                             NULL                                                                    },
};

#endif

/*============================================================================*
 * Test Driver                                                                *
 *============================================================================*/

/**
 * The test_perf() function launches testing units on the performance
 * monitoring interface.
 *
 * @author Pedro Henrique Penna
 */
void test_perf(void)
{
#if (CORE_HAS_PERF)

	/* API Tests */
	nanvix_puts("--------------------------------------------------------------------------------");
	for (int i = 0; perf_tests_api[i].test_fn != NULL; i++)
	{
		perf_tests_api[i].test_fn();
		nanvix_puts(perf_tests_api[i].name);
	}

#endif
}
