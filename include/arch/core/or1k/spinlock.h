/*
 * MIT License
 *
 * Copyright(c) 2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2018 Davidson Francis     <davidsondfgl@gmail.com>
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

#ifndef ARCH_CORE_OR1K_SPINLOCK_H_
#define ARCH_CORE_OR1K_SPINLOCK_H_

/**
 * @addtogroup or1k-core-spinlock Spinlock
 * @ingroup or1k-core
 *
 * @brief or1k Spinlocks
 */
/**@{*/

#ifndef _ASM_FILE_

	#include <nanvix/const.h>
	#include <stdint.h>

#endif /* _ASM_FILE_ */

	/**
	 * @name Spinlock State
	 */
	/**@{*/
	#define OR1K_SPINLOCK_UNLOCKED 0x0 /**< Unlocked */
	#define OR1K_SPINLOCK_LOCKED   0x1 /**< Locked   */
	/**@}*/

	/**
	 * @brief Spinlock.
	 */
	typedef uint32_t or1k_spinlock_t;

	/**
	 * @brief Initializes a or1k_spinlock_t.
	 *
	 * @param lock Target or1k_spinlock_t.
	 */
	static inline void or1k_spinlock_init(or1k_spinlock_t *lock)
	{
		register or1k_spinlock_t *lock_reg
			__asm__("r5") = lock;

		__asm__ __volatile__
		(
			"l.sw 0(r5), r0"
			:
			: "r" (lock_reg)
		);
	}

	/**
	 * @brief Attempts to lock a or1k_spinlock_t.
	 *
	 * @param lock Target or1k_spinlock_t.
	 *
	 * @returns Upon successful completion, the or1k_spinlock_t pointed to by
	 * @p lock is locked and zero is returned. Upon failure, non-zero
	 * is returned instead, and the lock is not acquired by the
	 * caller.
	 */
	static inline int or1k_spinlock_trylock(or1k_spinlock_t *lock)
	{
		register or1k_spinlock_t *lock_reg
			__asm__("r5") = lock;
		register or1k_spinlock_t lock_value
			__asm__("r7") = OR1K_SPINLOCK_UNLOCKED;
		register unsigned locked
			__asm__("r9") = 0;

		/* First, atomically reads the lock. */
		__asm__ __volatile__
		(
			"l.lwa r7, 0(r5)"
			: "=r" (lock_value)
			: "r" (lock_reg)
		);

		/* Lock already locked. */
		if (lock_value == OR1K_SPINLOCK_LOCKED)
			return (1);

		/* Tries to lock. */
		lock_value = OR1K_SPINLOCK_LOCKED;
		__asm__ __volatile__
		(
			"l.swa   0(r5),  r7\n"
			"l.mfspr r9, r0, 0x11\n"  /* Supervisor Register. */
			"l.andi  r9, r9, 0x200\n" /* Condition Flag.      */
			: "=r" (locked)
			: "r"  (lock_reg),
			  "r"  (lock_value)
		);

		/* Check if lock was successful. */
		if (locked)
			return (0);
		else
			return (1);
	}

	/**
	 * @brief Locks a or1k_spinlock_t.
	 *
	 * @param lock Target or1k_spinlock_t.
	 */
	static inline void or1k_spinlock_lock(or1k_spinlock_t *lock)
	{
		while (or1k_spinlock_trylock(lock))
			/* noop */;
	}

	/**
	 * @brief Unlocks a or1k_spinlock_t.
	 *
	 * @param lock Target or1k_spinlock_t.
	 */
	static inline void or1k_spinlock_unlock(or1k_spinlock_t *lock)
	{
		register or1k_spinlock_t *lock_reg
			__asm__("r5") = lock;

		__asm__ __volatile__
		(
			"1:\n"
			"	l.lwa r7, 0(r5)\n"
			"	l.swa 0(r5), r0\n"
			"	l.bnf 1b\n"
			"	l.nop\n"
			:
			: "r" (lock_reg)
		);
	}

/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @cond mor1kx
 */

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __spinlock_t          /**< @see spinlock_t    */
	#define __spinlock_init_fn    /**< spinlock_init()    */
	#define __spinlock_lock_fn    /**< spinlock_lock()    */
	#define __spinlock_trylock_fn /**< spinlock_trylock() */
	#define __spinlock_unlock_fn  /**< spinlock_unlock()  */
	/**@}*/

	/**
	 * @name Spinlock State
	 */
	/**@{*/
	#define SPINLOCK_UNLOCKED OR1K_SPINLOCK_UNLOCKED /**< @see OR1K_SPINLOCK_UNLOCKED */
	#define SPINLOCK_LOCKED   OR1K_SPINLOCK_LOCKED   /**< @see OR1K_SPINLOCK_LOCKED   */
	/**@}*/

#ifndef _ASM_FILE_

	/**
	 * @see or1k_spinlock_t
	 */
	typedef or1k_spinlock_t spinlock_t;

	/**
	 * @see ork1_spinlock_init().
	 */
	static inline void spinlock_init(spinlock_t *lock)
	{
		or1k_spinlock_init(lock);
	}

	/**
	 * @see ork1_spinlock_trylock().
	 */
	static inline int spinlock_trylock(spinlock_t *lock)
	{
		return (or1k_spinlock_trylock(lock));
	}

	/**
	 * @see ork1_spinlock_lock().
	 */
	static inline void spinlock_lock(spinlock_t *lock)
	{
		or1k_spinlock_lock(lock);
	}

	/**
	 * @see ork1_spinlock_unlock().
	 */
	static inline void spinlock_unlock(spinlock_t *lock)
	{
		or1k_spinlock_unlock(lock);
	}

#endif /* _ASM_FILE_ */

/**@endcond*/

#endif /* ARCH_CORE_OR1K_SPINLOCK_H_ */
