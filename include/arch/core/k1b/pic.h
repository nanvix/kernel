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

#ifndef ARCH_CORE_K1B_PIC_H_
#define ARCH_CORE_K1B_PIC_H_

/**
 * @addtogroup k1b-core-pic PIC
 * @ingroup k1b-core
 *
 * @brief Programmble Interrupt Controller
 */
/**@{*/

	#include <mOS_vcore_u.h>
	#include <stdint.h>

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_intlvl_set
	#define __hal_interrupt_ack
	#define __hal_interrupt_mask
	#define __hal_interrupt_unmask
	/**@}*/

	/**
	 * @brief Number of interrupt request lines.
	 */
	#ifdef __k1io__
		#define K1B_NUM_IRQ 13
	#else
		#define K1B_NUM_IRQ 10
	#endif

	/**
	 * @name Interrupt Request Lines
	 */
	/**@{*/
		#define K1B_IRQ_0 MOS_VC_IT_TIMER_0           /**< Timer 0           */
		#define K1B_IRQ_1 MOS_VC_IT_TIMER_1           /**< Timer 1           */
		#define K1B_IRQ_2 MOS_VC_IT_WATCHDOG          /**< Watchdog Timer    */
		#define K1B_IRQ_3 MOS_VC_IT_MAILBOX           /**< C-NoC Mailbox     */
		#define K1B_IRQ_4 MOS_VC_IT_DNOC_RX           /**< D-NoC RX          */
		#define K1B_IRQ_5 MOS_VC_IT_UCORE             /**< U-Core            */
		#define K1B_IRQ_6 MOS_VC_IT_NOCERR            /**< NoC Error         */
		#define K1B_IRQ_7 MOS_VC_IT_USER_0            /**< Virtual Line      */
		#define K1B_IRQ_8 MOS_VC_IT_WATCHDOG_OVERFLOW /**< Watchdog Overflow */
		#define K1B_IRQ_9 MOS_VC_IT_DEBUG             /**< Debug             */
	#ifdef __k1io__
		#define K1B_IRQ_10 MOS_VC_IT_GIC_1            /**< GIC 1             */
		#define K1B_IRQ_11 MOS_VC_IT_GIC_2            /**< GIC 2             */
		#define K1B_IRQ_12 MOS_VC_IT_GIC_3            /**< GIC2              */
	#endif
	/**@}*/

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
	 * @brief Interrupt request line number.
	 */
	typedef mOS_vcore_it_lines_e k1b_irq_t;

	/**
	 * @brief Masks of interrupt levels.
	 */
	extern uint32_t intlvl_masks[K1B_NUM_INTLVL];

	/**
	 * @brief Interrupt request lines.
	 */
	extern k1b_irq_t k1b_irqs[K1B_NUM_IRQ];

	/**
	 * @brief Current interrupt mask.
	 */
	extern uint32_t currmask;

	/**
	 * @brief Current interrupt level.
	 */
	extern int currlevel;

	/**
	 * @brief Sets the interrupt level of the underlying core.
	 *
	 * @param newlevel New interrupt level.
	 *
	 * @returns The old interrupt level.
	 */
	static inline int k1b_pic_lvl_set(int newlevel)
	{
		uint32_t mask;
		int oldlevel;

		mOS_set_it_level(mask = intlvl_masks[newlevel]);

		currmask = mask;
		oldlevel = currlevel;
		currlevel = newlevel;

		return (oldlevel);
	}

	/**
	 * @see k1b_pic_lvl_set()
	 *
	 * @cond k1b
	 */
	static inline int hal_intlvl_set(int newlevel)
	{
		return (k1b_pic_lvl_set(newlevel));
	}
	/**@endcond*/

	/**
	 * @brief Acknowledges an interrupt.
	 *
	 * The k1b_pic_ack() function acknowledges the end of interrupt to
	 * the interrupt controller of the underlying k1b core.
	 *
	 * @param intnum Number of the target interrupt.
	 */
	static inline void k1b_pic_ack(int intnum)
	{
		((void) intnum);
	}

	/**
	 * @see k1b_pic_ack()
	 *
	 * @cond k1b
	 */
	static inline void hal_interrupt_ack(int intnum)
	{
		k1b_pic_ack(intnum);
	}
	/**@endcond*/

	/**
	 * @brief Masks an interrupt.
	 *
	 * The k1b_pic_mask() function masks the interrupt line in which
	 * the interrupt @p intnum is hooked up in the underlying k1b
	 * core.
	 *
	 * @param intnum Number of the target interrupt.
	 */
	static inline void k1b_pic_mask(int intnum)
	{
		mOS_it_enable_num(k1b_irqs[intnum]);
	}

	/**
	 * @see k1b_pic_mask()
	 *
	 * @cond k1b
	 */
	static inline void hal_interrupt_mask(int intnum)
	{
		k1b_pic_mask(intnum);
	}
	/**@endcond*/

	/**
	 * @brief Masks an interrupt.
	 *
	 * The k1b_pic_unmask() function unmasks the interrupt line in which
	 * the interrupt @p intnum is hooked up in the underlying k1b
	 * core.
	 *
	 * @param intnum Number of the target interrupt.
	 */
	static inline void k1b_pic_unmask(int intnum)
	{
		mOS_it_disable_num(k1b_irqs[intnum]);
	}

	/**
	 * @see k1b_pic_unmask()
	 *
	 * @cond k1b
	 */
	static inline void hal_interrupt_unmask(int intnum)
	{
		k1b_pic_unmask(intnum);
	}
	/**@endcond*/

	/**
	 * @brief Initializes the PIC.
	 *
	 * The k1b_pic_setup() function initializes the programmble
	 * interrupt controler of the k1b core. Upon completion, it drops
	 * the interrupt level to the slowest ones, so that all interrupt
	 * lines are enabled.
	 */
	static inline void k1b_pic_setup(void)
	{
		k1b_pic_lvl_set(K1B_INTLVL_0);
	}

/**@}*/

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

#endif /* ARCH_CORE_K1B_PIC_H_ */
