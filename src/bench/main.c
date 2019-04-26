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

#include <nanvix.h>
#include "kbench.h"

/**
 * Performance events.
 */
struct perf_event perf_events[] = {
	{ "cycles",        PERF_CYCLES        },
	{ "branches",      PERF_BRANCH_TAKEN  },
	{ "branch_stalls", PERF_BRANCH_STALLS },
	{ "reg_stalls",    PERF_REG_STALLS    },
	{ "dcache_stalls", PERF_DCACHE_STALLS },
	{ "icache_stalls", PERF_ICACHE_STALLS },
	{ "dtlb_stalls",   PERF_DTLB_STALLS   },
	{ "itlb_stalls",   PERF_ITLB_STALLS   },
};

/**
 * @brief Lunches user-land testing units.
 *
 * @param argc Argument counter.
 * @param argv Argument variables.
 */
int main(int argc, const char *argv[])
{
	((void) argc);
	((void) argv);

	/* Shutdown. */
	shutdown();

	return (0);
}
