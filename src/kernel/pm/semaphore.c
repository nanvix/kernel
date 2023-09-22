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
#include <stdbool.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details This function performs a down operation in the semaphore pointed to
 * by @p sem. It atomically checks the current value of @p sem. If it is greater
 * than one, it decrements the semaphore counter by one and the calling thread
 * continue its execution, flow as usual.  Otherwise, the calling thread sleeps
 * until another thread performs a call to semaphore_up() on this semaphore.
 *
 * @see SEMAPHORE_INIT(), semaphore_up()
 */
void semaphore_down(struct semaphore *sem)
{
    KASSERT(sem != NULL);

    spinlock_lock(&sem->lock);

    while (true) {
        if (sem->count > 0) {
            break;
        }

        cond_wait(&sem->cond, &sem->lock);
    }

    sem->count--;

    spinlock_unlock(&sem->lock);
}

/**
 * @details This function performs an up operation in a semaphore pointed to by
 * @p sem. It atomically increments the current value of @p and wakes up all
 * threads that were sleeping in this semaphore, waiting for a semaphore_up()
 * operation.
 *
 * @see SEMAPHORE_INIT(), semaphore_down()
 */
void semaphore_up(struct semaphore *sem)
{
    KASSERT(sem != NULL);

    spinlock_lock(&sem->lock);
    sem->count++;
    cond_broadcast(&sem->cond);
    spinlock_unlock(&sem->lock);
}
