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

#ifndef ARCH_OR1K_SPINLOCK_H_
#define ARCH_OR1K_SPINLOCK_H_

/**
 * @addtogroup ork1-spinlock Spinlocks
 * @ingroup ork1
 *
 * @brief ork1 Spinlocks
 */
/**@{*/

	/* External dependencies. */
	#include <nanvix/const.h>
	#include <stdint.h>

/*============================================================================*
 *                             Spinlocks Interface                            *
 *============================================================================*/

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
	typedef uint64_t spinlock_t;

/*============================================================================*
 *                              Exported Interface                            *
 *============================================================================*/

	/**
	 * @name Provided Interface
	 *
	 * @cond ork1
	 */
	/**@{*/
	#define __spinlock_t          /**< @p spinlock_t      */
	#define __spinlock_init_fn    /**< spinlock_init()    */
	#define __spinlock_lock_fn    /**< spinlock_lock()    */
	#define __spinlock_trylock_fn /**< spinlock_trylock() */
	#define __spinlock_unlock_fn  /**< spinlock_unlock()  */
	/**@}*/
	/**@endcond*/

	/**
	 * @see OR1K_SPINLOCK_UNLOCKED
	 *
	 * @cond ork1
	 */
	#define SPINLOCK_UNLOCKED OR1K_SPINLOCK_UNLOCKED
	/**@endcond*/

	/**
	 * @see OR1K_SPINLOCK_LOCKED
	 *
	 * @cond ork1
	 */
	#define SPINLOCK_LOCKED OR1K_SPINLOCK_LOCKED
	/**@endcond*/

	/**
	 * @see ork1_spinlock_init()
	 *
	 * @cond ork1
	 */
	static inline void spinlock_init(spinlock_t *lock)
	{
		((void) lock);
	}
	/**@endcond*/

	/**
	 * @see ork1_spinlock_trylock()
	 *
	 * @cond ork1
	 */
	static inline int spinlock_trylock(spinlock_t *lock)
	{
		((void) lock);
		return (OR1K_SPINLOCK_LOCKED);
	}
	/**@endcond*/

	/**
	 * @see ork1_spinlock_lock()
	 *
	 * @cond ork1
	 */
	static inline void spinlock_lock(spinlock_t *lock)
	{
		((void) lock);
	}
	/**@endcond*/

	/**
	 * @see ork1_spinlock_unlock()
	 *
	 * @cond ork1
	 */
	static inline void spinlock_unlock(spinlock_t *lock)
	{
		((void) lock);
	}
	/**@endcond*/

/**@}*/

#endif /* ARCH_OR1K_SPINLOCK_H_ */

