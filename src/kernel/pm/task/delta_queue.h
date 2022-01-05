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

#if __NANVIX_USE_TASKS

#include <nanvix/hlib.h>

#ifndef NANVIX_DELTA_QUEUE_H_
#define NANVIX_DELTA_QUEUE_H_

	/*============================================================================*
	 * resource_delta_enqueue()                                                   *
	 *============================================================================*/

	/**
	 * @brief Enqueue a resource on a delta queue.
	 *
	 * @param queue Delta queue
	 * @param newp  New task 
	 */
	EXTERN int delta_queue_enqueue(
		struct resource_arrangement * arr,
		struct task * p
	);

	/*============================================================================*
	 * resource_delta_remote()                                                    *
	 *============================================================================*/

	/**
	 * @brief Remove a resource on a delta queue.
	 *
	 * @param queue Delta queue
	 * @param newp  Task 
	 */
	EXTERN int delta_queue_remove(
		struct resource_arrangement * arr,
		struct task * p
	);

	/*============================================================================*
	 * delta_queue_dequeue()                                                      *
	 *============================================================================*/

	/**
	 * @brief Try to dequeue a resource from a queue, but it is poped only if its
	 * period reach 0.
	 *
	 * @param queue Delta resource queue
	 *
	 * @returns Valid resource if it reachs 0 on its period. NULL otherwise.
	 */
	EXTERN struct task * delta_queue_dequeue(struct resource_arrangement * arr);

	/*============================================================================*
	 * delta_queue_head_factor()                                                  *
	 *============================================================================*/

	/**
	 * @brief Get the factor of the head.
	 *
	 * @param queue delta resource queue
	 *
	 * @returns Greater or equal to 0 if the queue head is valid, Negative
	 * otherwise.
	 */
	EXTERN int delta_queue_head_factor(struct resource_arrangement * arr);

#endif /* NANVIX_DELTA_QUEUE_H_ */

#endif /* __NANVIX_USE_TASKS */
