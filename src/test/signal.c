/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis     <davidsondfgl@gmail.com>
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
#include "test.h"

/**
 * @cond release_test
 */

/**
 * @brief Enable destructive testing.
 */
#define SIGNAL_DESTRUCTIVE_TEST 0

/*============================================================================*
 * Signal Unit Tests                                                          *
 *============================================================================*/

/**
 * @brief Auxiliar handler
 *
 * @author João Vicente Souto
 */
void dummy_handler(void * arg)
{
	dword_t signum = *((dword_t *) arg);

	KASSERT(signum == SIGPGFAULT);
}

/**
 * @brief API Test: Register and unregister a handler
 *
 * @author João Vicente Souto
 */
void test_api_signal_action(void)
{
	struct sigaction sigact;

	sigact.handler = dummy_handler;

	KASSERT(ksigclt(SIGPGFAULT, &sigact) == 0);

#if (SIGNAL_DESTRUCTIVE_TEST)

	int b;
	int *a = (int *) 0xdeadbeef;

	/* Page fault (Infinite loop) */
	b = *a;

	UNUSED(b);
#endif

	sigact.handler = NULL;

	KASSERT(ksigclt(SIGPGFAULT, &sigact) == 0);
}

/**
 * @brief Fault Test: Register and unregister a handler
 *
 * @author João Vicente Souto
 */
void test_fault_signal_action(void)
{
	struct sigaction sigact;

	sigact.handler = NULL;

	/* Invalid Signal ID */
	KASSERT(ksigclt(-1, &sigact) < 0);
	KASSERT(ksigclt(EXCEPTIONS_NUM, &sigact) < 0);
	KASSERT(ksigclt(EXCEPTIONS_NUM+1, &sigact) < 0);

	/* Invalid sigaction */
	KASSERT(ksigclt(SIGPGFAULT, NULL) < 0);

	sigact.handler = dummy_handler;
	KASSERT(ksigclt(SIGPGFAULT, &sigact) == 0);
	KASSERT(ksigclt(SIGPGFAULT, NULL) < 0);

	sigact.handler = NULL;
	KASSERT(ksigclt(SIGPGFAULT, &sigact) == 0);
	KASSERT(ksigclt(SIGPGFAULT, NULL) < 0);
}

/*============================================================================*/

/**@endcond*/
