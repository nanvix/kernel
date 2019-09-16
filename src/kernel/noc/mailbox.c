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
#include <nanvix/klib.h>
#include <posix/errno.h>

#if __TARGET_HAS_MAILBOX

/*============================================================================*
 * Control Structures.                                                        *
 *============================================================================*/

/**
 * @brief Table of synchronization points.
 */
PRIVATE struct mailbox
{
	struct resource resource; /**< Underlying resource.        */
	int refcount;             /**< References count.           */
	int fd;                   /**< Underlying file descriptor. */
	int nodenum;              /**< Target node number.         */
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

	return (mailbox_aread(mbxtab[mbxid].fd, buffer, size));
}

/*============================================================================*
 * do_mailbox_write()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_mailbox_awrite(int mbxid, const void * buffer, size_t size)
{
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

	return (mailbox_awrite(mbxtab[mbxid].fd, buffer, size));
}

/*============================================================================*
 * do_mailbox_wait()                                                          *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_mailbox_wait(int mbxid)
{
	/* Invalid mailbox. */
	if (!do_mailbox_is_valid(mbxid))
		return (-EBADF);

	dcache_invalidate();

	/* Waits. */
	return (mailbox_wait(mbxtab[mbxid].fd));
}

#endif /* __TARGET_HAS_MAILBOX */
