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

#include <arch/core/or1k/int.h>
#include <nanvix/const.h>
#include <nanvix/klib.h>

/**
 * @brief Interrupt handlers.
 */
PRIVATE void (*or1k_handlers[OR1K_NUM_HWINT])(int) = {
	NULL, NULL, NULL
};

/**
 * @brief Gets the next irq pending to be served.
 *
 * @returns Returns the irq number available or 0
 * if there's no one irq pending.
 */
PRIVATE int or1k_next_irq()
{
	unsigned picsr;
	int bit;

	bit   = 0;
	picsr = or1k_mfspr(OR1K_SPR_PICSR);

	while (!(picsr & 1) && bit < 32)
	{
		picsr >>= 1;	
		bit++;
	}

	return ( (!picsr) ? 0 : bit );
}


/**
 * @brief High-level hardware interrupt dispatcher.
 *
 * The do_hwint() function dispatches a hardware interrupt request
 * that was triggered to a previously-registered handler. If no
 * function was previously registered to handle the triggered hardware
 * interrupt request, this function returns immediately.
 *
 * @param num Interrupt request.
 * @param ctx Interrupted execution context.
 *
 * @note This function is called from assembly code.
 */
PUBLIC void or1k_do_hwint(int num, const struct context *ctx)
{
	int interrupt;
	UNUSED(ctx);
	
	/* 
	 * If clock, lets handle immediately.
	 *
	 * Since clock interrupts do not use the PIC,
	 * we cannnot use the approach below to find
	 * a clock int.
	 */
	if (num == OR1K_INT_CLOCK)
	{
		/* ack. */
		or1k_pic_ack(num);

		/* Nothing to do. */
		if (or1k_handlers[num] == NULL)
			return;

		or1k_handlers[num](num);
	}
	
	/*
	 * Lets also check for external interrupt, if
	 * there's any pending, handle.
	 */
	while ((interrupt = or1k_next_irq()) != 0)
	{
		/* ack. */
		or1k_pic_ack(interrupt);
		
		/* Nothing to do. */
		if (or1k_handlers[interrupt] == NULL)
			return;

		or1k_handlers[interrupt](interrupt);
	}
}

/**
 * The or1k_hwint_handler_set() function sets the function pointed to
 * by @p handler as the handler for the hardware interrupt whose
 * number is @p num.
 */
PUBLIC void or1k_hwint_handler_set(int num, void (*handler)(int))
{
	or1k_handlers[num] = handler;
}
