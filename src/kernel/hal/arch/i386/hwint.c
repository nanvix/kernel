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

#include <arch/core/i386/context.h>
#include <arch/core/i386/int.h>
#include <nanvix/const.h>
#include <nanvix/klib.h>

/**
 * @brief Interrupt handlers.
 */
PRIVATE void (*i386_handlers[I386_NUM_HWINT])(int) = {
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};

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
PUBLIC void i386_do_hwint(int num, const struct context *ctx)
{
	UNUSED(ctx);
	
	/* Nothing to do. */
	if (i386_handlers[num] == NULL)
		return;

	i386_handlers[num](num);
}

/**
 * The i386_hwint_handler_set() function sets the function pointed to
 * by @p handler as the handler for the hardware interrupt whose
 * number is @p num.
 */
PUBLIC void i386_hwint_handler_set(int num, void (*handler)(int))
{
	i386_handlers[num] = handler;
}
