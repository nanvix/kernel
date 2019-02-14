/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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

#ifndef NANVIX_HAL_SPINLOCK_H_
#define NANVIX_HAL_SPINLOCK_H_

	/* Core Interface Implementation */
	#include <nanvix/hal/core/_core.h>

/*============================================================================*
 * Interface Implementation Checking                                          *
 *============================================================================*/

	/* Constants */
	#ifndef SPINLOCK_LOCKED
	#error "SPINLOCK_LOCKED not defined"
	#endif
	#ifndef SPINLOCK_UNLOCKED
	#error "SPINLOCK_UNLOCKED not defined"
	#endif

	/* Types & Structures */
	#ifndef __spinlock_t
	#error "spinlock_t not defined?"
	#endif

	/* Functions */
	#ifndef __spinlock_init_fn
	#error "spinlock_init() not defined?"
	#endif
	#ifndef __spinlock_lock_fn
	#error "spinlock_lock() not defined?"
	#endif
	#ifndef __spinlock_trylock_fn
	#error "spinlock_trylock() not defined?"
	#endif
	#ifndef __spinlock_unlock_fn
	#error "spinlock_unlock() not defined?"
	#endif

/*============================================================================*
 * Spinlocks Interface                                                        *
 *============================================================================*/

/**
 * @addtogroup kernel-hal-core-spinlock Spinlock
 * @ingroup kernel-hal-core
 *
 * @brief Spinlocks HAL Interface
 */
/**@{*/

	#include <nanvix/const.h>

	/**
	 * @brief Initializes a spinlock.
	 *
	 * @param lock Target spinlock.
	 */
	EXTERN void spinlock_init(spinlock_t *lock);

	/**
	 * @brief Locks a spinlock.
	 *
	 * @param lock Target spinlock.
	 */
	EXTERN void spinlock_lock(spinlock_t *lock);

	/**
	 * @brief Attempts to lock a spinlock.
	 *
	 * @param lock Target spinlock.
	 *
	 * @returns Upon successful completion, the spinlock pointed to by
	 * @p lock is locked and non-zero is returned. Upon failure, zero
	 * is returned instead, and the lock is not acquired by the
	 * caller.
	 */
	EXTERN int spinlock_trylock(spinlock_t *lock);

	/**
	 * @brief Unlocks a spinlock.
	 *
	 * @param lock Target spinlock.
	 */
	EXTERN void spinlock_unlock(spinlock_t *lock);

/**@}*/

#endif /* NANVIX_HAL_SPINLOCK_H_ */
