/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_PM_SEMAPHORE_H_
#define NANVIX_KERNEL_PM_SEMAPHORE_H_

#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/pm/cond.h>

/**
 * @brief Semaphore
 */
struct semaphore {
    int count;           /** Semaphore counter.  */
    spinlock_t lock;     /** Semaphore lock.     */
    struct condvar cond; /** Condition variable. */
};

/**
 * @brief Static initializer for semaphores.
 *
 * The @p SEMAPHORE_INIT macro statically initializes the fields of
 * a semaphore. The initial value of the semaphore is set to @p x
 * in the initialization.
 *
 * @param x Initial value for semaphore.
 */
#define SEMAPHORE_INITIALIZER(x)                                               \
    {                                                                          \
        .count = (x), .lock = SPINLOCK_UNLOCKED, .cond = COND_INITIALIZER,     \
    }

/**
 * @brief Initializes a semaphore.
 *
 * The semaphore_init() function dynamically initializes the
 * fields of the semaphore pointed to by @p sem. The initial value
 * of the semaphore is set to @p x in the initialization.
 *
 * @param sem Target semaphore.
 * @param x   Initial semaphore value.
 */
static inline void semaphore_init(struct semaphore *sem, int x)
{
    KASSERT(x >= 0);
    KASSERT(sem != NULL);

    sem->count = x;
    spinlock_init(&sem->lock);
    cond_init(&sem->cond);
}

/**
 * @brief Performs a down operation in a semaphore.
 *
 * @param sem Target semaphore.
 */
extern void semaphore_down(struct semaphore *sem);

/**
 * @brief Performs an up operation in a semaphore.
 *
 * @param sem target semaphore.
 */
extern void semaphore_up(struct semaphore *sem);

#endif /* NANVIX_KERNEL_PM_SEMAPHORE_H_ */
