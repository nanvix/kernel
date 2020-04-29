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
 * @defgroup kernel-noc Noc Facility
 * @ingroup kernel
 *
 * @brief Noc Facility
 */

#ifndef NANVIX_NOC_MBUFFER_H_
#define NANVIX_NOC_MBUFFER_H_

#include <nanvix/hal.h>
#include <nanvix/hlib.h>
#include <nanvix/const.h>
#include <nanvix/kernel/mailbox.h>
#include <posix/errno.h>
#include <posix/stdarg.h>

	/**
	 * @name Release modes of mbuffers.
	 */
	/**@{*/
	#define MBUFFER_DISCARD_MESSAGE (0) /**< Discard message and releases the mbuffer. */
	#define MBUFFER_KEEP_MESSAGE    (1) /**< Keep message intact.                      */
	/**@}*/

	/**
	 * @brief Size of the mbuffer message header.
	 */
	#define MBUFFER_HEADER_SIZE sizeof(struct mbuffer_header)

	/**
	 * @brief Mbuffer message initializer.
	 */
	#define MBUFFER_MESSAGE_INITIALIZER (struct mbuffer_message){ \
		.header = {-1, -1, 0},                                    \
		.data   = '\0',                                           \
	}

	/**
	 * @brief Mbuffer initializer.
	 */
	#define MBUFFER_INITIALIZER {                   \
		.abstract = {                               \
			.resource = RESOURCE_INITIALIZER,       \
			.message  = MBUFFER_MESSAGE_INITIALIZER \
		}                                           \
	}

	/*============================================================================*
	 * Auxiliar Structures.                                                       *
	 *============================================================================*/

	/**
	 * @brief Mbuffer header.
	 */
	struct mbuffer_header
	{
		int dest; /* Data sender.       */
		int src;  /* Data destination.  */
		int size; /* Message data size. */
	};

	/**
	 * @brief Abstract mbuffer message.
	 */
	struct mbuffer_message
	{
		struct mbuffer_header header; /* Header.     */
		char data;                    /* First data. */
	};

	/*============================================================================*
	 * Mbuffers structures.                                                       *
	 *============================================================================*/

	/**
	 * @brief Mailbox message.
	 */
	struct mailbox_message
	{
		struct mbuffer_header header;                          /* Header. */
		char data[HAL_MAILBOX_MSG_SIZE - MBUFFER_HEADER_SIZE]; /* Data.   */
	};

	/**
	 * @brief Portal message.
	 */
	struct portal_message
	{
		struct mbuffer_header header;   /* Header. */
		char data[HAL_PORTAL_MAX_SIZE]; /* Data.   */
	};

	/**
	 * @brief Abstract mbuffer.
	 */
	struct mbuffer
	{
		/*
		 * XXX: Don't Touch! This Must Come First!
		 */
		struct resource resource;       /**< Generic resource information.   */
		struct mbuffer_message message; /**< Structure that holds a message. */
	};

	/**
	 * @brief Mailbox mbuffer.
	 */
	union mailbox_mbuffer
	{
		struct mbuffer abstract;
		struct
		{
			/*
			* XXX: Don't Touch! This Must Come First!
			*/
			struct resource resource;       /**< Generic resource information. */
			struct mailbox_message message; /**< Structure that holds a message. */
		};
	};

	/**
	 * @brief Portal mbuffer.
	 */
	union portal_mbuffer
	{
		struct mbuffer abstract;
		struct
		{
			/*
			* XXX: Don't Touch! This Must Come First!
			*/
			struct resource resource;       /**< Generic resource information. */
			struct portal_message message; /**< Structure that holds a message. */
		};
	};

	/**
	* @brief Resource pool.
	*/
	struct mbuffer_pool
	{
		void * mbuffers;     /**< Pool of mbuffers.        */
		int nmbuffers;		 /**< Number of mbuffers.      */
		size_t mbuffer_size; /**< mbuffer size (in bytes). */
		spinlock_t lock;
	};

	/*============================================================================*
	 * Mbuffer interface.                                                         *
	 *============================================================================*/

	EXTERN int mbuffer_alloc(struct mbuffer_pool *);
	EXTERN int mbuffer_release(struct mbuffer_pool *, int, int);
	EXTERN int mbuffer_search(struct mbuffer_pool *, int, int);
	EXTERN struct mbuffer * mbuffer_get(struct mbuffer_pool *, int);

#endif /* NANVIX_NOC_MBUFFER_H_ */

/**@}*/