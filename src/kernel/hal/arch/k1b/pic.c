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

#include <arch/core/k1b/pic.h>
#include <nanvix/const.h>

/**
 * Lookup table for masks of interrupt levels.
 */
PUBLIC uint32_t intlvl_masks[K1B_NUM_INTLVL] = {
	K1B_INTLVL_MASK_0,
	K1B_INTLVL_MASK_1,
	K1B_INTLVL_MASK_2,
	K1B_INTLVL_MASK_3,
	K1B_INTLVL_MASK_4,
	K1B_INTLVL_MASK_5,
	K1B_INTLVL_MASK_6,
	K1B_INTLVL_MASK_7,
	K1B_INTLVL_MASK_8,
	K1B_INTLVL_MASK_9,
	K1B_INTLVL_MASK_10,
	K1B_INTLVL_MASK_11,
	K1B_INTLVL_MASK_12,
	K1B_INTLVL_MASK_13,
	K1B_INTLVL_MASK_14,
	K1B_INTLVL_MASK_15
};

/**
 * Lookup table for interrupt request lines of hardware interrupts.
 */
PUBLIC k1b_irq_t k1b_irqs[K1B_NUM_IRQ] = {
	K1B_IRQ_0,
	K1B_IRQ_1,
	K1B_IRQ_2,
	K1B_IRQ_3,
	K1B_IRQ_4,
	K1B_IRQ_5,
	K1B_IRQ_6,
	K1B_IRQ_7,
	K1B_IRQ_8,
	K1B_IRQ_9,
#ifdef __k1io__
	K1B_IRQ_10,
	K1B_IRQ_11,
	K1B_IRQ_12
#endif
};

/**
 * Current interrupt mask of the underlying k1b core.
 */
PUBLIC uint32_t currmask = K1B_INTLVL_MASK_5;

/**
 * Current interrupt level of the underlying k1b core.
 */
PUBLIC int currlevel = K1B_INTLVL_0;
