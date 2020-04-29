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

#ifndef NANVIX_NOC_COMMUNICATOR_H_
#define NANVIX_NOC_COMMUNICATOR_H_

#include <nanvix/hal.h>
#include <nanvix/hlib.h>
#include <nanvix/const.h>
#include <nanvix/kernel/mailbox.h>
#include <posix/errno.h>
#include <posix/stdarg.h>

	/**
	 * @brief Resource flags.
	 */
	/**@{*/
	#define COMMUNICATOR_FLAGS_FINISHED (1 << 0) /**< Has it finished?     */
	#define COMMUNICATOR_FLAGS_ALLOWED  (1 << 1) /**< Has it been allowed? */
	/**@}*/

    #define COMM_IOCTL_GET_VOLUME  (MAILBOX_IOCTL_GET_VOLUME)
    #define COMM_IOCTL_GET_LATENCY (MAILBOX_IOCTL_GET_LATENCY)

    #define COMM_TYPE_INPUT      (0)
    #define COMM_TYPE_OUTPUT     (1)

    #define COMM_STATUS_SUCCESS  (0)
    #define COMM_STATUS_AGAIN    (1)
    #define COMM_STATUS_RECEIVED (2)

    #define COMMUNICATOR_INITIALIZER {    \
        .resource = RESOURCE_INITIALIZER, \
        .config = {-1, -1, NULL, 0ULL},   \
        .stats = {0ULL, 0ULL},            \
        .lock = SPINLOCK_UNLOCKED         \
    }

    struct comm_config
    {
        int fd;
        int remote;          /**< Remote address.                          */
        const void * buffer; /**< User level buffer.                       */
        size_t size;
    };

    struct pstats
    {
        size_t volume;    /**< Amount of data transferred. */
        uint64_t latency; /**< Transfer latency.           */
    };

    /**
    * @brief Communicator structure.
    */
    struct communicator
    {
        /*
         * XXX: Don't Touch! This Must Come First!
         */
        struct resource resource;  /**< Generic resource information.            */

        int flags;                 /**< Auxiliar flags.                          */
        struct comm_config config; /**< Communicaton configuration.              */
        struct pstats stats;       /**< Performance Statistics.                  */

        spinlock_t lock;           /**< Protection.                              */
    };

    /**
    * @brief Resource pool.
    */
    struct communicator_pool
    {
        struct communicator * communicators; /**< Pool of communicators.       */
        int ncommunicators;		             /**< Number of communicators.     */
    };

    /**
    * @brief Resource allocation interface.
    */
    /**@{*/
    typedef int (* active_free_fn)(int);
    typedef ssize_t (* active_comm_fn)(int, const struct comm_config *, struct pstats *);
    typedef int (* active_wait_fn)(int, const struct comm_config *, struct pstats *);
    /**@}*/

    EXTERN int communicator_alloc(const struct communicator_pool *, struct comm_config *, int);
    EXTERN int communicator_free(const struct communicator_pool *, int, int, active_free_fn);
    EXTERN ssize_t communicator_operate(struct communicator *, int, active_comm_fn);
    EXTERN int communicator_wait(struct communicator *, active_wait_fn);
    EXTERN int communicator_ioctl(struct communicator *, unsigned, va_list);

    /**
	 * @brief Sets a communicator as finished.
	 *
	 * @param comm Target communicator.
	 */
	static inline void communicator_set_finished(struct communicator *comm)
	{
		comm->flags |= COMMUNICATOR_FLAGS_FINISHED;
	}

	/**
	 * @brief Sets a communicator as not finished.
	 *
	 * @param comm Target communicator.
	 */
	static inline void communicator_set_notfinished(struct communicator *comm)
	{
		comm->flags &= ~COMMUNICATOR_FLAGS_FINISHED;
	}

    /**
	 * @brief Sets a communicator as allowed.
	 *
	 * @param comm Target communicator.
	 */
	static inline void communicator_set_allowed(struct communicator *comm)
	{
		comm->flags |= COMMUNICATOR_FLAGS_ALLOWED;
	}

	/**
	 * @brief Sets a communicator as not allowed.
	 *
	 * @param comm Target communicator.
	 */
	static inline void communicator_set_notallowed(struct communicator *comm)
	{
		comm->flags &= ~COMMUNICATOR_FLAGS_ALLOWED;
	}

    /**
	 * @brief Asserts whether or not a communicator is finished.
	 *
	 * @param comm Target communicator.
	 *
	 * @returns One if the target communicator is finished, and zero otherwise.
	 */
	static inline int communicator_is_finished(const struct communicator *comm)
	{
		return (comm->flags & COMMUNICATOR_FLAGS_FINISHED);
	}

	/**
	 * @brief Asserts whether or not a communicator is allowed.
	 *
	 * @param comm Target communicator.
	 *
	 * @returns One if the target communicator is allowed, and zero otherwise.
	 */
	static inline int communicator_is_allowed(const struct communicator *comm)
	{
		return (comm->flags & COMMUNICATOR_FLAGS_ALLOWED);
	}

#endif /* NANVIX_NOC_COMMUNICATOR_H_ */

/**@}*/
