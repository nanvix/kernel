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

#ifndef ARCH_K1B_PIC_H_
#define ARCH_K1B_PIC_H_

/**
 * @addtogroup k1b-pic Programmable Interrupt Controller
 *
 * @brief Interface for dealing with the programmble interrupt controller.
 */

/**@{*/

	#include <nanvix/const.h>
	#include <mOS_vcore_u.h>
	#include <stdint.h>

	/**
	 * @brief Number of interrupt levels.
	 */
	#define K1B_NUM_INTLVL 16

	/**
	 * @name Interrupt Levels
	 */
	/**@{*/
	#define K1B_INTLVL_0   0 /**< Interrupt level 0.  */
	#define K1B_INTLVL_1   1 /**< Interrupt level 1.  */
	#define K1B_INTLVL_2   2 /**< Interrupt level 2.  */
	#define K1B_INTLVL_3   3 /**< Interrupt level 3.  */
	#define K1B_INTLVL_4   4 /**< Interrupt level 4.  */
	#define K1B_INTLVL_5   5 /**< Interrupt level 5.  */
	#define K1B_INTLVL_6   6 /**< Interrupt level 6.  */
	#define K1B_INTLVL_7   7 /**< Interrupt level 7.  */
	#define K1B_INTLVL_8   8 /**< Interrupt level 8.  */
	#define K1B_INTLVL_9   9 /**< Interrupt level 9.  */
	#define K1B_INTLVL_10 10 /**< Interrupt level 10. */
	#define K1B_INTLVL_11 11 /**< Interrupt level 11. */
	#define K1B_INTLVL_12 12 /**< Interrupt level 12. */
	#define K1B_INTLVL_13 13 /**< Interrupt level 13. */
	#define K1B_INTLVL_14 14 /**< Interrupt level 14. */
	#define K1B_INTLVL_15 15 /**< Interrupt level 15. */
	/**@}*/

	/**
	 * @name Interrupt Levels Masks
	 */
	/**@{*/
	#define K1B_INTLVL_MASK_0  0x0 /**< Mask for interrupt level 0.  */
	#define K1B_INTLVL_MASK_1  0x1 /**< Mask for interrupt level 1.  */
	#define K1B_INTLVL_MASK_2  0x2 /**< Mask for interrupt level 2.  */
	#define K1B_INTLVL_MASK_3  0x3 /**< Mask for interrupt level 3.  */
	#define K1B_INTLVL_MASK_4  0x4 /**< Mask for interrupt level 4.  */
	#define K1B_INTLVL_MASK_5  0x5 /**< Mask for interrupt level 5.  */
	#define K1B_INTLVL_MASK_6  0x6 /**< Mask for interrupt level 6.  */
	#define K1B_INTLVL_MASK_7  0x7 /**< Mask for interrupt level 7.  */
	#define K1B_INTLVL_MASK_8  0x8 /**< Mask for interrupt level 8.  */
	#define K1B_INTLVL_MASK_9  0x9 /**< Mask for interrupt level 9.  */
	#define K1B_INTLVL_MASK_10 0xa /**< Mask for interrupt level 10. */
	#define K1B_INTLVL_MASK_11 0xb /**< Mask for interrupt level 11. */
	#define K1B_INTLVL_MASK_12 0xc /**< Mask for interrupt level 12. */
	#define K1B_INTLVL_MASK_13 0xd /**< Mask for interrupt level 13. */
	#define K1B_INTLVL_MASK_14 0xe /**< Mask for interrupt level 14. */
	#define K1B_INTLVL_MASK_15 0xf /**< Mask for interrupt level 15. */
	/**@}*/

	/**
	 * @brief Masks of interrupt levels.
	 */
	EXTERN uint32_t intlvl_masks[K1B_NUM_INTLVL];

	/**
	 * @brief Raises the interrupt level of the calling core.
	 *
	 * The k1b_pic_raise() function raises the interrupt level of the
	 * calling core to @p newlevel. The old interrupt level is
	 * returned.
	 *
	 * @param newlevel New interrupt level.
	 *
	 * @returns The old interrupt level.
	 */
	static inline int k1b_pic_raise(int newlevel)
	{
		uint32_t mask;

		mask = mOS_set_it_level(intlvl_masks[newlevel]);

		/* Query interrupt level. */
		for (int i = 0; i < K1B_NUM_INTLVL; i++)
		{
			if (intlvl_masks[i] == mask)
				return (i);
		}

		return (0);
	}

	/**
	 * @brief Drops the interrupt level of the calling core.
	 *
	 * The k1b_pic_drop() function drops the interrupt level of the
	 * calling core to @p newlevel.
	 */
	static inline void k1b_pic_drop(int newlevel)
	{
		mOS_set_it_level(intlvl_masks[newlevel]);
	}

	/**
	 * @brief Initializes the PIC.
	 *
	 * The k1b_pic_setup() initializes the PIC. Upon completion, it
	 * drops the interrupt level to the slowest ones, so that all
	 * interrupt lines are enabled.
	 */
	static inline void k1b_pic_setup(void)
	{
		k1b_pic_drop(K1B_INTLVL_0);
	}

/**@}*/

#endif /* ARCH_K1B_PIC_H_ */
