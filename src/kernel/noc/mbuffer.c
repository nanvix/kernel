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
#include <posix/errno.h>
#include <posix/stdarg.h>

#include "mbuffer.h"

#if __TARGET_HAS_MAILBOX

/*============================================================================*
 * mbuffer_alloc()                                                            *
 *============================================================================*/

/**
 * @brief Allocates a message buffer.
 *
 * @return Upon successful completion, mbufferid returned. Upon failure,
 * a negative number is returned instead.
 */
PUBLIC int mbuffer_alloc(struct mbuffer_pool * pool)
{
	int ret;
	char *base = (char *) pool->mbuffers;
	int n = pool->nmbuffers;
	size_t size = pool->mbuffer_size;

	ret = (-EINVAL);

	spinlock_lock(&pool->lock);

		/* Search for a free synchronization point. */
		for (int i = 0; i < n; i++)
		{
			struct mbuffer * buf;

			buf = (struct mbuffer *)(&base[mult(i, size)]);

			/* Found. */
			if (!resource_is_used(&buf->resource))
			{
				buf->resource = RESOURCE_INITIALIZER;
				buf->message.dest = -1;
				kmemset(buf->message.data, '\0', MBUFFER_DEFAULT_DATA_SIZE);

				resource_set_used(&buf->resource);

				ret = (i);
				break;
			}
		}

	spinlock_unlock(&pool->lock);

	return (ret);
}

/*============================================================================*
 * mbuffer_release()                                                          *
 *============================================================================*/

/**
 * @brief Releases the message buffer allocated to @p mbxid.
 *
 * @param mbufferid mbuffer id to release.
 * @param keep_msg  Keep / Discard the mbuffer message?
 *
 * @return Upon successful completion, zero is returned. Upon failure,
 * a negative number is returned instead.
 */
PUBLIC int mbuffer_release(struct mbuffer_pool * pool, int id, int keep_msg)
{
	char *base = (char *) pool->mbuffers;
	size_t size = pool->mbuffer_size;
	struct mbuffer * buf;

	buf = (struct mbuffer *)(&base[mult(id, size)]);

	spinlock_lock(&pool->lock);

		if (!resource_is_used(&buf->resource))
		{
			spinlock_unlock(&pool->lock);
			return (-EINVAL);
		}

		/* Sets the mbuffer as not used keeping its message. */
		if (keep_msg == MBUFFER_KEEP_MESSAGE)
			resource_set_busy(&buf->resource);

		/* Frees the mbuffer resource. */
		else
		{
			buf->message.dest = -1;
			kmemset(buf->message.data, '\0', MBUFFER_DEFAULT_DATA_SIZE);
			resource_set_unused(&buf->resource);
		}

	/* Unlocks the mbuffers table. */
	spinlock_unlock(&pool->lock);

	return (0);
}

/*============================================================================*
 * mbuffer_search()                                                           *
 *============================================================================*/

/**
 * @brief Searches for a stored message destinated to @p local_address.
 *
 * @param local_address Local HW address for which the messages come.
 *
 * @returns Upon successful completion, the mbuffer that contains the first
 * message found is returned. A negative error number is returned instead.
 */
PUBLIC int mbuffer_search(struct mbuffer_pool * pool, int dest, int src)
{
	int ret;
	char *base = (char *) pool->mbuffers;
	int n = pool->nmbuffers;
	size_t size = pool->mbuffer_size;

	ret = (-EINVAL);

	/* Locks the mbuffers table. */
	spinlock_lock(&pool->lock);

		for (int i = 0; i < n; i++)
		{
			struct mbuffer * buf;

			buf = (struct mbuffer *)(&base[mult(i, size)]);

			/* Is the buffer being used? */
			if (!resource_is_used(&buf->resource))
				continue;

			/* The buffer contains a stored message? */
			if (!resource_is_busy(&buf->resource))
				continue;

			/* Is this message addressed to the dest? */
			if (buf->message.dest != dest)
				continue;

			/* Is the message sender the expected? */
			if (src != -1 && buf->message.src != src)
				continue;

			ret = i;
			break;
		}

	/* Unlocks the mbuffers table. */
	spinlock_unlock(&pool->lock);

	return (ret);
}

PUBLIC struct mbuffer * mbuffer_get(struct mbuffer_pool * pool, int id)
{
	char *base = (char *) pool->mbuffers;
	size_t size = pool->mbuffer_size;

	return (struct mbuffer *)(&base[mult(id, size)]);
}

#endif /* __TARGET_HAS_MAILBOX */