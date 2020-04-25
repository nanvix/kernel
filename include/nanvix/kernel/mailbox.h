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

/**
 * @defgroup kernel-mailbox Mailbox Facility
 * @ingroup kernel
 *
 * @brief Mailbox Facility
 */

#ifndef NANVIX_MAILBOX_H_
#define NANVIX_MAILBOX_H_

	#include <nanvix/hal.h>
	#include <nanvix/const.h>
	#include <posix/stdarg.h>

	/**
	 * @name Requests for do_vmailbox_ioctl()
	 */
	/**@{*/
	#define MAILBOX_IOCTL_GET_VOLUME  1 /**< Get the amount of data transferred so far. */
	#define MAILBOX_IOCTL_GET_LATENCY 2 /**< Get the cumulative transfer latency.       */
	/**@}*/

	/**
	 * @brief Number of ports per HW mailbox.
	 *
	 * Maximum number of virtual mailboxes that can be vinculated to each HW mailbox.
	 */
	#define MAILBOX_PORT_NR 16

	/**
	 * @brief Maximum number of HW mailboxes.
	 *
	 * Maximum number of active mailboxes that may be created/opened.
	 *
	 * @note This constant is based on the Hal exported mailboxes plus 1 relative
	 * to when the user open a mailbox to the local node, what is supported by the
	 * kernel but not by the Hal.
	 */
	#define HW_MAILBOX_MAX (HAL_MAILBOX_CREATE_MAX + HAL_MAILBOX_OPEN_MAX + 1)

	/**
	 * @brief Maximum number of virtual mailboxes.
	 *
	 * Maximum number of virtual mailboxes that may be created/opened.
	 */
	#define KMAILBOX_MAX (HW_MAILBOX_MAX * MAILBOX_PORT_NR)

	/**
	 * @brief Mailbox message header size.
	 *
	 * Size of mailbox message header.
	 */
	#define KMAILBOX_MESSAGE_HEADER_SIZE (1 * sizeof(int))

	/**
	 * @brief Maximum number of message buffer resources.
	 *
	 * Maximum number of message buffers used to hold temporary data on kernel space.
	 */
	#define KMAILBOX_MESSAGE_BUFFERS_MAX 64

	/**
	 * @brief Mailbox message buffer max size.
	 *
	 * Maximum size of mailbox message data buffer.
	 */
	#define KMAILBOX_MESSAGE_SIZE (HAL_MAILBOX_MSG_SIZE - KMAILBOX_MESSAGE_HEADER_SIZE)

	/**
	 * @brief Creates a virtual mailbox.
	 *
	 * @param local Logic ID of the local node.
	 * @param port  Target port in @p local node.
	 *
	 * @returns Upon successful completion, the ID of the newly created
	 * mailbox is returned. Upon failure, a negative error code is
	 * returned instead.
	 */
	EXTERN int do_vmailbox_create(int local, int port);

	/**
	 * @brief Opens a virtual mailbox.
	 *
	 * @param remote      Logic ID of the target node.
	 * @param remote_port Target port in @p remote node.
	 *
	 * @returns Upon successful completion, the ID of the target mailbox
	 * is returned. Upon failure, a negative error code is returned
	 * instead.
	 */
	EXTERN int do_vmailbox_open(int remote, int remote_port);

	/**
	 * @brief Destroys a virtual mailbox.
	 *
	 * @param mbxid ID of the target virtual mailbox.
	 *
	 * @returns Upon successful completion, zero is returned. Upon failure,
	 * a negative error code is returned instead.
	 */
	EXTERN int do_vmailbox_unlink(int mbxid);

	/**
	 * @brief Closes a virtual mailbox.
	 *
	 * @param mbxid ID of the target virtual mailbox.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int do_vmailbox_close(int mbxid);

	/**
	 * @brief Reads data from a virtual mailbox.
	 *
	 * @param mbxid  ID of the target virtual mailbox.
	 * @param buffer Buffer where the data should be written to.
	 * @param size   Number of bytes to read.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int do_vmailbox_aread(int mbxid, void * buffer, size_t size);

	/**
	 * @brief Writes data to a virtual mailbox.
	 *
	 * @param mbxid ID of the target virtual mailbox.
	 * @param buffer   Buffer where the data should be read from.
	 * @param size     Number of bytes to write.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int do_vmailbox_awrite(int mbxid, const void * buffer, size_t size);

	/**
	 * @brief Waits for an asynchronous operation on a virtual mailbox.
	 *
	 * @param mbxid ID of the target virtual mailbox.
	 *
	 * @returns Upon successful completion, zero is returned. Upon
	 * failure, a negative error code is returned instead.
	 */
	EXTERN int do_vmailbox_wait(int mbxid);

	/**
	 * @brief Performs control operations in a virtual mailbox.
	 *
	 * @param mbxid   Target mailbox.
	 * @param request Request.
	 * @param args    Additional arguments.
	 *
	 * @returns Upon successful completion, zero is returned. Upon failure,
	 * a negative error code is returned instead.
	 */
	EXTERN int do_vmailbox_ioctl(int mbxid, unsigned request, va_list args);

	/**
	 * @brief Gets the @p mbxid logic port.
	 *
	 * @param mbxid Target mailbox.
	 *
	 * @returns Upon successful completion, the logic port is returned.
	 * Upon failure, a negative error code is returned instead.
	 */
	EXTERN int do_vmailbox_get_port(int mbxid);

	/**
	 * @brief Initializes the mailbox facility.
	 */
	EXTERN void kmailbox_init(void);

#endif /* NANVIX_MAILBOX_H_ */

/**@}*/
