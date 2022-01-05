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

#include "delta_queue.h"

#if __NANVIX_USE_TASKS

/**
 * @brief Cast a resource pointer to a task pointer.
 */
#define TASK_PTR(t) ((struct task *) t)

/*============================================================================*
 * delta_queue_order()                                                        *
 *============================================================================*/

/**
 * @brief Compare two resource to order them. Specifically, if b has a delta
 * factor that is less than the delta factor of a, we sinalizes that b is
 * smaller and it will be insert before a. Otherwise, we decrement the delta
 * factor of b by the delta factor o @p a.
 *
 * @param a Current delta factor task on arrangement.
 * @param b New task.
 *
 * @return 1 if b is smaller than a, -1 otherwise.
 */
PRIVATE int delta_queue_order(struct resource * a, struct resource * b)
{
	struct task * curr = TASK_PTR(a);
	struct task * newt = TASK_PTR(b);

	if (newt->delta_factor < curr->delta_factor)
		return (1);

	newt->delta_factor -= curr->delta_factor;

	return (-1);
}

/*============================================================================*
 * delta_queue_enqueue()                                                      *
 *============================================================================*/

/**
 * @brief Enqueue a resource on a delta queue.
 *
 * @param arr Delta queue
 * @param t   New task
 */
PUBLIC int delta_queue_enqueue(struct resource_arrangement * arr, struct task * t)
{
	/* Sanity checks. */
	KASSERT(arr && t);

	/* Insert in order based on delta factor. */
	KASSERT(resource_insert_ordered(arr, &t->resource, delta_queue_order) >= 0);

	/**
	 * Update next delta factor.
	 *
	 * @details We need to do this because the next task needs to have its delta
	 * factor based on the new task inserted.
	 */
	if (t->resource.next)
		TASK_PTR(t->resource.next)->delta_factor -= t->delta_factor;

	return (0);
}

/*============================================================================*
 * delta_queue_remove()                                                       *
 *============================================================================*/

/**
 * @brief Remove a resource from a delta queue.
 *
 * @param arr Delta queue
 * @param t   Task
 */
PUBLIC int delta_queue_remove(struct resource_arrangement * arr, struct task * t)
{
	/* Sanity checks. */
	KASSERT(arr || t);

	/* Not found? */
	if (resource_search(arr, &t->resource) < 0)
		return (-EINVAL);

	/**
	 * Update next delta fector.
	 *
	 * @details We must add the delta factor that it will be remove to the next
	 * task.
	 */
	if (t->resource.next)
		TASK_PTR(t->resource.next)->delta_factor += t->delta_factor;

	/* Remove it. */
	return (resource_pop(arr, &t->resource));
}

/*============================================================================*
 * delta_queue_tick()                                                         *
 *============================================================================*/

/**
 * @brief Try to dequeue a resource from a queue, but it is poped only if its
 * delta factor reach 0.
 *
 * @param arr Delta resource queue
 *
 * @returns Valid task if it reachs 0 on its delta factor. NULL otherwise.
 */
PUBLIC struct task * delta_queue_dequeue(struct resource_arrangement * arr)
{
	struct task * head;

	KASSERT(arr != NULL);

	/* Empty queue. */
	if ((head = TASK_PTR(arr->head)) == NULL)
		return (NULL);

	/* The head delta factor is over. */
	if (head->delta_factor == 0)
	{
		KASSERT(resource_dequeue(arr) == &head->resource);
		head->state = TASK_STATE_NOT_STARTED;

		return (head);
	}

	/* Decrement head delta factor. */
	head->delta_factor--;

	/* The head delta factor is not over. */
	return (NULL);
}

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
PUBLIC int delta_queue_head_factor(struct resource_arrangement * arr)
{
	KASSERT(arr != NULL);
	return ((arr->head != NULL) ? (TASK_PTR(arr->head)->delta_factor) : (-1));
}

#endif /* __NANVIX_USE_TASKS */
