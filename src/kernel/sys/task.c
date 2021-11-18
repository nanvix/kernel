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

#include <nanvix/kernel/mm.h>
#include <nanvix/kernel/thread.h>
#include <nanvix/const.h>
#include <posix/errno.h>

#if (THREAD_MAX > 1)

/*============================================================================*
 * kernel_task_current()                                                      *
 *============================================================================*/

PUBLIC int kernel_task_current(struct task ** task)
{
#if __NANVIX_USE_TASKS

	/* Invalid pointers. */
	if (UNLIKELY(task == NULL))
		return (-EINVAL);

	*task = task_current();

	return (0);

#else

	UNUSED(task);

	return (-ENOSYS);

#endif
}

/*============================================================================*
 * kernel_task_create()                                                       *
 *============================================================================*/

PUBLIC int kernel_task_create(struct task * task, task_fn fn, int period, char releases)
{
#if __NANVIX_USE_TASKS

	/* Invalid pointers. */
	if (UNLIKELY(task == NULL || fn == NULL))
		return (-EINVAL);

	/* Invaild period. */
	if (UNLIKELY(period < 0))
		return (-EINVAL);

	return (task_create(task, fn, period, releases));

#else

	UNUSED(task);
	UNUSED(fn);
	UNUSED(period);

	return (-ENOSYS);

#endif
}

/*============================================================================*
 * kernel_task_unlink()                                                       *
 *============================================================================*/

PUBLIC int kernel_task_unlink(struct task * task)
{
#if __NANVIX_USE_TASKS

	/* Invalid pointer. */
	if (UNLIKELY(task == NULL))
		return (-EINVAL);

	return (task_unlink(task));

#else

	UNUSED(task);

	return (-ENOSYS);

#endif
}

/*============================================================================*
 * kernel_task_connect()                                                      *
 *============================================================================*/

PUBLIC int kernel_task_connect(
	struct task * parent,
	struct task * child,
	bool is_dependency,
	bool is_temporary,
	char triggers
)
{
#if __NANVIX_USE_TASKS

	/* Invalid pointers. */
	if (UNLIKELY(parent == NULL || child == NULL))
		return (-EINVAL);

	return (task_connect(parent, child, is_dependency, is_temporary, triggers));

#else

	UNUSED(parent);
	UNUSED(child);
	UNUSED(is_dependency);
	UNUSED(is_temporary);
	UNUSED(triggers);

	return (-ENOSYS);

#endif

}

/*============================================================================*
 * kernel_task_disconnect()                                                   *
 *============================================================================*/

PUBLIC int kernel_task_disconnect(struct task * parent, struct task * child)
{
#if __NANVIX_USE_TASKS

	/* Invalid pointers. */
	if (UNLIKELY(parent == NULL || child == NULL))
		return (-EINVAL);

	return (task_disconnect(parent, child));

#else

	UNUSED(parent);
	UNUSED(child);

	return (-ENOSYS);

#endif
}

/*============================================================================*
 * kernel_task_dispatch()                                                     *
 *============================================================================*/

PUBLIC int kernel_task_dispatch(struct task * task, word_t args[TASK_ARGS_NUM])
{
#if __NANVIX_USE_TASKS

	/* Invalid pointer. */
	if (UNLIKELY(task == NULL))
		return (-EINVAL);

	/* Invalid arguments. */
	if (UNLIKELY(args == NULL))
		return (-EINVAL);

	return (task_dispatch(task, args[0], args[1], args[2], args[3], args[4]));

#else

	UNUSED(task);
	UNUSED(args);

	return (-ENOSYS);

#endif
}

/*============================================================================*
 * kernel_task_emit()                                                         *
 *============================================================================*/

PUBLIC int kernel_task_emit(struct task * task, int coreid, word_t args[TASK_ARGS_NUM])
{
#if __NANVIX_USE_TASKS

	/* Invalid pointer. */
	if (UNLIKELY(task == NULL))
		return (-EINVAL);

	/* Invalid coreid. */
	if (UNLIKELY(!WITHIN(coreid, 0, CORES_NUM)))
		return (-EINVAL);

	/* Invalid arguments. */
	if (UNLIKELY(args == NULL))
		return (-EINVAL);

	return (task_emit(task, coreid, args[0], args[1], args[2], args[3], args[4]));

#else

	UNUSED(task);
	UNUSED(coreid);
	UNUSED(args);

	return (-ENOSYS);

#endif
}

/*============================================================================*
 * kernel_task_wait()                                                         *
 *============================================================================*/

PUBLIC int kernel_task_wait(struct task * task)
{
#if __NANVIX_USE_TASKS

	/* Invalid pointer. */
	if (UNLIKELY(task == NULL))
		return (-EINVAL);

	if (core_get_id() == KTHREAD_DISPATCHER_CORE)
	{
		if (UNLIKELY(thread_get_curr_id() == KTHREAD_DISPATCHER_TID))
			return (-EDEADLK);
	}

	return (task_wait(task));

#else

	UNUSED(task);

	return (-ENOSYS);

#endif
}

/*============================================================================*
 * kernel_task_trywait()                                                      *
 *============================================================================*/

PUBLIC int kernel_task_trywait(struct task * task)
{
#if __NANVIX_USE_TASKS

	/* Invalid pointer. */
	if (UNLIKELY(task == NULL))
		return (-EINVAL);

	return (task_trywait(task));

#else

	UNUSED(task);

	return (-ENOSYS);

#endif
}

/*============================================================================*
 * kernel_task_exit()                                                         *
 *============================================================================*/

PUBLIC int kernel_task_exit(
	int retval,
	int management,
	task_merge_args_fn fn,
	word_t args[TASK_ARGS_NUM]
)
{
#if __NANVIX_USE_TASKS

	/* Invalid arguments. */
	if (args == NULL)
		return (-EINVAL);

	task_exit(retval, management, fn, args[0], args[1], args[2], args[3], args[4]);

	return (0);

#else

	UNUSED(retval);
	UNUSED(management);
	UNUSED(fn);
	UNUSED(args);

	return (-ENOSYS);

#endif

}

/*============================================================================*
 * kernel_task_stop()                                                         *
 *============================================================================*/

PUBLIC int kernel_task_stop(struct task * task)
{
#if __NANVIX_USE_TASKS

	/* Invalid pointer. */
	if (UNLIKELY(task == NULL))
		return (-EINVAL);

	return (task_stop(task));

#else

	UNUSED(task);

	return (-ENOSYS);

#endif
}

/*============================================================================*
 * kernel_task_continue()                                                     *
 *============================================================================*/

PUBLIC int kernel_task_continue(struct task * task)
{
#if __NANVIX_USE_TASKS

	/* Invalid pointer. */
	if (UNLIKELY(task == NULL))
		return (-EINVAL);

	return (task_continue(task));

#else

	UNUSED(task);

	return (-ENOSYS);

#endif
}

/*============================================================================*
 * kernel_task_complete()                                                     *
 *============================================================================*/

PUBLIC int kernel_task_complete(struct task * task, char management)
{
#if __NANVIX_USE_TASKS
	/* Invalid pointer. */
	if (UNLIKELY(task == NULL))
		return (-EINVAL);

	if (UNLIKELY(management == 0))
		return (-EINVAL);

	return (task_complete(task, management));
#else

	UNUSED(task);

	return (-ENOSYS);

#endif
}

#endif

