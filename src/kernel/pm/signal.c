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

#include <nanvix/hal/hal.h>
#include <nanvix/const.h>
#include <nanvix/signal.h>
#include <errno.h>


/**
 * @brief Information about signal.
 */
EXTENSION PRIVATE struct signal_info
{
	sa_handler handler; /**< Handler */
} ALIGN(sizeof(dword_t)) signals[EXCEPTIONS_NUM] = {
	[0 ... (EXCEPTIONS_NUM - 1)] = {
		.handler = NULL
	},
};

/**
 * @brief Signal handler
 */
PRIVATE void signal_handler(
	const struct exception *excp,
	const struct context *ctx
)
{
	word_t signum;

	signum = excp->num;

	if (signals[signum].handler)
	{
		/* Forge upcall. */
		upcall_forge(
			(struct context *) ctx,
			signals[signum].handler,
			&signum,
			sizeof(word_t)
		);
	}
}

/*============================================================================*
 * sigclt()                                                                   *
 *============================================================================*/

/**
 * @brief Controls the behavior of a signal.
 *
 * The sigclt() function modifies the treatment of a signal.
 *
 * @param signum Signal ID.
 * @param sigact Behavior descriptor.
 *
 * @returns Zero if successfully changes the behavior, non zero otherwise.
 */
PUBLIC int sigclt(int signum, struct sigaction * sigact)
{
	int ret;

	/* Invalid signal ID. */
	if ((signum < 0) || (signum >= EXCEPTIONS_NUM))
		return (-EINVAL);

	/* Unchanged the signal. */
	if (sigact == NULL)
		return (-EAGAIN);

	if (sigact->handler != NULL)
	{
		ret = 0;

		if (signals[signum].handler == NULL)
			ret = exception_register(signum, signal_handler);
	}
	else
		ret = exception_unregister(signum);

	if (ret != 0)
		return (ret);

	signals[signum].handler = sigact->handler;

	dcache_invalidate();

	return 0;
}
