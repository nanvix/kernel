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
tid_t kcall_thread_create(pid_t pid)
{
    // Check if pid is valid.
    if (!WITHIN(pid, 0, PROCESS_MAX)) {
        return (-EINVAL);
    }

    // Spawn thread.
    return (thread_create(pid, false));
}

/**
 * @details Exits the calling thread.
 */
noreturn void kcall_thread_exit(void)
{
    thread_exit();
    UNREACHABLE();
}

/**
 * @details Yields the calling thread.
 */
noreturn void kcall_thread_yield(void)
{
    thread_yield();
    UNREACHABLE();
}

/**
 * @details Returns the ID of the calling thread.
 */
tid_t kcall_thread_get_id(void)
{
    return (thread_get_curr());
}
