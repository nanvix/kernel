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

#include <nanvix/hal/hal.h>
#include <nanvix/kernel/mailbox.h>
#include <nanvix/klib.h>
#include <posix/errno.h>
#include <posix/stdarg.h>

#if __TARGET_HAS_MAILBOX

/*============================================================================*
 * Control Structures.                                                        *
 *============================================================================*/

/**
 * @brief Table of synchronization points.
 */
PRIVATE struct mailbox
{
	/* Control variables */
	struct resource resource; /**< Underlying resource.        */
	int refcount;             /**< References count.           */
	int fd;                   /**< Underlying file descriptor. */
	int nodenum;              /**< Target node number.         */

	/* Experiments variables */
	size_t volume;            /**< Amount of data transferred. */
	uint64_t latency;         /**< Transfer latency.           */
} ALIGN(sizeof(dword_t)) mbxtab[(MAILBOX_CREATE_MAX + MAILBOX_OPEN_MAX)];

/**
 * @brief Resource pool.
 */
PRIVATE const struct resource_pool mbxpool = {
	mbxtab, (MAILBOX_CREATE_MAX + MAILBOX_OPEN_MAX), sizeof(struct mailbox)
};

/*============================================================================*
 * do_mailbox_is_valid()                                                      *
 *============================================================================*/

/**
 * @brief Asserts whether or not a synchronization point is valid.
 *
 * @param mbxid ID of the Target Mailbox.
 *
 * @returns One if the target synchronization point is valid, and false
 * otherwise.
 */
PRIVATE int do_mailbox_is_valid(int mbxid)
{
	return WITHIN(mbxid, 0, (MAILBOX_CREATE_MAX + MAILBOX_OPEN_MAX));
}

/*============================================================================*
 * do_mailbox_create()                                                        *
 *============================================================================*/

/**
 * @brief Creates a mailbox.
 *
 * @param local Logic ID of the local node.
 *
 * @returns Upon successful completion, the ID of the newly created
 * mailbox is returned. Upon failure, a negative error code is
 * returned instead.
 */
PRIVATE int _do_mailbox_create(int local)
{
	int fd;    /* File descriptor. */
	int mbxid; /* Mailbox ID.      */

	/* Allocate a mailbox. */
	if ((mbxid = resource_alloc(&mbxpool)) < 0)
		return (-EAGAIN);

	if ((fd = mailbox_create(local)) < 0)
	{
		resource_free(&mbxpool, mbxid);
		return (fd);
	}

	/* Initialize mailbox. */
	mbxtab[mbxid].fd       = fd;
	mbxtab[mbxid].refcount = 1;
	mbxtab[mbxid].nodenum  = local;
	mbxtab[mbxid].volume   = 0ULL;
	mbxtab[mbxid].latency  = 0ULL;

	resource_set_rdonly(&mbxtab[mbxid].resource);
	resource_set_notbusy(&mbxtab[mbxid].resource);

	return (mbxid);
}

/**
 * @see _do_mailbox_create().
 */
PUBLIC int do_mailbox_create(int local)
{
	int mbxid; /* Mailbox ID. */

	/* Invalid local ID. */
	if (!WITHIN(local, 0, PROCESSOR_NOC_NODES_NUM))
		return (-EINVAL);

	/* Searchs for existing mailboxes. */
	for (int i = 0; i < (MAILBOX_CREATE_MAX + MAILBOX_OPEN_MAX); ++i)
	{
		if (!resource_is_used(&mbxtab[i].resource))
			continue;

		if (!resource_is_readable(&mbxtab[i].resource))
			continue;

		/* Not the same node num? */
		if (mbxtab[i].nodenum != local)
			continue;

		mbxid = i;
		mbxtab[i].refcount++;

		goto found;
	}

	/* Alloc a new mailbox. */
	mbxid = _do_mailbox_create(local);

found:
	dcache_invalidate();

	return (mbxid);
}

/*============================================================================*
 * do_mailbox_open()                                                          *
 *============================================================================*/

/**
 * @brief Opens a mailbox.
 *
 * @param remote Logic ID of the Target Node.
 *
 * @returns Upon successful completion, the ID of the target mailbox
 * is returned. Upon failure, a negative error code is returned instead.
 */
PRIVATE int _do_mailbox_open(int remote)
{
	int fd;    /* File descriptor. */
	int mbxid; /* Mailbox ID.      */

	/* Allocate a mailbox. */
	if ((mbxid = resource_alloc(&mbxpool)) < 0)
		return (-EAGAIN);

	if ((fd = mailbox_open(remote)) < 0)
	{
		resource_free(&mbxpool, mbxid);
		return (fd);
	}

	mbxtab[mbxid].fd       = fd;
	mbxtab[mbxid].refcount = 1;
	mbxtab[mbxid].nodenum  = remote;
	mbxtab[mbxid].volume   = 0ULL;
	mbxtab[mbxid].latency  = 0ULL;

	resource_set_wronly(&mbxtab[mbxid].resource);
	resource_set_notbusy(&mbxtab[mbxid].resource);

	return (mbxid);
}

/**
 * @see _do_mailbox_open().
 */
PUBLIC int do_mailbox_open(int remote)
{
	int mbxid; /* Mailbox ID. */

	/* Invalid remote ID. */
	if (!WITHIN(remote, 0, PROCESSOR_NOC_NODES_NUM))
		return (-EINVAL);

	/* Searchs for existing mailboxes. */
	for (int i = 0; i < (MAILBOX_CREATE_MAX + MAILBOX_OPEN_MAX); ++i)
	{
		if (!resource_is_used(&mbxtab[i].resource))
			continue;

		if (!resource_is_writable(&mbxtab[i].resource))
			continue;

		/* Not the same node num? */
		if (mbxtab[i].nodenum != remote)
			continue;

		mbxid = i;
		mbxtab[i].refcount++;

		goto found;
	}

	/* Alloc a new mailbox. */
	mbxid = _do_mailbox_open(remote);

found:
	dcache_invalidate();

	return (mbxid);
}

/*============================================================================*
 * _do_mailbox_release()                                                      *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PRIVATE int _do_mailbox_release(int mbxid, int (*release_fn)(int))
{
	int ret; /* HAL function return. */

	mbxtab[mbxid].refcount--;

	if (mbxtab[mbxid].refcount == 0)
	{
		if ((ret = release_fn(mbxtab[mbxid].fd)) < 0)
			return (ret);

		mbxtab[mbxid].fd      = -1;
		mbxtab[mbxid].nodenum = -1;

		resource_free(&mbxpool, mbxid);

		dcache_invalidate();
	}

	return (0);
}

/*============================================================================*
 * do_mailbox_unlink()                                                        *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_mailbox_unlink(int mbxid)
{
	/* Invalid mailbox. */
	if (!do_mailbox_is_valid(mbxid))
		return (-EBADF);

	/* Bad mailbox. */
	if (!resource_is_used(&mbxtab[mbxid].resource))
		return (-EBADF);

	/* Bad mailbox. */
	if (!resource_is_readable(&mbxtab[mbxid].resource))
		return (-EBADF);

	/* Release resource. */
	return (_do_mailbox_release(mbxid, mailbox_unlink));
}

/*============================================================================*
 * do_mailbox_close()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_mailbox_close(int mbxid)
{
	/* Invalid mailbox. */
	if (!do_mailbox_is_valid(mbxid))
		return (-EBADF);

	/* Bad mailbox. */
	if (!resource_is_used(&mbxtab[mbxid].resource))
		return (-EBADF);

	/* Bad mailbox. */
	if (!resource_is_writable(&mbxtab[mbxid].resource))
		return (-EBADF);

	/* Release resource. */
	return (_do_mailbox_release(mbxid, mailbox_close));
}

/*============================================================================*
 * do_mailbox_aread()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_mailbox_aread(int mbxid, void * buffer, size_t size)
{
	int ret;     /* HAL function return.           */
	uint64_t t1; /* Clock value before aread call. */
	uint64_t t2; /* Clock value after aread call.  */

	/* Invalid mailbox. */
	if (!do_mailbox_is_valid(mbxid))
		return (-EBADF);

	/* Invalid buffer. */
	if (buffer == NULL)
		return (-EINVAL);

	/* Invalid read size. */
	if (size != MAILBOX_MSG_SIZE)
		return (-EINVAL);

	/* Bad mailbox. */
	if (!resource_is_used(&mbxtab[mbxid].resource))
		return (-EBADF);

	/* Bad mailbox. */
	if (!resource_is_readable(&mbxtab[mbxid].resource))
		return (-EBADF);

	t1 = clock_read();

		/* Configures async read. */
		if ((ret = mailbox_aread(mbxtab[mbxid].fd, buffer, size)) < 0)
			return (ret);

	t2 = clock_read();

	/* Updates latency variable. */
	mbxtab[mbxid].latency += (t2 - t1);

	/* Updates volume variable. */
	mbxtab[mbxid].volume += ret;

	return (ret);
}

/*============================================================================*
 * do_mailbox_write()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_mailbox_awrite(int mbxid, const void * buffer, size_t size)
{
	int ret;     /* HAL function return.            */
	uint64_t t1; /* Clock value before awrite call. */
	uint64_t t2; /* Clock value after awrite call.  */

	/* Invalid mailbox. */
	if (!do_mailbox_is_valid(mbxid))
		return (-EBADF);

	/* Invalid buffer. */
	if (buffer == NULL)
		return (-EINVAL);

	/* Invalid write size. */
	if (size != MAILBOX_MSG_SIZE)
		return (-EINVAL);

	/* Bad mailbox. */
	if (!resource_is_used(&mbxtab[mbxid].resource))
		return (-EBADF);

	/* Bad mailbox. */
	if (!resource_is_writable(&mbxtab[mbxid].resource))
		return (-EBADF);

	t1 = clock_read();

		/* Configures async write. */
		if ((ret = mailbox_awrite(mbxtab[mbxid].fd, buffer, size)) < 0)
			return (ret);

	t2 = clock_read();

	/* Updates latency variable. */
	mbxtab[mbxid].latency += (t2 - t1);

	/* Updates volume variable. */
	mbxtab[mbxid].volume += ret;

	return (ret);
}

/*============================================================================*
 * do_mailbox_wait()                                                          *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_mailbox_wait(int mbxid)
{
	int ret;     /* HAL function return.            */
	uint64_t t1; /* Clock value before wait call. */
	uint64_t t2; /* Clock value after wait call.  */

	/* Invalid mailbox. */
	if (!do_mailbox_is_valid(mbxid))
		return (-EBADF);

	dcache_invalidate();

	t1 = clock_read();

		ret = mailbox_wait(mbxtab[mbxid].fd);

	t2 = clock_read();

	/* Updates latency variable. */
	mbxtab[mbxid].latency += (t2 - t1);

	return (ret);
}

/*============================================================================*
 * do_mailbox_ioctl()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
int do_mailbox_ioctl(int mbxid, unsigned request, va_list args)
{
	int ret = 0;

	/* Invalid mailbox. */
	if (!do_mailbox_is_valid(mbxid))
		return (-EBADF);

	/* Bad mailbox. */
	if (!resource_is_used(&mbxtab[mbxid].resource))
		return (-EBADF);

	/* Server request. */
	switch (request)
	{
		/* Get the amount of data transferred so far. */
		case MAILBOX_IOCTL_GET_VOLUME:
		{
			size_t *volume;
			volume = va_arg(args, size_t *);
			*volume = mbxtab[mbxid].volume;
		} break;

		/* Get the cumulative transfer latency. */
		case MAILBOX_IOCTL_GET_LATENCY:
		{
			uint64_t *latency;
			latency = va_arg(args, uint64_t *);
			*latency = mbxtab[mbxid].latency;
		} break;

		/* Operation not supported. */
		default:
			ret = (-ENOTSUP);
			break;
	}

	return (ret);
}

#endif /* __TARGET_HAS_MAILBOX */
