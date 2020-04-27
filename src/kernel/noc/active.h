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

#ifndef NANVIX_NOC_ACTIVE_H_
#define NANVIX_NOC_ACTIVE_H_

	#include <nanvix/hal.h>
	#include <nanvix/hlib.h>
	#include <nanvix/const.h>
	#include <posix/errno.h>
	#include <posix/stdarg.h>

	#include "mbuffer.h"
	#include "port.h"
	#include "communicator.h"

	/**
	 * @brief Resource flags.
	 */
	/**@{*/
	#define ACTIVE_FLAGS_ALLOWED  ((RESOURCE_FLAGS_MAPPED) << 1) /**< Has it been allowed? */
	/**@}*/

	/**
     * @brief Composes the logic address based on @p fd @p port.
     */
    #define ACTIVE_LADDRESS_COMPOSE(_fd, _port, _nports) (_fd * _nports + _port)

	/**
	 * @brief Extracts fd and port from id.
	 */
	/**@{*/
	#define ACTIVE_GET_LADDRESS_FD(_act, _id)   ((_id >= 0) ? (_id / ACTIVE_GET_NR_PORTS(_act)) : (_id))
	#define ACTIVE_GET_LADDRESS_PORT(_act, _id) ((_id >= 0) ? (_id % ACTIVE_GET_NR_PORTS(_act)) : (_id))
	/**@}*/

	#define ACTIVE_GET_NR_PORTS(_act) (_act->portpool.nports)

	#define ACTIVE_ANY_SRC           (-1)
	#define ACTIVE_COPY_TO_MBUFFER    (0)
	#define ACTIVE_COPY_FROM_MBUFFER  (1)

	/*============================================================================*
	* Control Structures.                                                        *
	*============================================================================*/

	struct active;

    /**
    * @brief Resource allocation interface.
    */
    /**@{*/
	typedef int (* hw_create_fn)(int);
	typedef int (* hw_open_fn)(int, int);
	typedef int (* hw_allow_fn)(struct active *, int);
    typedef ssize_t (* hw_aread_fn)(int mbxid, void *buffer, size_t size);
	typedef ssize_t (* hw_awrite_fn)(int mbxid, const void *buffer, size_t size);
    typedef int (* hw_wait_fn)(int);
	typedef int (* hw_copy_fn)(struct mbuffer *, const struct comm_config *, int);
	typedef int (* hw_config_fn)(struct mbuffer *, const struct comm_config *);
	typedef int (* hw_check_fn)(struct mbuffer *, const struct comm_config *);
    /**@}*/

	/**
	 * @brief Circular FIFO to hold write requests.
	 */
	struct requests_fifo
	{
		short head;         /**< Index of the first element. */
		short tail;         /**< Index of the last element.  */
		short max_capacity; /**< Max number of elements.     */
		short nelements;    /**< Nr of elements in the fifo. */
		short * fifo;       /**< Buffer to store elements.   */
	};

	/**
	 * @brief Table of active mailboxes.
	 */
	struct active
	{
		/*
		* XXX: Don't Touch! This Must Come First!
		*/
		struct resource resource; /**< Generic resource information. */

		int hwfd;                 /**< Underlying file descriptor.   */
		int local;                /**< Target node number.           */
		int remote;               /**< Target node number.           */
		int refcount;             /**< Target node number.           */

		size_t size;

		struct port_pool portpool;
		struct mbuffer_pool * mbufferpool;
		struct requests_fifo requests;
	
		const hw_create_fn do_create;
		const hw_open_fn do_open;
		const hw_allow_fn do_allow;
		const hw_aread_fn do_aread;
		const hw_awrite_fn do_awrite;
		const hw_wait_fn do_wait;
		const hw_copy_fn do_copy;
		const hw_config_fn do_header_config;
		const hw_check_fn do_header_check;

		spinlock_t lock;          /**< Protection.                   */
	};

	struct active_pool
	{
		struct active * actives;  /**< Pool of actives.   */
		const int nactives;       /**< Number of actives. */
	};

	EXTERN int active_alloc(const struct active_pool *, int, int, int, int);
	EXTERN int active_release(const struct active_pool *, int);
	EXTERN ssize_t active_aread(const struct active_pool *, int, const struct comm_config *, struct pstats *);
	EXTERN ssize_t active_awrite(const struct active_pool *, int, const struct comm_config *, struct pstats *);
	EXTERN int active_wait(const struct active_pool *, int, const struct comm_config *, struct pstats *);

	PUBLIC int active_create(const struct active_pool *, int);
	PUBLIC int active_open(const struct active_pool *, int, int);

    /**
	 * @brief Sets a active as allowed.
	 *
	 * @param active Target active.
	 */
	static inline void active_set_allowed(struct active * active)
	{
		active->resource.flags |= ACTIVE_FLAGS_ALLOWED;
	}

	/**
	 * @brief Sets a active as not allowed.
	 *
	 * @param active Target active.
	 */
	static inline void active_set_notallowed(struct active * active)
	{
		active->resource.flags &= ~ACTIVE_FLAGS_ALLOWED;
	}

	/**
	 * @brief Asserts whether or not a active is allowed.
	 *
	 * @param active Target active.
	 *
	 * @returns One if the target active is allowed, and zero otherwise.
	 */
	static inline int active_is_allowed(const struct active * active)
	{
		return (active->resource.flags & ACTIVE_FLAGS_ALLOWED);
	}

#endif /* NANVIX_NOC_ACTIVE_H_ */

/**@}*/
