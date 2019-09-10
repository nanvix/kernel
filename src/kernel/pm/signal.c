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
#include <nanvix/kernel/mm.h>
#include <nanvix/kernel/signal.h>
#include <nanvix/const.h>
#include <posix/errno.h>

/**
 * @brief Signal Handlers Table lock.
 */
spinlock_t sigtab_lock = SPINLOCK_UNLOCKED;

/**
 * @brief Table information about signals.
 */
EXTENSION PRIVATE struct signal_info
{
	ksa_handler handler; /**< Signal handler */
} ALIGN(CACHE_LINE_SIZE) signals[EXCEPTIONS_NUM] = {
	[0 ... (EXCEPTIONS_NUM - 1)] = { .handler = NULL },
};

/*============================================================================*
 * signal_handler()                                                           *
 *============================================================================*/

/**
 * @brief Wrapper to receive exceptions passed by the HAL.
 *
 * The signal_handler() function deal with exceptions from HAL and
 * pass them to userspace by upcall.
 *
 * @param excp Exception emitted.
 * @param ctx  Interrupted context.
 */
PRIVATE void signal_handler(
	const struct exception *excp,
	const struct context *ctx
)
{
	dword_t signum;

	signum = excp->num;

	spinlock_lock(&sigtab_lock);

		/* Concurrent anomaly? */
		if (signals[signum].handler == NULL)
		{
			spinlock_unlock(&sigtab_lock);

			kpanic("cannot forward signal to unknown handler");

			UNREACHABLE();
		}

		ksa_handler handler = signals[signum].handler;

	spinlock_unlock(&sigtab_lock);

	/* Forge upcall. */
	upcall_forge(
		(struct context *) ctx,
		handler,
		&signum,
		sizeof(dword_t)
	);
}

/*============================================================================*
 * signal_control()                                                           *
 *============================================================================*/

/**
 * The signal_control() function modifies the treatment of a signal.
 */
PUBLIC int signal_control(int signum, struct ksigaction *sigact)
{
	int ret;
	int without_handler;

	/*
	 * Invalid signal ID.
	 *
	 * FIXME: we should only allow values
	 * for exceptions that are mapped into signals.
	 */
	if (!WITHIN(signum, 0, EXCEPTIONS_NUM))
		return (-EINVAL);

	/* Unchanged the signal. */
	if (sigact == NULL)
		return (-EAGAIN);

	/* Registration operation? */
	if (sigact->handler != NULL)
	{
		ret = 0;

		spinlock_lock(&sigtab_lock);

			without_handler = (signals[signum].handler == NULL);

		spinlock_unlock(&sigtab_lock);

		if (without_handler)
			ret = exception_register(signum, signal_handler);
	}
	else
		ret = exception_unregister(signum);

	/* Did it fail in some operation? */
	if (ret != 0)
		return (ret);

	spinlock_lock(&sigtab_lock);

		/* Sets signal handler */
		signals[signum].handler = sigact->handler;

	spinlock_unlock(&sigtab_lock);

	return (0);
}
