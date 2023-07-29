/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_ARCH_X86_SPINLOCK_H_
#define NANVIX_KERNEL_HAL_ARCH_X86_SPINLOCK_H_

#ifndef _ASM_FILE_
#include <stdint.h>
#endif /* !_ASM_FILE_ */

/**
 * @name Spinlock State
 */
/**@{*/
#define SPINLOCK_UNLOCKED 0x0 /**< Unlocked */
#define SPINLOCK_LOCKED 0x1   /**< Locked   */
/**@}*/

#ifndef _ASM_FILE_

/**
 * @brief Spinlock.
 */
typedef uint32_t spinlock_t;

/**
 * @brief Initializes a spinlock_t.
 *
 * @param lock Target spinlock_t.
 *
 * @todo Implement this function.
 */
static inline void spinlock_init(spinlock_t *lock)
{
    *lock = SPINLOCK_UNLOCKED;
}

/**
 * @brief Attempts to lock a spinlock_t.
 *
 * @param lock Target spinlock_t.
 *
 * @returns Upon successful completion, the spinlock_t pointed to by
 * @p lock is locked and zero is returned. Upon failure, non-zero
 * is returned instead, and the lock is not acquired by the
 * caller.
 *
 * @todo Implement this function.
 */
static inline int spinlock_trylock(spinlock_t *lock)
{
    return (!__sync_bool_compare_and_swap(
        lock, SPINLOCK_UNLOCKED, SPINLOCK_LOCKED));
}

/**
 * @brief Locks a spinlock_t.
 *
 * @param lock Target spinlock_t.
 *
 * @todo Implement this function.
 */
static inline void spinlock_lock(spinlock_t *lock)
{
    while (spinlock_trylock(lock)) {
        /* noop */;
    }
    __sync_synchronize();
}

/**
 * @brief Unlocks a spinlock_t.
 *
 * @param lock Target spinlock_t.
 *
 * @todo Implement this function.
 */
static inline void spinlock_unlock(spinlock_t *lock)
{
    *lock = SPINLOCK_UNLOCKED;
    __sync_synchronize();
}

#endif /* !_ASM_FILE_ */

#endif /* NANVIX_KERNEL_HAL_ARCH_X86_SPINLOCK_H_ */
