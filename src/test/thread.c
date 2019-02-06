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
#include "test.h"

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

/**
 * @brief Thread manager stress test.
 */
void test_thread(void)
{

	for (int j = 0; j < NITERATIONS; j++)
	{
		kthread_t tid[NTHREADS];

		/* Spawn threads. */
		puts("----> spawning threads...\n");
		for (int i = 1; i < NTHREADS; i++)
			kthread_create(&tid[i], task, NULL);

		/* Wait for threads. */
		puts("---> waiting for threads...\n");
		for (int i = 1; i < NTHREADS; i++)
			kthread_join(tid[i], NULL);
	}
}

