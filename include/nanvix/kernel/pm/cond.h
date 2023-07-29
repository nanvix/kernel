/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_PM_COND_H_
#define NANVIX_KERNEL_PM_COND_H_

#include <nanvix/kernel/hal.h>

/**
 * @brief Condition variable.
 */
struct condvar {
    spinlock_t lock;      /**< Lock for sleeping queue. */
    struct thread *queue; /**< Sleeping queue.          */
};

/**
 * @brief Static initializer for condition variables.
 *
 * The @p COND_INITIALIZER macro statically initiallizes a
 * conditional variable.
 */
#define COND_INITIALIZER                                                       \
    {                                                                          \
        .lock = SPINLOCK_UNLOCKED, .queue = NULL                               \
    }

/**
 * @brief Initializes a condition variable.
 *
 * @param cond Target condition variable.
 */
static inline void cond_init(struct condvar *cond)
{
    spinlock_init(&cond->lock);
    cond->queue = NULL;
}

/**
 * @brief Waits on a condition variable.
 *
 * @param cond Target condition variable.
 * @param lock Target spinlock to acquire.
 *
 * @returns Upon successful completion zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern int cond_wait(struct condvar *cond, spinlock_t *lock);

/**
 * @brief Unlocks all threads waiting on a condition variable.
 *
 * @param cond Target condition variable.
 *
 * @returns Upon successful completion zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern int cond_broadcast(struct condvar *cond);

#endif /* NANVIX_KERNEL_PM_COND_H_ */
