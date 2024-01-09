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
#include <stdbool.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details This function performs a down operation in the semaphore pointed to
 * by @p sem. It atomically checks the current value of @p sem. If it is greater
 * than one, it decrements the semaphore counter by one and the calling process
 * continue its execution, flow as usual.  Otherwise, the calling process sleeps
 * until another process performs a call to semaphore_up() on this semaphore.
 *
 * @see SEMAPHORE_INIT(), semaphore_up()
 */
void semaphore_down(struct semaphore *sem)
{
    KASSERT(sem != NULL);

    while (true) {
        if (sem->count > 0) {
            break;
        }

        cond_wait(&sem->cond);
    }

    sem->count--;
}

/**
 * @details This function performs an up operation in a semaphore pointed to by
 * @p sem. It atomically increments the current value of @p and wakes up all
 * processes that were sleeping in this semaphore, waiting for a semaphore_up()
 * operation.
 *
 * @see SEMAPHORE_INIT(), semaphore_down()
 */
void semaphore_up(struct semaphore *sem)
{
    KASSERT(sem != NULL);

    sem->count++;
    cond_broadcast(&sem->cond);
}
