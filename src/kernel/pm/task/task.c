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

/* Must come first. */
#define __NEED_SECTION_GUARD

#include "periodic_queue.h"

#if __NANVIX_USE_TASKS

#include <nanvix/kernel/event.h>

/*============================================================================*
 * Task system variables                                                      *
 *============================================================================*/

/**
 * @brief Helpers
 */
/**@{*/
#define TASK_PTR(x) ((struct task *) (x)) /**< Cast to a task. */
/**@}*/

/**
 * @brief Task board.
 */
PRIVATE struct task_board
{
	/**
	 * @name Control.
	 */
	/**@{*/
	int color;                                        /**< Current color.         */
	int counter;                                      /**< New task ID control.   */
	int shutdown;                                     /**< Request shutdown.      */
	spinlock_t lock;                                  /**< Board protection.      */
	struct semaphore sem;                             /**< Actives tasks control. */
	/**@}*/

	/**
	 * @name Current management.
	 */
	/**@{*/
	bool exit;                                        /**< Exit call performed.   */
	int retval;                                       /**< Return value.          */
	byte_t management;                                /**< Management trigger.    */
	struct task * ctask;                              /**< Current task.          */
	task_merge_args_fn merge;                         /**< Merge args function.   */
	word_t exit_args[TASK_ARGS_NUM];                  /**< Exit arguments.        */
	/**@}*/

	/**
	 * @name Tracked Tasks.
	 */
	/**@{*/
	struct resource_arrangement actives;              /**< Ready tasks.           */
	struct resource_arrangement waiting;              /**< Blocked tasks.         */
	struct resource_arrangement periodics;            /**< Periodic tasks.        */
	struct resource_arrangement emissions[CORES_NUM]; /**< Emitted tasks.         */
	/**@}*/
} taskboard ALIGN(CACHE_LINE_SIZE);

/*============================================================================*
 * Management                                                                 *
 *============================================================================*/

/*============================================================================*
 * task_is_invalid()                                                          *
 *============================================================================*/

/**
 * @brief Verify if a task is invalid.
 *
 * @param task Task pointer.
 *
 * @return True if task is invalid, false otherwise.
 */
PRIVATE inline bool task_is_invalid(struct task * task)
{
	return (
		task == NULL
		|| !WITHIN(task->id, 0, taskboard.counter)
		|| task->state == TASK_STATE_INVALID
	);
}

/*============================================================================*
 * __task_remove()                                                            *
 *============================================================================*/

/**
 * @brief Move a task from one state to another.
 *
 * @param task Task pointer.
 *
 * @return Zero if successfully move the task, non-zero otherwise.
 */
PRIVATE inline int __task_remove(struct task * task)
{
	/* Based on task state: */
	switch (task->state)
	{
		/* Remove from the active queue. */
		case TASK_STATE_READY:
			return (resource_pop(&taskboard.actives, &task->resource));

		/* Remove from the waiting queue. */
		case TASK_STATE_STOPPED:
			return (resource_pop(&taskboard.waiting, &task->resource));

		/* Remove from the periodic queue. */
		case TASK_STATE_PERIODIC:
			return (periodic_queue_remove(&taskboard.periodics, task));

		/* It is not in any queue. */
		case TASK_STATE_NOT_STARTED:
		case TASK_STATE_RUNNING:
		case TASK_STATE_COMPLETED:
		case TASK_STATE_ERROR:
			return (0);

		/**
		 * Otherwise, it is an error.
		 * Tasks don't leave these states internally.
		 */
		case TASK_STATE_INVALID:
		default:
			return (-EBADF);
	}
}

/*============================================================================*
 * __task_insert()                                                            *
 *============================================================================*/

/**
 * @brief Move a task from one state to another.
 *
 * @param task      Task pointer.
 * @param new_state New state value.
 *
 * @return Zero if successfully move the task, non-zero otherwise.
 */
PRIVATE inline int __task_insert(struct task * task, int new_state)
{
	/* Based on task state: */
	switch (new_state)
	{
		/* Insert in the active queue and notify the Dispatcher. */
		case TASK_STATE_READY:
		{
			if (task->priority == TASK_PRIORITY_HIGH)
				resource_push_front(&taskboard.actives, &task->resource);
			else
				resource_push_back(&taskboard.actives, &task->resource);

			semaphore_up(&taskboard.sem);
		} break;

		/* Insert in the waiting queue. */
		case TASK_STATE_STOPPED:
			resource_push_back(&taskboard.waiting, &task->resource);
			break;

		/* Insert in the periodic queue. */
		case TASK_STATE_PERIODIC:
			task->delta_factor = task->period;
			periodic_queue_enqueue(&taskboard.periodics, task);
			break;

		/* We don't put in any queue. */
		case TASK_STATE_COMPLETED:
		case TASK_STATE_ERROR:
		case TASK_STATE_INVALID:
			break;

		/* Otherwise, it is an error. */
		case TASK_STATE_NOT_STARTED:
			/* task_create sets Not Started manually. */
		case TASK_STATE_RUNNING:
			/* Dispatcher sets Running manually. */
		default:
			return (-EBADF);
	}

	/* Sets new state. */
	task->state = new_state;

	/* Restore schedule type. */
	task->schedule_type = task->period > 0 ?
		TASK_SCHEDULE_PERIODIC : TASK_SCHEDULE_READY;

	return (0);
}

/*============================================================================*
 * __task_move()                                                              *
 *============================================================================*/

/**
 * @brief Move a task from one state to another.
 *
 * @param task      Task pointer.
 * @param new_state New state value.
 *
 * @return Zero if successfully move the task, non-zero otherwise.
 */
PRIVATE int __task_move(struct task * task, int new_state)
{
	int ret; /* Return value. */

	/* Task is already on the next state. */
	if (task->state == new_state)
		return (0);

	/* Remove the task from a queue, or not, based on its state. */
	if ((ret = __task_remove(task)) < 0)
		return (ret);

	/* Insert thet task in a queue, or not, based on the next state. */
	if (__task_insert(task, new_state) < 0)
		kpanic("[kernel][task] Inconsistency on move a task (%d -> %d).", task->state, new_state);

	/* Success. */
	return (0);
}

/*============================================================================*
 * __task_dispatch()                                                          *
 *============================================================================*/

/**
 * @brief Enqueue a task in ready queues.
 *
 * @param task Task pointer.
 *
 * @return Zero if successfully dispatch the task, non-zero otherwise.
 */
PRIVATE inline int __task_dispatch(struct task * task)
{
	return (__task_move(task, TASK_STATE_READY));
}

/*============================================================================*
 * __task_schedule()                                                          *
 *============================================================================*/

/**
 * @brief Enqueue a task in one of the three queues.
 *
 * @param task Task pointer.
 *
 * @return Zero if successfully schedule the task, non-zero otherwise.
 */
PRIVATE inline int __task_schedule(struct task * task)
{
	return (__task_move(task, task->schedule_type));
}

/*============================================================================*
 * __task_disconnect()                                                        *
 *============================================================================*/

/**
 * @brief Disconnect tasks.
 *
 * @param parent Task pointer.
 * @param child  Task pointer.
 *
 * @returns Zero if success disconnect two tasks, and negative number they are not
 * connected.
 */
PRIVATE int __task_disconnect(struct task * parent, struct task * child)
{
	int i;              /* Iterator.   */
	int j;              /* Iterator.   */
	word_t mask;        /* Mask.       */
	bool is_dependency; /* flow type.  */
	bool match_flow;    /* Match flow. */

	KASSERT(parent != NULL && child != NULL);

	/* Does the parent have no children or does the child have no parents? */
	if (UNLIKELY(parent->nchildren <= 0 || child->rparents <= 0))
		return (-EINVAL);

	/* Find connection. */
	for (i = 0; i < parent->nchildren; ++i)
	{
		/* Ignore other children. */
		if (parent->children[i].child != child)
			continue;

		/**
		 * Decrease counters.
		 */
		parent->nchildren--;
		child->rparents--;
		if (((signed char) child->nparents) > 0)
			child->nparents--;

		/**
		 * Resets parent bit.
		 */
		is_dependency = (parent->children[i].is_dependency != 0);

		/* Find j position related to parent type. */
		for (j = child->rparents; j >= 0; --j)
		{
			mask       = (1 << j);
			match_flow = ((child->parent_types & mask) != 0);

			/* Are the bits from different types. */
			if (is_dependency != match_flow)
				continue;

			break;
		}

		KASSERT(j >= 0);

		/* Resets j position. */
		child->parent_types &= ~mask;

		/* Moves n bit to j position. */
		if (j < child->rparents)
		{
			mask = child->parent_types & (1 << child->rparents);    //! Gets value of the bit n
			child->parent_types |= (mask >> (child->rparents - j)); //! Set bit j with bit n value
			child->parent_types &= ~(1 << child->rparents);         //! Set 0 to bit n;
		}

		/**
		 * Invalidated node.
		 */
		parent->children[i] = TASK_NODE_INVALID;

		/* Break loop with current position stored on "i". */
		break;
	}

	/* Move next children back one slot. */
	for (; i < parent->nchildren; ++i)
		parent->children[i] = parent->children[i + 1];

	/* Success. */
	return (0);
}

/*============================================================================*
 * __task_notify()                                                            *
 *============================================================================*/

/**
 * @brief Complete a task
 *
 * @param task         Task pointer.
 * @param new_state    New state.
 * @param mgnt_trigger Management trigger.
 *
 * @return Zero if successfully complete the task, non-zero otherwise.
 */
PRIVATE int __task_notify(struct task * task, byte_t new_state, byte_t mgnt_trigger)
{
	int ret;             /* Return value.                    */
	bool is_dependency;  /* Connection's type.               */
	bool is_temporary;   /* Connection's lifetime.           */
	byte_t triggers;     /* Connection's trigger conditions. */
	struct task * child; /* Child task pointer.              */

	/* Moves the task to new state. */
	if (UNLIKELY((ret = __task_move(task, new_state)) < 0))
		return (ret);

	/* Sinalizes children that the parent are completed. */
	for (int i = 0; i < task->nchildren; ++i)
	{
		is_dependency = task->children[i].is_dependency;
		is_temporary  = task->children[i].is_temporary;
		triggers      = task->children[i].triggers;
		child         = task->children[i].child;

		KASSERT(child != NULL && child->rparents > 0 && task->children[i].is_valid);

		/**
		 * Is the management trigger in the current trigger?
		 * Then nofities child.
		 */
		if (mgnt_trigger & triggers)
		{
			/* Propagates the return value. */
			child->retval = task->retval;

			/* Must we configure the child's arguments? */
			if (taskboard.merge != NULL)
				taskboard.merge(taskboard.exit_args, child->args);

			/* Indicates that parent finished. */
			if (!is_temporary)
			{
				if (child->nparents > 0)
					child->nparents--;
				else
					kprintf("[kernel][task] Warning: Notification number > nparents (on notif)!");
			}

			/* Indicates that parent finished and removes soft dependency. */
			else if (UNLIKELY((ret = __task_disconnect(task, child)) < 0))
				return (ret);

			/**
			 * Are we in a flow connection and/or the child does not have
			 * dependencies.
			 */
			if ((!is_dependency && child->parent_types == 0) || (child->nparents == 0))
			{
				/* Reschedule the child based on its schedule type. */
				if (UNLIKELY((ret = __task_schedule(child)) < 0))
					return (ret);
			}
		}
	}

	/**
	 * Release the semaphore only with the management triggered is set on
	 * the task releases.
	 */
	if (mgnt_trigger & task->releases)
		semaphore_up(&task->sem);

	/* Success. */
	return (0);
}

/*============================================================================*
 * __task_error()                                                             *
 *============================================================================*/

/**
 * @brief Release task because an error occured.
 *
 * @param task Task pointer.
 *
 * @return Zero if successfully propagate the task, non-zero otherwise.
 */
PRIVATE int __task_error(struct task * task)
{
	int ret;             /* Return value.                    */
	bool is_dependency;  /* Connection's type.               */
	bool is_temporary;   /* Connection's lifetime.           */
	byte_t triggers;     /* Connection's trigger conditions. */
	struct task * child; /* Child task pointer.              */

	/* Moves the task to ERROR state. */
	if (UNLIKELY((ret = __task_move(task, TASK_STATE_ERROR)) < 0))
		return (ret);

	/* Propagates the error to children. */
	for (int i = 0; i < task->nchildren; ++i)
	{
		is_dependency = task->children[i].is_dependency;
		is_temporary  = task->children[i].is_temporary;
		triggers      = task->children[i].triggers;
		child         = task->children[i].child;

		KASSERT(child != NULL && child->rparents > 0 && task->children[i].is_valid);

		/**
		 * TODO: Should we propagate the error to soft dependencies?
		 */

		/* Does the current triggers alow notification of the error? */
		if (triggers & (TASK_TRIGGER_ERROR_THROW | TASK_TRIGGER_ERROR_CATCH))
		{
			/* Ignore already colored nodes (current color). */
			if (child->color == taskboard.color)
				continue;

			/* Propagates the error. */
			child->retval = task->retval;
			child->color  = taskboard.color;

			/* Must we configure the child's arguments? */
			if (taskboard.merge != NULL)
				taskboard.merge(taskboard.exit_args, child->args);

			/* Indicates that parent finished. */
			if (!is_temporary)
			{
				if (child->nparents > 0)
					child->nparents--;
				else
					kprintf("[kernel][task] Warning: Notification number > nparents (on error)!");
			}

			/* Indicates that parent finished and removes soft dependency. */
			else if (UNLIKELY((ret = __task_disconnect(task, child)) < 0))
				return (ret);
/**
 * We don't reset the child state if we redispatch a flow, so maybe we must
 * retrigger an error condition.
 */
#if 0
			/* Is the child already in an error state? */
			if (child->state == TASK_STATE_ERROR)
				continue;
#endif

			/* Throw error, propagation. */
			if (triggers & TASK_TRIGGER_ERROR_THROW)
			{
				/* Go to down (deep-first). */
				if (UNLIKELY((ret = __task_error(child)) < 0))
					return (ret);
			}

			/* Catch, only notifies the child. */
			else
			{
				/**
				 * Are we in a flow connection and/or the child does not have
				 * dependencies.
				 */
				if ((!is_dependency && child->parent_types == 0) || (child->nparents == 0))
				{
					/* Reschedule the child based on its schedule type. */
					if (UNLIKELY((ret = __task_schedule(child)) < 0))
						return (ret);
				}
			}
		}
	}

	/* TODO: Do we need to release the task before or after the children? */
	if (task->releases & TASK_MANAGEMENT_ERROR)
		semaphore_up(&task->sem);

	/* Success. */
	return (0);
}

/*============================================================================*
 * __task_again()                                                             *
 *============================================================================*/

/**
 * @brief Remove a task and its children because a error.
 *
 * @param task Task pointer.
 *
 * @return Zero if successfully reschedule the task, non-zero otherwise.
 */
PRIVATE inline int __task_again(struct task * task)
{
	return (__task_notify(task, TASK_STATE_READY, TASK_MANAGEMENT_AGAIN));
}

/*============================================================================*
 * __task_stop()                                                              *
 *============================================================================*/

/**
 * @brief Remove a task and its children because a error.
 *
 * @param task Task pointer.
 *
 * @return Zero if successfully stop the task, non-zero otherwise.
 */
PRIVATE inline int __task_stop(struct task * task)
{
	return (__task_notify(task, TASK_STATE_STOPPED, TASK_MANAGEMENT_STOP));
}

/*============================================================================*
 * __task_periodic()                                                          *
 *============================================================================*/

/**
 * @brief Remove a task and its children because a error.
 *
 * @param task Task pointer.
 *
 * @return Zero if successfully program a scheduling to the task, non-zero
 * otherwise.
 */
PRIVATE inline int __task_periodic(struct task * task)
{
	return (__task_notify(task, TASK_STATE_PERIODIC, TASK_MANAGEMENT_PERIODIC));
}

/*============================================================================*
 * __task_emit()                                                              *
 *============================================================================*/

/**
 * @brief Emit a task to the target core operate.
 *
 * @param task   Task pointer.
 * @param coreid Core ID.
 *
 * @returns Zero if successfully register the task, non-zero otherwise.
 */
PRIVATE int __task_emit(struct task * task, int coreid)
{
	KASSERT(task->id != TASK_NULL_ID);

#if 0
	/* Must we verify if the task is out of the task system? */
	KASSERT(task->state == (READY | COMPLETE))
#endif

	/* Is a periodic task? */
	if (UNLIKELY(task->period > 0))
		return (-EBADF);

	/* Does the task have parents or children? */
	if (UNLIKELY(task->rparents != 0 || task->nchildren != 0))
		return (-EBADF);

	/* Change task state to ready. */
	task->state = TASK_STATE_READY;

	/* Active task. */
	KASSERT(resource_enqueue(&taskboard.emissions[coreid], &task->resource) == 0);

	/* Success. */
	return (0);
}

/*============================================================================*
 * Dispatcher                                                                 *
 *============================================================================*/

/*============================================================================*
 * task_loop()                                                                *
 *============================================================================*/

PUBLIC void task_loop(void)
{
	int ret;             /* Return value.            */
	int intlvl;          /* Previous interrup level. */
	struct task * ctask; /* Current task.            */

	kprintf("[kernel][task][dispatcher] Working on core %d!", core_get_id());
	interrupts_enable();

	/* We do not want to be interrupted in the critical region. */
	intlvl = interrupts_set_level(INTERRUPT_LEVEL_NONE);
	spinlock_lock(&taskboard.lock);

	/* Endless loop. */
	while (LIKELY(!taskboard.shutdown))
	{
		/* Waits for tasks. */
		spinlock_unlock(&taskboard.lock);
			semaphore_down(&taskboard.sem);
		spinlock_lock(&taskboard.lock);

			/* Consumes task. */
			ctask           = TASK_PTR(resource_dequeue(&taskboard.actives));
			ctask->state    = TASK_STATE_RUNNING;

			/* Reload task's informations. */
			ctask->nparents = ctask->rparents;

			/* Sets default management variables. */
			taskboard.ctask      = ctask;
			taskboard.management = TASK_MANAGEMENT_USER0;
			taskboard.exit       = false;

		spinlock_unlock(&taskboard.lock);
		interrupts_set_level(intlvl);

		/* Valid function. */
		if (UNLIKELY(ctask->fn == NULL))
			kpanic("[kernel][task] Task without a function.");

		/* Executes the function of the task. */
		ctask->retval = ctask->fn(
			ctask->args[0],
			ctask->args[1],
			ctask->args[2],
			ctask->args[3],
			ctask->args[4]
		);

		intlvl = interrupts_set_level(INTERRUPT_LEVEL_NONE);
		spinlock_lock(&taskboard.lock);

			ret = (-EINVAL);

			if (taskboard.exit)
				ctask->retval = taskboard.retval;

			/* Evaluates the management operation. */
			switch (taskboard.management)
			{
				/* Reschedules the task. */
				case TASK_MANAGEMENT_AGAIN:
					ret = __task_again(ctask);
					break;

				/* Blocks the task. */
				case TASK_MANAGEMENT_STOP:
					ret = __task_stop(ctask);
					break;

				/* Program the reschedule of the task. */
				case TASK_MANAGEMENT_PERIODIC:
					ret = __task_periodic(ctask);
					break;

				/* An error occured. */
				case TASK_MANAGEMENT_ERROR:
					ret = __task_error(ctask);
					taskboard.color++;
					break;

				/* Completes the task. */
				default:
				{
					/* Is it a combination of user triggers? */
					if (LIKELY(WITHIN(taskboard.management, TASK_MANAGEMENT_USER0, TASK_MANAGEMENT_AGAIN)))
						ret = __task_notify(ctask, TASK_STATE_COMPLETED, taskboard.management);
				} break;
			}

			/* Error detected. */
			if (UNLIKELY(ret != 0))
				kpanic("[kernel][task] Inconsistency on task management (%d).", taskboard.management);

			/* Reset merge function. */
			if (taskboard.exit)
			{
				taskboard.merge  = NULL;
				taskboard.retval = 0;
				for (int i = 0; i < TASK_ARGS_NUM; ++i)
					taskboard.exit_args[i] = 0;
			}

			/* Reset current task. */
			taskboard.ctask = NULL;
	}

	interrupts_set_level(intlvl);
	spinlock_unlock(&taskboard.lock);
}

/*============================================================================*
 * Interface exported                                                         *
 *============================================================================*/

/*============================================================================*
 * task_current()                                                             *
 *============================================================================*/

/**
 * @brief Get current task.
 *
 * @returns Current thread running. NULL if the dispatcher is not executing
 * a task.
 */
PUBLIC struct task * task_current(void)
{
	struct task * curr;         /* Current task pointer. */
	struct section_guard guard; /* Section guard.        */

	/* Prevent this call be preempted by any maskable interrupt. */
	section_guard_init(&guard, &taskboard.lock, INTERRUPT_LEVEL_NONE);

	/* Consult the current thread. */
	section_guard_entry(&guard);
		curr = taskboard.ctask;
	section_guard_exit(&guard);

	return (curr);
}

/*============================================================================*
 * task_exit()                                                                *
 *============================================================================*/

/**
 * @brief Define management actions performed after task conclusion.
 *
 * @param retval     Return value.
 * @param management Management action.
 * @param merge      Merge arguments function.
 * @param arg0       Argument value.
 * @param arg1       Argument value.
 * @param arg2       Argument value.
 */
PUBLIC void task_exit(
	int retval,
	int management,
	task_merge_args_fn merge,
	word_t arg0,
	word_t arg1,
	word_t arg2,
	word_t arg3,
	word_t arg4
)
{
	/* Dispatcher thread cannot block waiting for a task. */
	if (UNLIKELY(thread_get_curr() != KTHREAD_DISPATCHER))
		kpanic("[kernel][task] task_exit must be only called by the Dispatcher.");

	spinlock_lock(&taskboard.lock);

		/* Valid management? */
		if (LIKELY(WITHIN(management, TASK_MANAGEMENT_USER0, ((TASK_MANAGEMENT_ERROR << 1) - 1))))
			taskboard.management = management;
		else
			taskboard.management = TASK_MANAGEMENT_ERROR;

		/* Sets merge function. */
		taskboard.merge = merge;

		/* Sets arguments. */
		taskboard.exit_args[0] = arg0;
		taskboard.exit_args[1] = arg1;
		taskboard.exit_args[2] = arg2;
		taskboard.exit_args[3] = arg3;
		taskboard.exit_args[4] = arg4;

		/* Sets retval. */
		taskboard.retval = retval;

		/* Notify that task_exit was called (temp because we return). */
		taskboard.exit = true;

	spinlock_unlock(&taskboard.lock);

	/**
	 * TODO: We must not return.
	 * TODO: We must get and set up the return value of the task.
	 */
}

/*============================================================================*
 * task_create()                                                              *
 *============================================================================*/

/**
 * @brief Create a task.
 *
 * Sets the struct parameters and initializes the mutex for the waiting
 * control. The @p arg pointer can be a NULL pointer.
 *
 * @param task     Task pointer.
 * @param fn       Function pointer.
 * @param priority Priority level.
 * @param period   Period of the task.
 * @param releases When the semaphore will be released based on the
 *                 management value.
 *
 * @return Zero if successfully create the task, non-zero otherwise.
 */
PUBLIC int task_create(
	struct task * task,
	task_fn fn,
	int priority,
	int period,
	char releases
)
{
	/* Invalid task. */
	if (UNLIKELY(task == NULL || fn == NULL))
		return (-EINVAL);

	/**
	 * TODO: Should we verify if we are setting values of a task that is on
	 * specific states? For instance, READY or STOPPED.
	 *
	 * I think we cannot verify if a task is already created because maybe we
	 * are reusing an old task structure or a task on a dirty segment of memory.
	 * However, if the task_unlink was called, the task ID is invalid.
	 */

	/* Init the resource. */
	task->resource = RESOURCE_INITIALIZER;
	resource_set_used(&task->resource);

	/* Is it a periodic task? */
	if (period > 0)
	{
		task->period        = period;
		task->schedule_type = TASK_SCHEDULE_PERIODIC;
	}

	/* Normal task. */
	else
	{
		task->period        = 0;
		task->schedule_type = TASK_SCHEDULE_READY;
	}

	/* Dependendy graph. */
	task->nparents     = 0;
	task->rparents     = 0;
	task->parent_types = 0;
	task->nchildren    = 0;
	for (int i = 0; i < TASK_CHILDREN_MAX; ++i)
		task->children[i] = TASK_NODE_INVALID;

	/* Management. */
	task->fn    = fn;
	task->state = TASK_STATE_NOT_STARTED;
	task->color = 0;

	spinlock_lock(&taskboard.lock);
		task->id = taskboard.counter++;
	spinlock_unlock(&taskboard.lock);

	/* Control variables. */
	task->priority = priority;
	task->releases = releases;
	semaphore_init(&task->sem, 0);

	/* Success. */
	return (0);
}

/*============================================================================*
 * task_unlink()                                                              *
 *============================================================================*/

/**
 * @brief Destroy a task.
 *
 * Sets the struct parameters and initializes the mutex for the waiting
 * control. The @p arg pointer can be a NULL pointer.
 *
 * @param task Task pointer.
 *
 * @return Zero if successfully unlink the task, non-zero otherwise.
 */
PUBLIC int task_unlink(struct task * task)
{
	int ret = (-EINVAL); /* Return value. */

	/* Invalid task. */
	if (UNLIKELY(task == NULL))
		return (-EINVAL);

	spinlock_lock(&taskboard.lock);

		/* Invalid task. */
		if (UNLIKELY(task_is_invalid(task)))
			goto error;

		/**
		 * Has parents or children.
		 *
		 * TODO: Can we unlink all hard dependencies together?
		 */
		if (UNLIKELY(task->rparents > 0 || task->nchildren > 0))
			goto error;

		/* Release task resources and reset informations. */
		if (LIKELY((ret = __task_move(task, TASK_STATE_INVALID)) >= 0))
		{
			task->id = TASK_NULL_ID;

			/* TODO: Destroy semaphore (issue #299) */
		}

error:
	spinlock_unlock(&taskboard.lock);

	/* Success or failure. */
	return (ret);
}

/*============================================================================*
 * task_connect()                                                             *
 *============================================================================*/

/**
 * @brief Create a connection on the @p child task to the @p parent task.
 *
 * @param parent       Independent task.
 * @param child        Dependent task.
 * @param is_dependency      Connection's type.
 * @param is_temporary Connection's lifetime.
 * @param triggers     Connection's triggers.
 *
 * @return Zero if successfully create the dependency, non-zero otherwise.
 */
PUBLIC int task_connect(
	struct task * parent,
	struct task * child,
	bool is_dependency,
	bool is_temporary,
	char triggers
)
{
	int ret = (-EINVAL);        /* return value.  */
	struct section_guard guard; /* Section guard. */

	/* Invalid triggers. */
	if (UNLIKELY(!(triggers & TASK_TRIGGER_ALL)))
		return (-EINVAL);

#if 0 /* TODO: Should we force a unique error trigger? */
	/* Invalid triggers: throw or catch. */
	if (UNLIKELY((triggers & TASK_TRIGGER_ERROR_THROW) && (triggers & TASK_TRIGGER_ERROR_CATCH)))
		return (-EINVAL);
#endif

	/* Prevent this call be preempted by any maskable interrupt. */
	section_guard_init(&guard, &taskboard.lock, INTERRUPT_LEVEL_NONE);

	section_guard_entry(&guard);

		/* Invalid parent. */
		if (UNLIKELY(task_is_invalid(parent)))
			goto error;

		/* Invalid child. */
		if (UNLIKELY(task_is_invalid(child)))
			goto error;

		/* Parent has no free slot on dependency list. */
		if (UNLIKELY(parent->nchildren >= ((int) TASK_CHILDREN_MAX)))
			goto error;

		/* Parent has no free slot on dependency list. */
		if (UNLIKELY(child->rparents >= ((int) TASK_PARENTS_MAX)))
			goto error;

		/* Sanity check. */
		KASSERT(!parent->children[(int) parent->nchildren].is_valid);

		/* Connect: parent -> child. */
		parent->children[(int) parent->nchildren].is_valid      = true;
		parent->children[(int) parent->nchildren].is_dependency = is_dependency;
		parent->children[(int) parent->nchildren].is_temporary  = is_temporary;
		parent->children[(int) parent->nchildren].triggers      = (byte_t) triggers;
		parent->children[(int) parent->nchildren].child         = child;
		parent->nchildren++;

		/* Connect: parent <- child. */
		if (is_dependency)
			child->parent_types |= (1 << child->rparents);
		child->rparents++;
		child->nparents++;

		ret = (0);

error:
	section_guard_exit(&guard);

	/* Success or failure. */
	return (ret);
}

/*============================================================================*
 * task_disconnect()                                                          *
 *============================================================================*/

/**
 * @brief Destroy a dependency on the @p child task to the @p parent task.
 *
 * @param parent Independent task.
 * @param child  Dependent task.
 *
 * @return Zero if successfully destroy the dependency, non-zero otherwise.
 */
PUBLIC int task_disconnect(struct task * parent, struct task * child)
{
	int ret;                    /* Return value.  */
	struct section_guard guard; /* Section guard. */

	/* Invalid tasks. */
	if (UNLIKELY(parent == NULL || child == NULL))
		return (-EINVAL);

	/* Prevent this call be preempted by any maskable interrupt. */
	section_guard_init(&guard, &taskboard.lock, INTERRUPT_LEVEL_NONE);

	section_guard_entry(&guard);
		ret = __task_disconnect(parent, child);
	section_guard_exit(&guard);

	return (ret);
}

/*============================================================================*
 * task_dispatch()                                                            *
 *============================================================================*/

/**
 * @brief Enqueue a task to the dispatcher thread operate.
 *
 * @param task Task pointer.
 *
 * @returns Zero if successfully dispatch a task, non-zero otherwise.
 */
PUBLIC int task_dispatch(
	struct task * task,
	word_t arg0,
	word_t arg1,
	word_t arg2,
	word_t arg3,
	word_t arg4
)
{
	int ret = (-EINVAL);        /* Return value.  */
	struct section_guard guard; /* Section guard. */

	/* Prevent this call be preempted by any maskable interrupt. */
	section_guard_init(&guard, &taskboard.lock, INTERRUPT_LEVEL_NONE);

	section_guard_entry(&guard);

		/* Invalid task. */
		if (UNLIKELY(task_is_invalid(task)))
			goto error;

		task->args[0] = arg0;
		task->args[1] = arg1;
		task->args[2] = arg2;
		task->args[3] = arg3;
		task->args[4] = arg4;
		task->retval  = 0;

		/* Dispatch task. */
		ret = __task_dispatch(task);

error:
	section_guard_exit(&guard);

	/* Success or failure. */
	return (ret);
}

/*============================================================================*
 * task_wait()                                                                *
 *============================================================================*/

/**
 * @brief Wait for a task to complete.
 *
 * @param task Task pointer.
 *
 * @returns Zero if successfully wait for a task, non-zero otherwise.
 */
PUBLIC int task_wait(struct task * task)
{
	int ret = (-EINVAL); /* Return value. */

	/* Dispatcher thread cannot block waiting for a task. */
	if (UNLIKELY(thread_get_curr() == KTHREAD_DISPATCHER))
		return (-EINVAL);

	spinlock_lock(&taskboard.lock);

		/* Invalid task. */
		if (UNLIKELY(task_is_invalid(task)))
			goto error;

	spinlock_unlock(&taskboard.lock);

	/* Waits for all stages be completed. */
	semaphore_down(&task->sem);

	spinlock_lock(&taskboard.lock);

		ret = task->retval;
error:
	spinlock_unlock(&taskboard.lock);

	/* Success or failure. */
	return (ret);
}

/*============================================================================*
 * task_trywait()                                                             *
 *============================================================================*/

/**
 * @brief Wait for a task to complete.
 *
 * @param task Task pointer.
 *
 * @returns Zero if successfully wait for a task, non-zero otherwise.
 */
PUBLIC int task_trywait(struct task * task)
{
	int ret = (-EINVAL); /* Return value. */

	spinlock_lock(&taskboard.lock);

		/* Invalid task. */
		if (UNLIKELY(task_is_invalid(task)))
			goto error;

		/* Tries to wait for the task be completed. */
		if (semaphore_trydown(&task->sem) >=  0)
			ret = task->retval;
		else
			ret = (-EPROTO);

error:
	spinlock_unlock(&taskboard.lock);

	return (ret);
}

/*============================================================================*
 * task_continue()                                                            *
 *============================================================================*/

/**
 * @brief Continue a blocked task.
 *
 * @details If the task is the current task, it will do not make any
 * operation over the task. If it is blocked, we move the task to the active
 * task queue.
 *
 * @param task Task pointer.
 *
 * @returns Zero if successfully wakeup the task. -EAGAIN if the thread is not
 * tracked (THREAD_STATE_NOT_STARTED). Or, -EINVAL for invalid task pointer.
 */
PUBLIC int task_continue(struct task * task)
{
	int ret = (-EBADF); /* Return value. */

	spinlock_lock(&taskboard.lock);

		/* Valid task. */
		if (UNLIKELY(task_is_invalid(task)))
			goto error;

		/* Based on task state: */
		switch (task->state)
		{
			/* Dispatch task. */
			case TASK_STATE_STOPPED:
			case TASK_STATE_PERIODIC:
				ret = __task_dispatch(task);
				break;

			/* Restore schedule type. */
			case TASK_STATE_NOT_STARTED:
			case TASK_STATE_COMPLETED:
				task->schedule_type = task->period > 0 ?
					TASK_SCHEDULE_PERIODIC : TASK_SCHEDULE_READY;
			/* Do nothing. */
			case TASK_STATE_READY:
			case TASK_STATE_RUNNING:
				ret = (0);
				break;

			/* Otherwise, it is an error. */
			case TASK_STATE_ERROR:
			case TASK_STATE_INVALID:
			default:
				break;
		}

error:
	spinlock_unlock(&taskboard.lock);

	/* Success or failure. */
	return (ret);
}

/*============================================================================*
 * task_complete()                                                            *
 *============================================================================*/

/**
 * @brief Complete a task.
 *
 * @param task Task pointer.
 *
 * @returns Zero if successfully complete the task, non-zero otherwise.
 */
PUBLIC int task_complete(struct task * task, char management)
{
	int ret = (-EBADF); /* Return value. */

	/* Invalid task. */
	if (UNLIKELY(task == NULL))
		return (-EINVAL);

	if (UNLIKELY(!WITHIN(management, TASK_MANAGEMENT_USER0, TASK_MANAGEMENT_AGAIN)))
		return (-EINVAL);

	spinlock_lock(&taskboard.lock);

		/* Valid task? */
		if (UNLIKELY(task_is_invalid(task)))
			goto error;

		/* Based on task state: */
		switch (task->state)
		{
			/* Dispatch task. */
			case TASK_STATE_NOT_STARTED:
			case TASK_STATE_READY:
			case TASK_STATE_STOPPED:
			case TASK_STATE_PERIODIC:
			case TASK_STATE_COMPLETED:
				ret = __task_notify(task, TASK_STATE_COMPLETED, management);
				break;

			/* Otherwise, it is an error. */
			case TASK_STATE_RUNNING:
			case TASK_STATE_ERROR:
			case TASK_STATE_INVALID:
			default:
				break;
		}

error:
	spinlock_unlock(&taskboard.lock);

	/* Success or failure. */
	return (ret);
}

/*============================================================================*
 * task_stop()                                                                *
 *============================================================================*/

/**
 * @brief Complete a task.
 *
 * @param task Task pointer.
 *
 * @returns Zero if successfully complete the task, non-zero otherwise.
 */
PUBLIC int task_stop(struct task * task)
{
	int ret = (-EBADF); /* Return value. */

	/* Invalid task. */
	if (UNLIKELY(task == NULL))
		return (-EINVAL);

	spinlock_lock(&taskboard.lock);

		/* Valid task? */
		if (UNLIKELY(task_is_invalid(task)))
			goto error;

		/* Based on task state: */
		switch (task->state)
		{
			/* Dispatch task. */
			case TASK_STATE_READY:
			case TASK_STATE_PERIODIC:
				ret = __task_stop(task);
				break;

			/* Change schedule type. */
			case TASK_STATE_NOT_STARTED:
			case TASK_STATE_COMPLETED:
				task->schedule_type = TASK_SCHEDULE_STOPPED;
			/* Do nothing. */
			case TASK_STATE_STOPPED:
				ret = (0);
				break;

			/* Otherwise, it is an error. */
			case TASK_STATE_RUNNING:
			case TASK_STATE_ERROR:
			case TASK_STATE_INVALID:
			default:
				break;
		}

error:
	spinlock_unlock(&taskboard.lock);

	/* Success or failure. */
	return (ret);
}

/*============================================================================*
 * User Behaviors                                                             *
 *============================================================================*/

/*============================================================================*
 * task_handler()                                                             *
 *============================================================================*/

/**
 * @brief Handle scheduling kernel events.
 */
PRIVATE void task_handler(int evnum)
{
	int coreid;                              /* Core id.               */
	struct task * task;                      /* Task pointer.          */
	struct resource_arrangement * emissions; /* Target emission queue. */

	KASSERT(evnum == KEVENT_TASK);

	coreid    = core_get_id();
	emissions = &taskboard.emissions[coreid];

	/* We do not want to be interrupted in the critical region. */
	spinlock_lock(&taskboard.lock);

	/* Endless loop. */
	while (LIKELY((task = TASK_PTR(resource_dequeue(emissions))) != NULL))
	{
			task->state = TASK_STATE_RUNNING;

		spinlock_unlock(&taskboard.lock);

		/* Invalid function. */
		if (UNLIKELY(task->fn == NULL))
			kpanic("[kernel][task] Invalid emission function.");

		/* Executes the function of the task. */
		task->retval = task->fn(
			task->args[0],
			task->args[1],
			task->args[2],
			task->args[3],
			task->args[4]
		);

		spinlock_lock(&taskboard.lock);

			/* Completes the task. */
			task->state = TASK_STATE_COMPLETED;
			semaphore_up(&task->sem);
	}

	spinlock_unlock(&taskboard.lock);
}

/*============================================================================*
 * task_create()                                                              *
 *============================================================================*/

/**
 * @brief Emit a task to the target core operate.
 *
 * @param task   Task pointer.
 * @param coreid Core ID.
 *
 * @returns Zero if successfully emit the task, non-zero otherwise.
 */
PUBLIC int task_emit(
	struct task * task,
	int coreid,
	word_t arg0,
	word_t arg1,
	word_t arg2,
	word_t arg3,
	word_t arg4
)
{
	int ret; /* Return value. */

	/* Invalid task. */
	if (UNLIKELY(task == NULL))
		return (-EINVAL);

	/* Invalid core. */
	if (UNLIKELY(!WITHIN(coreid, 0, CORES_NUM)))
		return (-EINVAL);

	task->args[0] = arg0;
	task->args[1] = arg1;
	task->args[2] = arg2;
	task->args[3] = arg3;
	task->args[4] = arg4;
	task->retval  = 0;

	/* Dispatch task. */
	spinlock_lock(&taskboard.lock);
		ret = __task_emit(task, coreid);
	spinlock_unlock(&taskboard.lock);

	/* Failure. */
	if (ret < 0)
		return (ret);

	/* Notify emit events. */
	if (core_get_id() != coreid)
		ret = kevent_notify(KEVENT_TASK, coreid);

	/* Handle the task. */
	else
		task_handler(KEVENT_TASK);

	/* Success. */
	return (0);
}

/*============================================================================*
 * Initialization and Management                                              *
 *============================================================================*/

/*============================================================================*
 * task_tick()                                                                *
 *============================================================================*/

/**
 * @brief Notify a system tick to the periodic queue.
 */
PUBLIC void task_tick(void)
{
	int trials;
	int locked;
	struct task * task; /* Task pointer. */

	trials = 25;

	do
		locked = spinlock_trylock(&taskboard.lock);
	while ((!locked) && (--trials > 0));

		/* Ignore. Maybe we interrupted inside this critical region. */
		if (!locked)
			return;

		/**
		 * Decrement the period of the periodic queue's head and try to
		 * dequeue the task.
		 */
		while ((task = periodic_queue_dequeue(&taskboard.periodics)) != NULL)
		{
			__task_dispatch(task);

			/* There is no more task to pop. */
			if (periodic_queue_next_period(&taskboard.periodics) != 0)
				break;
		}

	spinlock_unlock(&taskboard.lock);
}

/*============================================================================*
 * task_init()                                                                *
 *============================================================================*/

/**
 * @brief Initializes the task system.
 */
PUBLIC void task_init(void)
{
	/* Configures control variable. */
	taskboard.counter  = 1;
	taskboard.color    = 1;
	taskboard.shutdown = false;

	spinlock_init(&taskboard.lock);
	semaphore_init(&taskboard.sem, 0);

	/* Configures management variables. */
	taskboard.management   = TASK_MANAGEMENT_USER0;
	taskboard.exit         = false;
	taskboard.ctask        = NULL;
	taskboard.merge        = NULL;
	taskboard.retval       = 0;
	for (int i = 0; i < TASK_ARGS_NUM; ++i)
		taskboard.exit_args[i] = 0;

	/* Configures queues. */
	taskboard.actives   = RESOURCE_ARRANGEMENT_INITIALIZER;
	taskboard.waiting   = RESOURCE_ARRANGEMENT_INITIALIZER;
	taskboard.periodics = RESOURCE_ARRANGEMENT_INITIALIZER;

	for (int i = 0; i < CORES_NUM; ++i)
		taskboard.emissions[i] = RESOURCE_ARRANGEMENT_INITIALIZER;

	/* Set task handler. */
	KASSERT(kevent_set_handler(KEVENT_TASK, task_handler) == 0);
}

#endif /* __NANVIX_USE_TASKS */

