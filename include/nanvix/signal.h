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

/**
 * @defgroup kernel- Signal System
 * @ingroup kernel
 *
 * @brief Signal System
 */

#ifndef NANVIX_SIGNAL_H_
#define NANVIX_SIGNAL_H_

	/* External dependencies. */
	#include <nanvix/hal/hal.h>
	#include <nanvix/const.h>

/*============================================================================*
 * Signal System                                                              *
 *============================================================================*/

	/**
	 * @name Signal Types
	 */
	/**@{*/
	#define SIGOPCODE  EXCEPTION_INVALID_OPCODE     /**< Invalid opcode signal     */
	#define SIGPGFAULT EXCEPTION_PAGE_FAULT         /**< Page fault signal         */
	#define SIGPGPROT  EXCEPTION_PAGE_PROTECTION    /**< Page protection signal    */
	#define SIGGENPROT EXCEPTION_GENERAL_PROTECTION /**< General protection signal */
	/**@}*/

	/**
	 * @brief Signal Action handler.
	 */
	typedef void (*ksa_handler)(void *);

	/**
	 * @brief Signal Action struct (Partial POSIX specification).
	 */
	struct ksigaction
	{
		ksa_handler handler; /**< Pointer to a signal-catching function. */
	};

	/**
	 * @brief Controls the behavior of a signal.
	 *
	 * @param signum Signal ID.
	 * @param sigact Behavior descriptor.
	 *
	 * @returns Zero if successfully changes the behavior, non zero otherwise.
	 */
	EXTERN int signal_control(int signum, struct ksigaction *sigact);

	/**
	 * @brief Schedules an alarm signal.
	 *
	 * The alarm() function schedule an alarm signal to trigger when
	 * the @seconds seconds pass.
	 *
	 * @param seconds Time in seconds.
	 *
	 * @returns Zero if successfully register the alarm, non zero otherwise.
	 *
	 * @todo: TODO implement this function.
	 */
	static int signal_alarm(int seconds)
	{
		UNUSED(seconds);

		// TODO: implement this function.

		return (0);
	}

	/**
	 * @brief Sends a signal.
	 *
	 * The signal_send() function sends a signal @signum to another thread @tid.
	 *
	 * @param signum Signal ID.
	 * @param tid    Thread ID.
	 *
	 * @returns Zero if successfully sends the signal, non zero otherwise.
	 *
	 *
	 * @todo: TODO implement this function.
	 */
	static int signal_send(int signum, int tid)
	{
		UNUSED(signum);
		UNUSED(tid);

		// TODO: implement this function.

		return (0);
	}

	/**
	 * @brief Waits for the receipt of a signal.
	 *
	 * The signal_wait() function waits for the receipt of a @signum signal.
	 *
	 * @param signum Signal ID.
	 *
	 * @returns Zero if successfully receives the signal, non zero otherwise.
	 *
	 * @todo: TODO implement this function.
	 */
	static inline int signal_wait(int signum)
	{
		UNUSED(signum);

		// TODO: implement this function.

		return (0);
	}

	/**
	 * @brief Returns from a signal handler.
	 *
	 * The signal_return() function returns from a signal handler, restoring the
	 * execution stream.
	 *
	 * @todo: TODO implement this function.
	 */
	static inline void signal_return(void)
	{

		// TODO: implement this function.
	}

#endif /* NANVIX_SIGNAL_H_ */

/**@}*/
