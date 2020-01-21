/*
 * MIT License
 *
 * Copyright(c) 2011-2020 The Maintainers of Nanvix
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

#ifndef NANVIX_KERNEL_EXCP_H_
#define NANVIX_KERNEL_EXCP_H_

	#include <nanvix/kernel/thread.h>

#if (THREAD_MAX > 1)

	#include <nanvix/const.h>
	#include <nanvix/hal.h>

	/**
	 * @name Actions Upon an Exception
	 */
	/**@{*/
	#define EXCP_ACTION_IGNORE 0 /**< Igore  */
	#define EXCP_ACTION_HANDLE 1 /**< Handle */
	/**@}*/

#ifdef __NANVIX_MICROKERNEL

	/**
	 * @brief Asserts if an exception is valid.
	 *
	 * @param x Number of target exception.
	 *
	 * @returns One if the execption @p x is valid and zero otherwise.
	 */
	#define exception_is_valid(x) \
		(((x) >= 0) && ((x) < EXCEPTIONS_NUM))

	/**
	 * @brief Initializes the user-space exception module.
	 */
	EXTERN void exception_init(void);

	/**
	 * @brief Sets a user-space exception handler.
	 *
	 * @param excpnum Number of the target exception.
	 * @param action  Action upon target exception.
	 *
	 * @return Upon successful completion, zero is returned. Upon
	 * failure a negative error code is returned instead.
	 */
	EXTERN int exception_control(int excpnum, int action);

	/**
	 * @brief Pauses the user-space exception handler.
	 *
	 * @param excp Information about the exception
	 *
	 * @return Upon successful completion, zero is returned. Upon
	 * failure a negative error code is returned instead.
	 */
	EXTERN int exception_pause(struct exception *excp);

	/**
	 * @brief Waits for a user-space exception handler.
	 *
	 * @param excpnum Number of the target exception.
	 * @param excp    Information about the exception.
	 *
	 * @return Upon successful completion, zero is returned. Upon
	 * failure a negative error code is returned instead.
	 */
	EXTERN int exception_wait(int excpnum, const struct exception *excp);

	/**
	 * @brief Resumes a kernel-space exception handler.
	 *
	 * @return Upon successful completion, zero is returned. Upon
	 * failure a negative error code is returned instead.
	 */
	EXTERN int exception_resume(void);

#endif /* __NANVIX_MICROKERNEL */

#endif /* THREAD_MAX > 1*/

#endif /* NANVIX_KERNEL_EXCP_H_ */
