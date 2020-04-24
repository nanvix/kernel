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

/* Must come first. */
#define __NEED_RESOURCE

#include <nanvix/hal.h>
#include <nanvix/hlib.h>
#include <nanvix/const.h>
#include <posix/errno.h>
#include <posix/stdarg.h>

#include "communicator.h"

/*============================================================================*
 * communicator_alloc()                                                       *
 *============================================================================*/

PUBLIC int communicator_alloc(
    const struct communicator_pool * pool,
    struct comm_config * config,
    int type
)
{
	struct communicator * comm;

	/* Search for a free synchronization point. */
	for (int i = 0; i < pool->ncommunicators; i++)
	{
		comm = &pool->communicators[i];

		spinlock_lock(&comm->lock);

			/* Found. */
			if (!resource_is_used(&comm->resource))
			{
				comm->resource = RESOURCE_INITIALIZER;
				comm->config   = *config;
				comm->stats    = (struct pstats){0ULL, 0ULL};

				resource_set_used(&comm->resource);

				if (type == COMM_TYPE_INPUT)
					resource_set_rdonly(&comm->resource);
				else
					resource_set_wronly(&comm->resource);

				spinlock_unlock(&comm->lock);

				return (i);
			}

		spinlock_unlock(&comm->lock);
	}

	return (-EAGAIN);
}

/*============================================================================*
 * communicator_free()                                                        *
 *============================================================================*/

/**
 * @brief Dumb resource releaser.
 *
 * @param pool Target generic resource pool.
 * @param id   ID of the target resource.
 *
 * @note This function is non-blocking.
 * @note This function is @b NOT thread safe.
 * @note This function is reentrant.
 */
PUBLIC int communicator_free(
    const struct communicator_pool *pool,
    int id,
    int type,
    active_free_fn do_free
)
{
    int ret; /* Function return. */
	struct communicator * comm;

	comm = &pool->communicators[id];

	spinlock_lock(&comm->lock);

		ret = (-EBADF);

		/* Bad communicator. */
		if (!resource_is_used(&comm->resource))
			goto error;

        /* Bad communicator. */
		if (type == COMM_TYPE_INPUT)
        {
            if (!resource_is_readable(&comm->resource))
                goto error;
        }

		/* Bad communicator (COMM_TYPE_OUTPUT). */
		else if (!resource_is_writable(&comm->resource))
			goto error;

		ret = (-EBUSY);

		/* Busy communicator. */
		if (resource_is_busy(&comm->resource))
			goto error;

		/* Releases communicator. */
		if ((ret = do_free(comm->config.fd)) == 0)
			resource_set_unused(&comm->resource);

error:
	spinlock_unlock(&comm->lock);

	return (ret);
}

/*============================================================================*
 * communicator_operate()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC ssize_t communicator_operate(
	struct communicator * comm,
	int type,
	active_comm_fn do_comm
)
{
	ssize_t ret; /* Return value. */

	spinlock_lock(&comm->lock);

		ret = (-EBADF);

		/* Bad communicator. */
		if (!resource_is_used(&comm->resource))
			goto error;

		/* Bad communicator. */
		if (type == COMM_TYPE_INPUT)
		{
			if (!resource_is_readable(&comm->resource))
				goto error;

			ret = (-EACCES);

			if (!communicator_is_allowed(comm))
				goto error;
		}

		/* Bad communicator (COMM_TYPE_OUTPUT). */
		else if (!resource_is_writable(&comm->resource))
			goto error;

		ret = (-EBUSY);

		/* Busy communicator. */
		if (resource_is_busy(&comm->resource))
			goto error;

		ret = do_comm(
			comm->config.fd,
			&comm->config,
			&comm->stats
		);

        /* Successfully complete communication? */
        if (ret >= 0)
        {
            if (ret == COMM_STATUS_RECEIVED)
            {
				communicator_set_finished(comm);
				communicator_set_notallowed(comm);
            }

			ret = comm->config.size;

			/* Sets the communicator as busy. */
			resource_set_busy(&comm->resource);
		}

error:
	spinlock_unlock(&comm->lock);

	return (ret);
}

/*============================================================================*
 * communicator_wait()                                                         *
 *============================================================================*/

/**
 * @brief Waits on a communicator to finish an assynchronous operation.
 *
 * @param mbxid Logic ID of the target communicator.
 *
 * @returns Upon successful completion, a positive number is returned.
 * Upon failure, a negative error code is returned instead.
 */
PUBLIC int communicator_wait(
	struct communicator * comm,
	active_wait_fn do_wait
)
{
	int ret; /* Return value. */

	spinlock_lock(&comm->lock);

		/* Bad communicator. */
		if (!resource_is_used(&comm->resource))
		{
			spinlock_unlock(&comm->lock);
			return (-EBADF);
		}

		/* communicator not set as busy. */
		if (!resource_is_busy(&comm->resource))
		{
			spinlock_unlock(&comm->lock);
			return (-EBADF);
		}

		/* communicator already finished its last operation. */
		if (communicator_is_finished(comm))
		{
			communicator_set_notfinished(comm);

			ret = 0;
			goto release;
		}

	spinlock_unlock(&comm->lock);

	ret = do_wait(
		comm->config.fd,
		&comm->config,
		&comm->stats
	);

	spinlock_lock(&comm->lock);

		/* Revoke communicator allow. */
		if (ret == COMM_STATUS_SUCCESS)
			communicator_set_notallowed(comm);

release:
		comm->config.buffer = NULL;
        comm->config.size   = 0ULL;
		resource_set_notbusy(&comm->resource);
	spinlock_unlock(&comm->lock);

	return (ret);
}

/*============================================================================*
 * communicator_ioctl()                                                        *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int communicator_ioctl(
    struct communicator * comm,
    unsigned request,
    va_list args
)
{
	int ret; /* Return value. */

	spinlock_lock(&comm->lock);

		ret = (-EBADF);

		/* Bad communicator. */
		if (!resource_is_used(&comm->resource))
			goto error;

		ret = (-EBUSY);

		/* Busy communicator. */
		if (resource_is_busy(&comm->resource))
			goto error;

		ret = 0;

		/* Parse request. */
		switch (request)
		{
			/* Get the amount of data transferred so far. */
			case COMM_IOCTL_GET_VOLUME:
			{
				size_t *volume;
				volume = va_arg(args, size_t *);
				*volume = comm->stats.volume;
			} break;

			/* Get the cumulative transfer latency. */
			case COMM_IOCTL_GET_LATENCY:
			{
				uint64_t *latency;
				latency = va_arg(args, uint64_t *);
				*latency = comm->stats.latency;
			} break;

			/* Operation not supported. */
			default:
				ret = (-ENOTSUP);
				break;
		}

error:
	spinlock_unlock(&comm->lock);

	return (ret);
}
