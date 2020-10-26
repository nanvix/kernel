/*
 * MIT License
 *
 * Copyright(c) 2011-2020 The Maintainers of Nanvix
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

#include <nanvix/kernel/thread.h>
#include <nanvix/kernel/mm.h>

#include "common.h"

#if CLUSTER_IS_MULTICORE

/**
 * @brief Thread's exit value
 */
EXTENSION PRIVATE struct exit_value
{
	int tid;
	void * retval;
} exit_values[KTHREAD_EXIT_VALUE_NUM] = {
	[0 ... (KTHREAD_EXIT_VALUE_NUM - 1)] = {
		.tid = KTHREAD_NULL_TID,
		.retval = NULL
	}
};

/**
 * @brief Global counter for saving exit values.
 */
PRIVATE int next_exit_value = 0;

/**
 * @brief Saves the retval of the leaving thread in the exit values array.
 *
 * @param retval return value of the target thread
 * @param leaving_thread theard of the retval we're saving
 */
PUBLIC void thread_save_retval(void* retval, struct thread *leaving_thread)
{
	/* Check if retval is a valid pointer*/
	if (retval)
	{
		exit_values[next_exit_value].tid    = thread_get_id(leaving_thread);
		exit_values[next_exit_value].retval = retval;

		next_exit_value = (next_exit_value + 1) % KTHREAD_EXIT_VALUE_NUM;
	}
}

/**
 * @brief Searches the exit_values array for the target return value of
 * a thread.
 *
 * @param retval return value of the target thread
 * @param tid Thread's ID
 */
PUBLIC void thread_search_retval(void **retval, int tid)
{
	/* We need the adress of retval to be different than NULL to use it. */
	if (retval)
	{
		/**
		 * We set the value of retval as NULL in case
		 * we dont find a corresponding TID in the search.
		 */
		*retval = NULL;

		/* Search. */
		for (int i = 0; i < KTHREAD_EXIT_VALUE_NUM; i++)
		{
			/* Found? */
			if (exit_values[i].tid == tid)
			{
				*retval = exit_values[i].retval;
				break;
			}
		}
	}
}

#endif
