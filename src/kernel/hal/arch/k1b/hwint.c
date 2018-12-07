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

#include <arch/k1b/int.h>
#include <arch/k1b/int.h>
#include <arch/k1b/ivt.h>
#include <arch/k1b/util.h>
#include <nanvix/const.h>
#include <nanvix/hal/memory.h>
#include <nanvix/hal/interrupt.h>
#include <nanvix/klib.h>
#include <errno.h>

/**
 * @brief Interrupt handlers.
 */
PRIVATE hal_interrupt_handler_t k1b_handlers[K1B_NUM_HWINT] = {
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

/**
 * The k1b_do_hwint() function dispatches a hardware interrupt that
 * was triggered to a specific hardware interrupt handler. The ID of
 * hardware interrupt is used to index the table of hardware interrupt
 * handlers, and the context and the interrupted context pointed to by
 * ctx is currently unsed.
 */
PUBLIC void k1b_do_hwint(k1b_hwint_id_t hwintid, struct k1b_context *ctx)
{
	int num = 0;

	UNUSED(ctx);

	/* Get interrupt number. */
	for (int i = 0; i < K1B_NUM_HWINT; i++)
	{
		if (hwints[i] == hwintid)
		{
			num = i;
			goto found;
		}
	}

	return;

found:

	k1b_handlers[num](num);
}

/**
 * The hal_interrupt_set_handler() function sets the function pointed
 * to by @p handler as the handler for the hardware interrupt whose
 * number is @p num.
 */
PUBLIC void hal_interrupt_set_handler(int num, hal_interrupt_handler_t handler)
{
	k1b_handlers[num] = handler;
	hal_dcache_invalidate();
}
