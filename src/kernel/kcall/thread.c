/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/errno.h>
#include <nanvix/kernel/pm.h>
#include <stdnoreturn.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Creates a new thread.
 */
tid_t kcall_thread_create(void *(*start)(), void *args, void (*caller)())
{
    KASSERT((word_t)start > USER_BASE_VIRT && (word_t)start < USER_END_VIRT);
    KASSERT((word_t)caller > USER_BASE_VIRT && (word_t)caller < USER_END_VIRT);
    return (thread_create(process_get_curr(), start, args, caller));
}

/**
 * @details Exits the calling thread.
 */
noreturn void kcall_thread_exit(void *retval)
{
    thread_exit(retval);
    UNREACHABLE();
}

/**
 * @details Yields the calling thread.
 */
void kcall_thread_yield(void)
{
    thread_yield();
}

/**
 * @details Returns the ID of the calling thread.
 */
tid_t kcall_thread_get_id(void)
{
    return (thread_get_curr());
}

/**
 * @details Waits for a thread to terminate.
 */
int kcall_thread_join(tid_t tid, void **retval)
{
    return (thread_join(tid, retval));
}

/**
 * @details Detaches a thread.
 */
int kcall_thread_detach(tid_t tid)
{
    return (thread_detach(tid));
}
