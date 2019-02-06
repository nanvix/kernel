/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2018-2018 Davidson Francis <davidsondfgl@gmail.com>
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

/**
 * @addtogroup or1k-PIC PIC Chip
 * @ingroup or1k
 *
 * @brief Programmable Interrupt Controller
 */
/**@{*/

#ifndef ARCH_OR1K_PIC_H_
#define ARCH_OR1K_PIC_H_

	/**
	 * @name Provided Interface
	 */
	/**@{*/
	#define __hal_intlvl_set
	#define __hal_interrupt_mask
	#define __hal_interrupt_unmask
	#define __hal_interrupt_ack
	/**@}*/

	/**
	 * @brief Number of interrupt levels.
	 */
	#define OR1K_NUM_INTLVL 6

	/**
	 * @name Interrupt Levels
	 */
	/**@{*/
	#define OR1K_INTLVL_5 5 /**< Level 5: all hardware interrupts enabled.         */
	#define OR1K_INTLVL_4 4 /**< Level 4: all hardware interrupts enabled.         */
	#define OR1K_INTLVL_3 3 /**< Level 3: clock, ompic, serial interrupts enabled. */
	#define OR1K_INTLVL_2 2 /**< Level 2: clock, ompic interrupts enabled.         */
	#define OR1K_INTLVL_1 1 /**< Level 1: clock interrupts enabled.                */
	#define OR1K_INTLVL_0 0 /**< Level 0: all hardware interrupts disabled.        */
	/**@}*/

	/**
	 * @name Interrupt Levels Masks
	 */
	/**@{*/
	#define OR1K_INTLVL_MASK_5 0x0006 /**< Mask for interrupt level 5.  */
	#define OR1K_INTLVL_MASK_4 0x0006 /**< Mask for interrupt level 4.  */
	#define OR1K_INTLVL_MASK_3 0x0006 /**< Mask for interrupt level 3.  */
	#define OR1K_INTLVL_MASK_2 0x0002 /**< Mask for interrupt level 2.  */
	#define OR1K_INTLVL_MASK_1 0x0001 /**< Mask for interrupt level 1.  */
	#define OR1K_INTLVL_MASK_0 0x0000 /**< Mask for interrupt level 0.  */
	/**@}*/

#ifndef _ASM_FILE_

	#include <arch/or1k/core.h>
	#include <target/or1k/pc.h>
	#include <nanvix/const.h>
	#include <stdint.h>

	/**
	 * @brief Masks of interrupt levels.
	 */
	EXTERN uint32_t intlvl_masks[OR1K_NUM_INTLVL];

	/**
	 * @brief Current interrupt mask.
	 */
	EXTERN uint32_t currmask;

	/**
	 * @brief Current interrupt level.
	 */
	EXTERN int currlevel;

	/**
	 * @brief Sets the interrupt level of the calling core.
	 *
	 * @param newlevel New interrupt level.
	 *
	 * @returns The old interrupt level.
	 */
	static inline int or1k_pic_lvl_set(int newlevel)
	{
		uint32_t mask;
		int oldlevel;

		mask = intlvl_masks[newlevel];
		or1k_mtspr(OR1K_SPR_PICMR, mask);

		/* Check if timer should be masked. */
		if (newlevel == OR1K_INTLVL_0)
			or1k_mtspr(OR1K_SPR_SR, or1k_mfspr(OR1K_SPR_SR) & ~OR1K_SPR_SR_TEE);
		else
			or1k_mtspr(OR1K_SPR_SR, or1k_mfspr(OR1K_SPR_SR) |  OR1K_SPR_SR_TEE);

		currmask = mask;
		oldlevel = currlevel;
		currlevel = newlevel;

		return (oldlevel);
	}

	/**
	 * @see or1k_pic_lvl_set()
	 *
	 * @cond or1k
	 */
	static inline int hal_intlvl_set(int newlevel)
	{
		return (or1k_pic_lvl_set(newlevel));
	}
	/**@endcond*/

	/**
	 * @brief Acknowledges an interrupt.
	 *
	 * @param intnum Number of the target interrupt.
	 */
	static inline void or1k_pic_ack(int intnum)
	{
		if (intnum == OR1K_PC_INT_CLOCK)
			or1k_mtspr(OR1K_SPR_TTMR, or1k_mfspr(OR1K_SPR_TTMR) & ~OR1K_SPR_TTMR_IP);
		else
			or1k_mtspr(OR1K_SPR_PICSR, (1UL << intnum));
	}

	/**
	 * @see or1k_pic_ack()
	 *
	 * @cond or1k
	 */
	static inline void hal_interrupt_ack(int intnum)
	{
		or1k_pic_ack(intnum);
	}
	/**@endcond*/

	/**
	 * @brief Masks an interrupt.
	 *
	 * @param intnum Number of the target interrupt.
	 */
	static inline void or1k_pic_mask(int intnum)
	{
		if (intnum == OR1K_PC_INT_CLOCK)
			or1k_mtspr(OR1K_SPR_SR, or1k_mfspr(OR1K_SPR_SR) & ~OR1K_SPR_SR_TEE);
		else
			or1k_mtspr(OR1K_SPR_PICMR, or1k_mfspr(OR1K_SPR_PICMR) & ~(1 << intnum));
	}

	/**
	 * @see or1k_pic_mask()
	 *
	 * @cond or1k
	 */
	static inline void hal_interrupt_mask(int intnum)
	{
		or1k_pic_mask(intnum);
	}
	/**@endcond*/

	/**
	 * @brief Unmasks an interrupt.
	 *
	 * @param intnum Number of the target interrupt.
	 */
	static inline void or1k_pic_unmask(int intnum)
	{
		or1k_mtspr(OR1K_SPR_PICMR, or1k_mfspr(OR1K_SPR_PICMR) | (1 << intnum));
	}

	/**
	 * @see or1k_pic_unmask()
	 *
	 * @cond or1k
	 */
	static inline void hal_interrupt_unmask(int intnum)
	{
		or1k_pic_unmask(intnum);
	}
	/**@endcond*/

	/**
	 * @brief Initializes the PIC.
	 */
	static inline void or1k_pic_setup(void)
	{
		unsigned upr;

		upr = or1k_mfspr(OR1K_SPR_UPR);
		if ( !(upr & OR1K_SPR_UPR_PICP) )
			while (1);

		/* Unmask serial and ompic device. */
		or1k_pic_mask(OR1K_INTLVL_0);
	}

#endif /* _ASM_FILE_ */

/**@}*/

#endif /* ARCH_OR1K_PIC_H_ */
