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

#include <nanvix/mm.h>
#include <nanvix/thread.h>
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
 * @brief Shared variable.
 */
static volatile int var;

/*============================================================================*
 * Mutex                                                                      *
 *============================================================================*/

/**
 * @brief Mutex.
 */
struct
{
	bool locked;              /**< Locked?           */
	spinlock_t lock;          /**< Lock.             */
	kthread_t tids[NTHREADS]; /**< Sleeping threads. */
} mutex;

/**
 * @brief Initializes the mutex.
 */
static void mutex_init(void)
{
	mutex.locked = false;
	mutex.lock = SPINLOCK_UNLOCKED;
	for (int i = 0; i < NTHREADS; i++)
		mutex.tids[i] = -1;
}

/**
 * @brief Locks the mutex.
 */
static void mutex_lock(void)
{
	kthread_t tid;

	tid = kthread_self();

	do
	{
		spinlock_lock(&mutex.lock);

			for (int i = 0; i < NTHREADS; i++)
			{
				if (mutex.tids[i] == tid)
				{
					for (int j = i; j < (NTHREADS - 1); j++)
						mutex.tids[j] = mutex.tids[j + 1];
					mutex.tids[NTHREADS - 1] = -1;
					break;
				}
			}

			/* Lock. */
			if (!mutex.locked)
			{
				mutex.locked = true;
				spinlock_unlock(&mutex.lock);
				break;
			}

			for (int i = 0; i < NTHREADS; i++)
			{
				if (mutex.tids[i] == -1)
				{
					mutex.tids[i] = tid;
					break;
				}
			}

		spinlock_unlock(&mutex.lock);

		test_assert(sleep() == 0);
	} while (true);
}

/**
 * @brief unlocks the mutex.
 */
static void mutex_unlock(void)
{
again:
	spinlock_lock(&mutex.lock);

		if (mutex.tids[0] != -1)
		{
			if (wakeup(mutex.tids[0]) != 0)
			{
				spinlock_unlock(&mutex.lock);
				goto again;
			}
		}

		mutex.locked = false;

	spinlock_unlock(&mutex.lock);
}

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
		mutex_lock();
			var++;
		mutex_unlock();
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
	mutex_init();

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
		mutex_lock();
			var++;
		mutex_unlock();
	}

	return (NULL);
}

/**
 * @brief Stress Test: Sleep/Wakeup
 */
void test_stress_sleep_wakeup(void)
{
	kthread_t tids[NTHREADS];
	mutex_init();

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
