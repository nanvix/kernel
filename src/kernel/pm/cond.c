/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
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
 * @details This function causes the calling thread to block, until the
 * condition variable pointed to by @p cond is signaled and the calling thread
 * is chosen to run. If @p lock is unlocked before the calling thread blocks,
 * and when it wakes up the lock is re-acquired.
 *
 * @see cond_broadcast()
 */
int cond_wait(struct condvar *cond, spinlock_t *lock)
{
    KASSERT(cond != NULL);
    KASSERT(lock != NULL);

    struct thread *curr_thread = thread_get_curr();

    /* Enqueue calling thread. */
    spinlock_lock(&cond->lock);
    curr_thread->next = cond->queue;
    cond->queue = curr_thread;
    spinlock_unlock(&cond->lock);

    /* Put the calling thread to sleep. */
    thread_sleep(lock);

    return (0);
}

/**
 * @details This function sends a wakeup signal to all threads
 * that are currently blocked waiting on the conditional variable
 * pointed to by @p cond.
 *
 * @see cond_wait().
 */
int cond_broadcast(struct condvar *cond)
{
    KASSERT(cond != NULL);

    spinlock_lock(&cond->lock);

    /* Wakeup all threads. */
    while (UNLIKELY(cond->queue != NULL)) {
        thread_wakeup(cond->queue);
        cond->queue = cond->queue->next;
    }

    spinlock_unlock(&cond->lock);

    return (0);
}
