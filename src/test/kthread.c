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
#include "test.h"

/**
 * @name Extra Tests
 */
/**@{*/
#define UTEST_KTHREAD_BAD_START 0 /**< Test bad thread start?    */
#define UTEST_KTHREAD_BAD_ARG   0 /**< Test bad thread argument? */
#define UTEST_KTHREAD_BAD_JOIN  0 /**< Test bad thread join?     */
#define UTEST_KTHREAD_BAD_EXIT  0 /**< Test bad thread exit?     */
#define UTEST_KTHREAD_STRESS    0 /**< Run stress tests?         */
/**@}*/

/**
 * @brief Dummy task.
 *
 * @param arg Unused argument.
 */
static void *task(void *arg)
{
	((void) arg);

	return (NULL);
}

/*============================================================================*
 * API Testing Units                                                          *
 *============================================================================*/

/**
 * @brief API test for thread identification.
 */
static void test_api_kthread_self(void)
{
	test_assert(kthread_self() == 1);
}

/**
 * @brief API test for thread creation/termination.
 */
static void test_api_kthread_create(void)
{
#if (THREAD_MAX > 1)

	kthread_t tid;

	/* Spawn thread. */
	test_assert(kthread_create(&tid, task, NULL) == 0);

	/* Wait for thread. */
	test_assert(kthread_join(tid, NULL) == 0);

#endif
}

/*============================================================================*
 * Fault Testing Units                                                        *
 *============================================================================*/

/**
 * @brief Fault Test: Invalid Thread Create
 */
static void test_fault_kthread_create_invalid(void)
{
#if (THREAD_MAX > 1)

	kthread_t tid;

	/* Invalid start routine. */
	test_assert(kthread_create(&tid, NULL, NULL) < 0);

#endif
}

/**
 * @brief Fault Test: Bad Thread Create
 */
static void test_fault_kthread_create_bad(void)
{
#if (THREAD_MAX > 1)

	kthread_t tid;

	UNUSED(tid);

	/* Bad starting routine. */
#if (defined(UTEST_KTHREAD_BAD_START) && (UTEST_KTHREAD_BAD_START == 1))
	test_assert(kthread_create(&tid, (void *(*)(void *)) KBASE_VIRT, NULL) < 0);
	test_assert(kthread_create(&tid, (void *(*)(void *)) (UBASE_VIRT - PAGE_SIZE), NULL) < 0);
#endif

	/* Bad argument. */
#if (defined(UTEST_KTHREAD_BAD_ARG) && (UTEST_KTHREAD_BAD_ARG == 1))
	test_assert(kthread_create(&tid[0], task, (void *(*)(void *)) KBASE_VIRT) < 0);
	test_assert(kthread_create(&tid[0], task, (void *(*)(void *)) (UBASE_VIRT - PAGE_SIZE)) < 0);
#endif

#endif
}

/**
 * @brief Fault Test: Invalid Thread Join
 */
static void test_fault_kthread_join_invalid(void)
{
#if (THREAD_MAX > 1)

	test_assert(kthread_join(-1, NULL) < 0);
	test_assert(kthread_join(0, NULL) < 0);
	test_assert(kthread_join(1, NULL) < 0);

#endif
}

/**
 * @brief Fault Test: Bad Thread Join
 */
static void test_fault_kthread_join_bad(void)
{
#if (THREAD_MAX > 1)

	kthread_t tid;

	/* Join bad thread. */
	test_assert(kthread_create(&tid, task, NULL) == 0);
	test_assert(kthread_join(2, NULL) < 0);
	test_assert(kthread_join(tid, NULL) == 0);

	/* Join with bad return value. */
#if (defined(UTEST_KTHREAD_BAD_JOIN) && (UTEST_KTHREAD_BAD_JOIN == 1))
	test_assert(kthread_create(&tid, task, NULL) == 0);
	test_assert(kthread_join(tid, (void *)(KBASE_VIRT)) < 0);
	test_assert(kthread_join(tid, (void *)(UBASE_VIRT - PAGE_SIZE)) < 0);
	test_assert(kthread_join(tid, NULL) == 0);
#endif

#endif
}

/*============================================================================*
 * Stress Testing Units                                                       *
 *============================================================================*/

/**
 * @brief Fault Test: Create Too Many Threads
 */
static void test_stress_kthread_create_overflow(void)
{
#if (THREAD_MAX > 1)

	kthread_t tid[NTHREADS + 1];

	for (int i = 0; i < NTHREADS; i++)
		test_assert(kthread_create(&tid[i], task, NULL) == 0);
	test_assert(kthread_create(&tid[NTHREADS], task, NULL) < 0);
	for (int i = 0; i < NTHREADS; i++)
		test_assert(kthread_join(tid[i], NULL) == 0);

#endif
}

/**
 * @brief Stress test for thread creation/termination.
 */
static void test_stress_kthread_create(void)
{
#if (THREAD_MAX > 2)

	for (int j = 0; j < NITERATIONS; j++)
	{
		kthread_t tid[NTHREADS];

		/* Spawn threads. */
		for (int i = 0; i < NTHREADS; i++)
			test_assert(kthread_create(&tid[i], task, NULL) == 0);

		/* Wait for threads. */
		for (int i = 0; i < NTHREADS; i++)
			test_assert(kthread_join(tid[i], NULL) == 0);
	}

#endif
}


/*============================================================================*
 * Test Driver                                                                *
 *============================================================================*/

/**
 * @brief API tests.
 */
static struct test thread_mgmt_tests_api[] = {
	{ test_api_kthread_self,   "[test][thread][api] thread identification       [passed]" },
	{ test_api_kthread_create, "[test][thread][api] thread creation/termination [passed]" },
	{ NULL,                     NULL                                                      },
};

/**
 * @brief Fault tests.
 */
static struct test thread_mgmt_tests_fault[] = {
	{ test_fault_kthread_create_invalid,  "[test][thread][fault] invalid thread create [passed]" },
	{ test_fault_kthread_create_bad,      "[test][thread][fault] bad thread create     [passed]" },
	{ test_fault_kthread_join_invalid,    "[test][thread][fault] invalid thread join   [passed]" },
	{ test_fault_kthread_join_bad,        "[test][thread][fault] bad thread join       [passed]" },
	{ NULL,                                NULL                                                  },
};

/**
 * @brief Stress tests.
 */
static struct test thread_mgmt_tests_stress[] = {
#if (UTEST_KTHREAD_STRESS)
	{ test_fault_kthread_create_overflow, "[test][thread][stress] thread creation overflow    [passed]" },
	{ test_stress_kthread_create,         "[test][thread][stress] thread creation/termination [passed]" },
#endif
	{ NULL,                                NULL                                                         },
};

/**
 * The test_thread_mgmt() function launches testing units on thread manager.
 *
 * @author Pedro Henrique Penna
 */
void test_thread_mgmt(void)
{
	/* API Tests */
	nanvix_puts("--------------------------------------------------------------------------------");
	for (int i = 0; thread_mgmt_tests_api[i].test_fn != NULL; i++)
	{
		thread_mgmt_tests_api[i].test_fn();
		nanvix_puts(thread_mgmt_tests_api[i].name);
	}

	/* Fault Tests */
	nanvix_puts("--------------------------------------------------------------------------------");
	for (int i = 0; thread_mgmt_tests_fault[i].test_fn != NULL; i++)
	{
		thread_mgmt_tests_fault[i].test_fn();
		nanvix_puts(thread_mgmt_tests_fault[i].name);
	}

	/* Stress Tests */
	nanvix_puts("--------------------------------------------------------------------------------");
	for (int i = 0; thread_mgmt_tests_stress[i].test_fn != NULL; i++)
	{
		thread_mgmt_tests_stress[i].test_fn();
		nanvix_puts(thread_mgmt_tests_stress[i].name);
	}
}
