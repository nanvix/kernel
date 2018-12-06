/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
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
#include <nanvix/klib.h>
#include <errno.h>

/**
 * @brief Hardware interrupts table.
 */
PRIVATE struct
{
	int handled; /**< Handled? */
} interrupts[HAL_INT_NR];

/**
 * @brief Default hardware interrupt handler.
 *
 * @param num Number of triggered interrupt.
 */
PRIVATE void default_handler(int num)
{
	UNUSED(num);

	noop();
}

/**
 * The interrupt_register() function registers @p handler as the
 * handler function for the interrupt whose number is @p num. If a
 * handler function was previously registered with this number, the
 * interrupt_register() function fails.
 */
PUBLIC int interrupt_register(int num, hal_interrupt_handler_t handler)
{
	KASSERT(num >= 0);

	/* Handler function already registered. */
	if (interrupts[num].handled)
		return (-EBUSY);
	
	interrupts[num].handled = TRUE;
	hal_dcache_invalidate();
	hal_interrupt_set_handler(num, handler);

	kprintf("interrupt handler registered for irq %d", num);
	
	return (0);
}


/**
 * Initializes hardware interrupts by registering a default handler to
 * all available interrupts.
 */
PUBLIC void interrupt_setup(void)
{
	for (int i = 0; i < HAL_INT_NR; i++)
	{
		interrupts[i].handled = FALSE;
		hal_dcache_invalidate();	
		hal_interrupt_set_handler(i, default_handler);
	}

	kputs("initializing interrupts\n");
}

