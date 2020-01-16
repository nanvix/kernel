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

#ifndef _NET_MAILBOX_H_
#define _NET_MAILBOX_H_

	#include <nanvix/hlib.h>
	#include <posix/stdint.h>
	#include <nanvix/kernel/config.h>

		#define MAILBOX_RECV_BUFFER_NB 10
		#define MAILBOX_MSG_SIZE       100
		#define OUTPUT_MAILBOX_NB      10
		#define MAILBOX_PORT           1122

		/* Constants */
		#define MAILBOX_CREATE_MAX 1
		#define MAILBOX_OPEN_MAX   1

	#ifdef __NANVIX_HAS_NETWORK

		/**
		 * @brief Initialize a mailbox into the mailboxes array
		 *
		 * @return Returns 1 upon successfull mailbox creation, a negative
		 * number if an error occured.
		 */
		EXTERN int net_mailbox_create(int local);

		/**
		 * @brief Initialize a mailbox connected the give remote
		 *
		 * @return Returns the mailbox ID of the created mailox, a negative
		 * number if an error occured.
		 */
		EXTERN int net_mailbox_open(int remote);

		/**
		 * @brief Remove the mbxid mailbox to the mailboxes array if it was
		 * created with mailbox_create.
		 *
		 * @return Returns 1 for a successfull unlink, a negative number if an
		 * error occured.
		 */
		EXTERN int net_mailbox_unlink(int mbxid);

		/**
		 * @brief Remove the mbxid mailbox to the mailboxes array if it was
		 * created with mailbox_open.
		 *
		 * @return Returns 1 for a successfull unlink, a negative number if an
		 * error occured.
		 */
		EXTERN int net_mailbox_close(int mbxid);

		/**
		 * @brief Write the message contained in buffer of size size to the
		 * mailbox mbxid.
		 *
		 * @return Returns 1 for a successfull write, a negative number if an
		 * error occured
		 */
		EXTERN int net_mailbox_awrite(int mbxid, const void * buffer, size_t size);

		/**
		 * @brief Read up to size bytes from the input_mailbox into buffer. If
		 * read less than the total length of the message, the rest of the message
		 * is discarded.
		 *
		 * @return Returns 1 upon successfull read, a negative number if an
		 * error occured.
		 */
		EXTERN int net_mailbox_aread(int mbxid, void * buffer, size_t size);

		/**
		 * @brief Waits asynchronous operation.
		 *
		 * @param mbxid ID of the target mailbox.
		 *
		 * @return Zero if wait read correctly and non zero otherwise.
		 */
		EXTERN int net_mailbox_wait(int mbxid);

		/**
		 * @brief Mailbox interface
		 */

		/**
		 * @see net_mailbox_create().
		 */
		static inline int mailbox_create(int local)
		{
			return (net_mailbox_create(local));
		}

		/**
		 * @see net_mailbox_open().
		 */
		static inline int mailbox_open(int remote)
		{
			return (net_mailbox_open(remote));
		}

		/**
		 * @see net_mailbox_unlink().
		 */
		static inline int mailbox_unlink(int mbxid)
		{
			return (net_mailbox_unlink(mbxid));
		}

		/**
		 * @see net_mailbox_close().
		 */
		static inline int mailbox_close(int mbxid)
		{
			return (net_mailbox_close(mbxid));
		}

		/**
		 * @see net_mailbox_awrite().
		 */
		static inline int mailbox_awrite(int mbxid, const void *buffer, uint64_t size)
		{
			return (net_mailbox_awrite(mbxid, buffer, size));
		}

		/**
		 * @see net_mailbox_aread().
		 */
		static inline int mailbox_aread(int mbxid, void *buffer, uint64_t size)
		{
			return (net_mailbox_aread(mbxid, buffer, size));
		}

		/**
		 * @see net_mailbox_wait().
		 */
		static inline int mailbox_wait(int mbxid)
		{
			UNUSED(mbxid);
			return (0);
		}

	#endif /* __NANVIX_HAS_NETWORK */

#endif /* _NET_MAILBOX_H_ */
