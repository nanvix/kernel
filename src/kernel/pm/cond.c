/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/pm.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details This function causes the calling process to block, until the
 * condition variable pointed to by @p cond is signaled and the calling process
 * is chosen to run.
 *
 * @see cond_broadcast()
 */
int cond_wait(struct condvar *cond)
{
    KASSERT(cond != NULL);

    struct process *curr_process = process_get_curr();

    /* Enqueue calling process. */
    curr_process->next = cond->queue;
    cond->queue = curr_process;

    /* Put the calling process to sleep. */
    process_sleep();

    return (0);
}

/**
 * @details This function sends a wakeup signal to all processes that are
 * currently blocked waiting on the conditional variable pointed to by @p cond.
 *
 * @see cond_wait().
 */
int cond_broadcast(struct condvar *cond)
{
    KASSERT(cond != NULL);

    /* Wakeup all processes. */
    while (UNLIKELY(cond->queue != NULL)) {
        process_wakeup(cond->queue);
        cond->queue = cond->queue->next;
    }

    return (0);
}
