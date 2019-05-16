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


#if (THREAD_MAX > 2)

/**
 * @brief Number of threads to spawn.
 */
#define NTHREADS (THREAD_MAX - 1)

/**
 * @brief Number of trials.
 */
#define NTRIALS 1000

/**
 * @brief Mutex for shared variable.
 */
static struct nanvix_mutex mutex;

/**
 * @brief Shared variable.
 */
static volatile int var;

/*============================================================================*
 * API Testing Units                                                          *
 *============================================================================*/

/**
 * @brief Increments the shared variable.
 *
 * @param arg Dummy parameter.
 */
static void *task1(void *arg)
{
	((void) arg);

	/* Increment a variable many times. */
	for (int i = 0; i < NTRIALS; i++)
	{
		nanvix_mutex_lock(&mutex);
			var++;
		nanvix_mutex_unlock(&mutex);
	}

	return (NULL);
}

/**
 * @brief API Test: Sleep/Wakeup
 */
void test_api_sleep_wakeup(void)
{
	kthread_t tids[NTHREADS];

	var = 0;
	nanvix_mutex_init(&mutex);

	/* Spawn threads. */
	for (int i = 0; i < NTHREADS; i++)
		test_assert(kthread_create(&tids[i], task1, NULL) == 0);

	/* Wait for threads. */
	for (int i = 0; i < NTHREADS; i++)
		test_assert(kthread_join(tids[i], NULL) == 0);

	test_assert(var == NTRIALS*NTHREADS);
}

/*============================================================================*
 * Fault Injection Testing Units                                              *
 *============================================================================*/

/**
 * @brief Dummy task.
 *
 * @param arg Dummy parameter.
 */
static void *task2(void *arg)
{
	((void) arg);

	return (NULL);
}

/**
 * @brief Fault Injection Test: Sleep/Wakeup
 */
void test_fault_sleep_wakeup(void)
{
	kthread_t mytid;
	kthread_t tid;

	mytid = kthread_self();

	/* Spawn thread. */
	test_assert(kthread_create(&tid, task2, NULL) == 0);

		/* Wakeup invalid thread. */
		test_assert(wakeup(-1) < 0);

		/* Wakeup bad thread. */
		test_assert(wakeup(mytid) < 0);

	/* Wait for thread. */
	test_assert(kthread_join(tid, NULL) == 0);

	/* Wakeup bad thread. */
	test_assert(wakeup(tid) < 0);
}

/*============================================================================*
 * API Testing Units                                                          *
 *============================================================================*/

/**
 * @brief Increments the shared variable.
 *
 * @param arg Dummy parameter.
 */
static void *task3(void *arg)
{
	((void) arg);

	/* Increment a variable many times. */
	for (int i = 0; i < NTRIALS; i++)
	{
		nanvix_mutex_lock(&mutex);
			var++;
		nanvix_mutex_unlock(&mutex);
	}

	return (NULL);
}

/**
 * @brief Stress Test: Sleep/Wakeup
 */
void test_stress_sleep_wakeup(void)
{
	kthread_t tids[NTHREADS];
	nanvix_mutex_init(&mutex);

	for (int k = 0; k < NITERATIONS; k++)
	{
		var = 0;

		/* Spawn threads. */
		for (int i = 0; i < NTHREADS; i++)
			test_assert(kthread_create(&tids[i], task3, NULL) == 0);

		/* Wait for threads. */
		for (int i = 0; i < NTHREADS; i++)
			test_assert(kthread_join(tids[i], NULL) == 0);

		test_assert(var == NTRIALS*NTHREADS);
	}
}

#endif
