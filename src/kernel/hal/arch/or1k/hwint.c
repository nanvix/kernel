/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2017-2018 Davidson Francis     <davidsondfgl@gmail.com>
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

#include <nanvix/const.h>
#include <nanvix/hal/interrupt.h>

/**
 * @brief Interrupt handlers.
 */
PRIVATE hal_interrupt_handler_t or1k_handlers[OR1K_NUM_HWINT] = 
	{ NULL, NULL, NULL };

/**
 * @brief Hardware interrupt dispatcher.
 *
 * @param irq Interrupt request.
 *
 * @note This function is called from assembly code.
 */
PUBLIC void do_hwint(int irq)
{
	/* Nothing to do. */
	if (or1k_handlers[irq] == NULL)
		return;

	or1k_handlers[irq](irq);
}

/**
 * The hal_interrupt_set_handler() function sets the function pointed
 * to by @p handler as the handler for the hardware interrupt whose
 * number is @p num.
 */
PUBLIC void hal_interrupt_set_handler(int num, hal_interrupt_handler_t handler)
{
	or1k_handlers[num] = handler;
}
