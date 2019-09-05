/*
 * MIT License
 *
 * Copyright(c) 2011-2019 The Maintainers of Nanvix
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
 * @defgroup kernel-mailbox Mailbox Facility
 * @ingroup kernel
 *
 * @brief Mailbox Facility
 */

#ifndef NANVIX_MAILBOX_H_
#define NANVIX_MAILBOX_H_

	#include <nanvix/hal/hal.h>
	#include <nanvix/const.h>
	#include <posix/stdarg.h>

	/**
	 * @name Requests for do_mailbox_ioctl()
	 */
	/**@{*/
	#define MAILBOX_IOCTL_GET_VOLUME  1 /**< Get the amount of data transferred so far. */
	#define MAILBOX_IOCTL_GET_LATENCY 2 /**< Get the cumulative transfer latency.       */
	/**@}*/

	/**
	 * @brief Creates a mailbox.
	 *
	 * @param local Logic ID of the Local Node.
	 *
	 * @returns Upon successful completion, the ID of the newly created
	 * mailbox is returned. Upon failure, a negative error code is
	 * returned instead.
	 */
	EXTERN int do_mailbox_create(int local);

	/**
	 * @brief Opens a mailbox.
	 *
	 * @param remote Logic ID of the Target Node.
	 *
	 * @returns Upon successful completion, the ID of the target mailbox
	 * is returned. Upon failure, a negative error code is returned
	 * instead.
	 */
	EXTERN int do_mailbox_open(int remote);

	/**
	 * @brief Destroys a mailbox.
	 *
	 * @param mbxid ID of the Target Mailbox.
	 *
	 * @returns Upon successful completion, zero is returned. Upon failure,
	 * a negative error code is returned instead.
	 */
	EXTERN int do_mailbox_unlink(int mbxid);

	/**
	 * @brief Closes a mailbox.
	 *
	 * @param mbxid ID of the Target Mailbox.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int do_mailbox_close(int mbxid);

	/**
	 * @brief Reads data from a mailbox.
	 *
	 * @param mbxid  ID of the Target Mailbox.
	 * @param buffer Buffer where the data should be written to.
	 * @param size   Number of bytes to read.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int do_mailbox_aread(int mbxid, void * buffer, size_t size);

	/**
	 * @brief Writes data to a mailbox.
	 *
	 * @param mbxid ID of the Target Mailbox.
	 * @param buffer   Buffer where the data should be read from.
	 * @param size     Number of bytes to write.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int do_mailbox_awrite(int mbxid, const void * buffer, size_t size);

	/**
	 * @brief Waits for an asynchronous operation on a mailbox to complete.
	 *
	 * @param mbxid ID of the Target Mailbox.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int do_mailbox_wait(int mbxid);

	/**
	 * @brief Performs control operations in a mailbox.
	 *
	 * @param mbxid   Target mailbox.
	 * @param request Request.
	 * @param args    Additional arguments.
	 *
	 * @param Upon successful completion, zero is returned. Upon failure,
	 * a negative error code is returned instead.
	 */
	EXTERN int do_mailbox_ioctl(int mbxid, unsigned request, va_list args);

#endif /* NANVIX_MAILBOX_H_ */

/**@}*/
