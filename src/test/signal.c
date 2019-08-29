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
	struct ksigaction sigact;

	sigact.handler = dummy_handler;

	KASSERT(ksigctl(SIGPGFAULT, &sigact) == 0);

#if (SIGNAL_DESTRUCTIVE_TEST)

	int b;
	int *a = (int *) 0xdeadbeef;

	/* Page fault (Infinite loop) */
	b = *a;

	UNUSED(b);
#endif

	sigact.handler = NULL;

	KASSERT(ksigctl(SIGPGFAULT, &sigact) == 0);
}

/**
 * @brief API tests.
 */
static struct test signal_tests_api[] = {
	{ test_api_signal_action, "[test][signal][api] signal register/unregister [passed]" },
	{ NULL,                    NULL                                                       },
};

/*============================================================================*
 * Fault Injection Testing Units                                              *
 *============================================================================*/


/**
 * @brief Fault Test: Register and unregister a handler
 *
 * @author João Vicente Souto
 */
void test_fault_signal_action(void)
{
	struct ksigaction sigact;

	sigact.handler = NULL;

	/* Invalid Signal ID */
	KASSERT(ksigctl(-1, &sigact) < 0);
	KASSERT(ksigctl(EXCEPTIONS_NUM, &sigact) < 0);
	KASSERT(ksigctl(EXCEPTIONS_NUM+1, &sigact) < 0);

	/* Invalid sigaction */
	KASSERT(ksigctl(SIGPGFAULT, NULL) < 0);

	sigact.handler = dummy_handler;
	KASSERT(ksigctl(SIGPGFAULT, &sigact) == 0);
	KASSERT(ksigctl(SIGPGFAULT, NULL) < 0);

	sigact.handler = NULL;
	KASSERT(ksigctl(SIGPGFAULT, &sigact) == 0);
	KASSERT(ksigctl(SIGPGFAULT, NULL) < 0);
}

/**
 * @brief API tests.
 */
static struct test signal_tests_fault[] = {
	{ test_fault_signal_action, "[test][signal][fault] signal register/unregister [passed]" },
	{ NULL,                      NULL                                                       },
};


/*============================================================================*/

/**
 * The test_perf() function launches testing units on the performance
 * monitoring interface.
 *
 * @author Pedro Henrique Penna
 */
void test_signal(void)
{

	/* API Tests */
	nanvix_puts("--------------------------------------------------------------------------------");
	for (int i = 0; signal_tests_api[i].test_fn != NULL; i++)
	{
		signal_tests_api[i].test_fn();
		nanvix_puts(signal_tests_api[i].name);
	}

	/* API Tests */
	nanvix_puts("--------------------------------------------------------------------------------");
	for (int i = 0; signal_tests_fault[i].test_fn != NULL; i++)
	{
		signal_tests_fault[i].test_fn();
		nanvix_puts(signal_tests_fault[i].name);
	}
}
