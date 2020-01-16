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

#include <nanvix/hal.h>
#include <nanvix/kernel/mailbox.h>
#include <nanvix/hlib.h>
#include <net/mailbox.h>
#include <posix/errno.h>
#include <posix/stdarg.h>

#if defined(__TARGET_HAS_MAILBOX) || defined(__NANVIX_HAS_NETWORK)

/**
 * @brief Search types for do_mailbox_search().
 */
enum mailbox_search_type {
	MAILBOX_SEARCH_INPUT = 0,
	MAILBOX_SEARCH_OUTPUT = 1
} resource_type_enum_t;

/*============================================================================*
 * Control Structures.                                                        *
 *============================================================================*/

/**
 * @brief Table of virtual mailboxes.
 */
PRIVATE struct
{
	/**
	 * @name Control Variables
	 */
	/**@{*/
	int fd; /**< Index to table of active mailboxes. */
	/**@}*/

	/**
	 * @name Performance Statistics
	 */
	/**@{*/
	size_t volume;   /**< Amount of data transferred. */
	uint64_t latency;/**< Transfer latency.           */
	/**@}*/
} ALIGN(sizeof(dword_t)) virtual_mailboxes[KMAILBOX_MAX] = {
	[0 ... (KMAILBOX_MAX - 1)] = { .fd = -1 },
};

/**
 * @brief Table of active mailboxes.
 */
PRIVATE struct mailbox
{
	struct resource resource;  /**< Underlying resource.        */
	int refcount;              /**< References count.           */
	int hwfd;                  /**< Underlying file descriptor. */
	int nodenum;               /**< Target node number.         */
} active_mailboxes[(MAILBOX_CREATE_MAX + MAILBOX_OPEN_MAX)];

/**
 * @brief Resource pool.
 */
PRIVATE const struct resource_pool mbxpool = {
	active_mailboxes, (MAILBOX_CREATE_MAX + MAILBOX_OPEN_MAX), sizeof(struct mailbox)
};

/*============================================================================*
 * do_vmailbox_is_valid()                                                     *
 *============================================================================*/

/**
 * @brief Asserts whether or not a virtual mailbox ID is valid.
 *
 * @param mbxid ID of the Target Mailbox.
 *
 * @returns One if the mailboxID is valid, and false otherwise.
 */
PRIVATE int do_vmailbox_is_valid(int mbxid)
{
	return WITHIN(mbxid, 0, KMAILBOX_MAX);
}

/*============================================================================*
 * do_vmailbox_alloc()                                                        *
 *============================================================================*/

/**
 * @brief Searches for a free virtual mailbox.
 *
 * @returns Upon successful completion, the index of the virtual
 * mailbox found is returned. Upon failure, a negative number is
 * returned instead.
 */
PRIVATE int do_vmailbox_alloc(void)
{
	for (int i = 0; i < KMAILBOX_MAX; ++i)
	{
		/* Found. */
		if (virtual_mailboxes[i].fd < 0)
			return (i);
	}

	return (-1);
}

/*============================================================================*
 * do_mailbox_search()                                                        *
 *============================================================================*/

/**
 * @name Helper Macros for do_mailbox_search()
 */
/**@{*/

/**
 * @brief Asserts an input mailbox.
 */
#define MAILBOX_SEARCH_IS_INPUT(mbxid,type) \
	((type == MAILBOX_SEARCH_INPUT) && !resource_is_readable(&active_mailboxes[mbxid].resource))

/**
 * @brief Asserts an output mailbox.
 */
#define MAILBOX_SEARCH_IS_OUTPUT(mbxid,type) \
	 ((type == MAILBOX_SEARCH_OUTPUT) && !resource_is_writable(&active_mailboxes[mbxid].resource))
/**@}*/

/**
 * @brief Searches for a mailbox.
 *
 * Searches for a mailbox in the table of active mailboxes.
 *
 * @param nodenum     Logic ID of the requesting node.
 * @param search_type Type of the searched resource.
 *
 * @returns Upon successful completion, the ID of the mailbox found is
 * returned. Upon failure, a negative error code is returned instead.
 */
PRIVATE int do_mailbox_search(int nodenum, enum mailbox_search_type search_type)
{
	for (int i = 0; i < (MAILBOX_CREATE_MAX + MAILBOX_OPEN_MAX); ++i)
	{
		if (!resource_is_used(&active_mailboxes[i].resource))
			continue;

		if (MAILBOX_SEARCH_IS_INPUT(i, search_type))
			continue;

		else if (MAILBOX_SEARCH_IS_OUTPUT(i, search_type))
			continue;

		/* Not the node we are looking for. */
		if (active_mailboxes[i].nodenum != nodenum)
			continue;

		return (i);
	}

	return (-1);
}

/*============================================================================*
 * do_vmailbox_create()                                                       *
 *============================================================================*/

/**
 * @brief Creates a hardware mailbox.
 *
 * @param local Logic ID of the target local node.
 *
 * @returns Upon successful completion, the ID of the newly created
 * hardware mailbox is returned. Upon failure, a negative error code
 * is returned instead.
 */
PRIVATE int _do_mailbox_create(int local)
{
	int hwfd;  /* File descriptor. */
	int mbxid; /* Mailbox ID.      */

	/* Search target hardware mailbox. */
	if ((mbxid = do_mailbox_search(local, MAILBOX_SEARCH_INPUT)) >= 0)
		return (mbxid);

	/* Allocate resource. */
	if ((mbxid = resource_alloc(&mbxpool)) < 0)
		return (-EAGAIN);

	/* Create underlying input hardware mailbox. */
	if ((hwfd = mailbox_create(local)) < 0)
	{
		resource_free(&mbxpool, mbxid);
		return (hwfd);
	}

	/* Initialize hardware mailbox. */
	active_mailboxes[mbxid].hwfd     = hwfd;
	active_mailboxes[mbxid].refcount = 0;
	active_mailboxes[mbxid].nodenum  = local;
	resource_set_rdonly(&active_mailboxes[mbxid].resource);
	resource_set_notbusy(&active_mailboxes[mbxid].resource);

	return (mbxid);
}

/**
 * @brief Creates a virtual mailbox.
 *
 * @param local Logic ID of the target local node.
 *
 * @returns Upon successful completion, the ID of the newly created
 * virtual mailbox is returned. Upon failure, a negative error code
 * is returned instead.
 */
PUBLIC int do_vmailbox_create(int local)
{
	int mbxid;  /* Hardware mailbox ID. */
	int vmbxid; /* Virtual mailbox ID.  */

	/* Allocate a virtual mailbox. */
	if ((vmbxid = do_vmailbox_alloc()) < 0)
		return (-EAGAIN);

	/* Create hardware mailbox. */
	if ((mbxid = _do_mailbox_create(local)) < 0)
		return (mbxid);

	/* Initialize virtual mailbox. */
	virtual_mailboxes[vmbxid].fd      = mbxid;
	virtual_mailboxes[vmbxid].volume  = 0ULL;
	virtual_mailboxes[vmbxid].latency = 0ULL;
	active_mailboxes[mbxid].refcount++;

	dcache_invalidate();
	return (vmbxid);
}

/*============================================================================*
 * do_vmailbox_open()                                                         *
 *============================================================================*/

/**
 * @brief Opens a hardware mailbox.
 *
 * @param remote Logic ID of the target remote node.
 *
 * @returns Upon successful completion, the ID of the newly opened
 * hardware mailbox is returned. Upon failure, a negative error code
 * is returned instead.
 */
PRIVATE int _do_mailbox_open(int remote)
{
	int hwfd;  /* File descriptor. */
	int mbxid; /* Mailbox ID.      */

	/* Search target hardware mailbox. */
	if ((mbxid = do_mailbox_search(remote, MAILBOX_SEARCH_OUTPUT)) >= 0)
		return (mbxid);

	/* Allocate resource. */
	if ((mbxid = resource_alloc(&mbxpool)) < 0)
		return (-EAGAIN);

	/* Open underlying output hardware mailbox. */
	if ((hwfd = mailbox_open(remote)) < 0)
	{
		resource_free(&mbxpool, mbxid);
		return (hwfd);
	}

	/* Initialize hardware mailbox. */
	active_mailboxes[mbxid].hwfd     = hwfd;
	active_mailboxes[mbxid].refcount = 0;
	active_mailboxes[mbxid].nodenum  = remote;
	resource_set_wronly(&active_mailboxes[mbxid].resource);
	resource_set_notbusy(&active_mailboxes[mbxid].resource);

	return (mbxid);
}

/**
 * @brief Opens a virtual mailbox.
 *
 * @param remote Logic ID of the target remote node.
 *
 * @returns Upon successful completion, the ID of the newly opened
 * virtual mailbox is returned. Upon failure, a negative error code
 * is returned instead.
 */
PUBLIC int do_vmailbox_open(int remote)
{
	int mbxid;  /* Hardware mailbox ID. */
	int vmbxid; /* Virtual mailbox ID.  */

	/* Allocate a virtual mailbox. */
	if ((vmbxid = do_vmailbox_alloc()) < 0)
		return (-EAGAIN);

	/* Create hardware mailbox. */
	if ((mbxid = _do_mailbox_open(remote)) < 0)
		return (mbxid);

	/* Initialize virtual mailbox. */
	virtual_mailboxes[vmbxid].fd       = mbxid;
	virtual_mailboxes[vmbxid].volume   = 0ULL;
	virtual_mailboxes[vmbxid].latency  = 0ULL;
	active_mailboxes[mbxid].refcount++;

	dcache_invalidate();
	return (vmbxid);
}

/*============================================================================*
 * _do_mailbox_release()                                                      *
 *============================================================================*/

/**
 * @brief Releases a hardware mailbox.
 *
 * @param mbxid      ID of the target hardware mailbox.
 * @param release_fn Release function.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PRIVATE int _do_mailbox_release(int mbxid, int (*release_fn)(int))
{
	int ret;

	if ((ret = release_fn(active_mailboxes[mbxid].hwfd)) < 0)
		return (ret);

	active_mailboxes[mbxid].hwfd    = -1;
	active_mailboxes[mbxid].nodenum = -1;
	resource_free(&mbxpool, mbxid);

	dcache_invalidate();
	return (0);
}

/*============================================================================*
 * do_vmailbox_unlink()                                                       *
 *============================================================================*/

/**
 * @brief Unlinks a created virtual mailbox.
 *
 * @param mbxid Logic ID of the target virtual mailbox.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int do_vmailbox_unlink(int mbxid)
{
	int fd; /* Active_mailboxes table index. */

	/* Invalid virtual mailbox. */
	if (!do_vmailbox_is_valid(mbxid))
		return (-EINVAL);

	fd = virtual_mailboxes[mbxid].fd;

	/* Bad virtual mailbox. */
	if (!resource_is_used(&active_mailboxes[fd].resource))
		return (-EBADF);

	/* Bad virtual mailbox. */
	if (!resource_is_readable(&active_mailboxes[fd].resource))
		return (-EBADF);

	/* Unlink virtual mailbox. */
	virtual_mailboxes[mbxid].fd = -1;
	active_mailboxes[fd].refcount--;

	/* Release underlying hardware mailbox. */
	if (active_mailboxes[fd].refcount == 0)
		return (_do_mailbox_release(fd, mailbox_unlink));

	return (0);
}

/*============================================================================*
 * do_vmailbox_close()                                                        *
 *============================================================================*/

/**
 * @brief Closes an opened virtual mailbox.
 *
 * @param mbxid Logic ID of the target virtual mailbox.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int do_vmailbox_close(int mbxid)
{
	int fd; /* Active_mailboxes table index. */

	/* Invalid virtual mailbox. */
	if (!do_vmailbox_is_valid(mbxid))
		return (-EINVAL);

	fd = virtual_mailboxes[mbxid].fd;

	/* Bad virtual mailbox. */
	if (!resource_is_used(&active_mailboxes[fd].resource))
		return (-EBADF);

	/* Bad virtual mailbox. */
	if (!resource_is_writable(&active_mailboxes[fd].resource))
		return (-EBADF);

	/* Unlink hardware mailbox. */
	virtual_mailboxes[mbxid].fd = -1;
	active_mailboxes[fd].refcount--;

	/* Release underlying hardware mailbox. */
	if (active_mailboxes[fd].refcount == 0)
		return (_do_mailbox_release(fd, mailbox_close));

	return (0);
}

/*============================================================================*
 * do_vmailbox_aread()                                                        *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_vmailbox_aread(int mbxid, void *buffer, size_t size)
{
	int ret;     /* HAL function return.                     */
	int fd;      /* Active mailbox index of virtual mailbox. */
	uint64_t t1; /* Clock value before aread call.           */
	uint64_t t2; /* Clock value after aread call.            */

	/* Invalid virtual mailbox. */
	if (!do_vmailbox_is_valid(mbxid))
		return (-EINVAL);

	fd = virtual_mailboxes[mbxid].fd;

	/* Bad virtual mailbox. */
	if (!resource_is_used(&active_mailboxes[fd].resource))
		return (-EBADF);

	/* Bad virtual mailbox. */
	if (!resource_is_readable(&active_mailboxes[fd].resource))
		return (-EBADF);

	resource_set_async(&active_mailboxes[fd].resource);

	t1 = clock_read();

		/* Setup asynchronous read. */
		if ((ret = mailbox_aread(active_mailboxes[fd].hwfd, buffer, size)) < 0)
			return (ret);

	t2 = clock_read();

	/* Update performance statistics. */
	virtual_mailboxes[mbxid].latency += (t2 - t1);
	virtual_mailboxes[mbxid].volume += ret;

	return (ret);
}

/*============================================================================*
 * do_vmailbox_awrite()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_vmailbox_awrite(int mbxid, const void * buffer, size_t size)
{
	int ret;     /* HAL function return.                     */
	int fd;      /* Active mailbox index of virtual mailbox. */
	uint64_t t1; /* Clock value before awrite call.          */
	uint64_t t2; /* Clock value after awrite call.           */

	/* Invalid virtual mailbox. */
	if (!do_vmailbox_is_valid(mbxid))
		return (-EINVAL);

	fd = virtual_mailboxes[mbxid].fd;

	/* Bad virtual mailbox. */
	if (!resource_is_used(&active_mailboxes[fd].resource))
		return (-EBADF);

	/* Bad virtual mailbox. */
	if (!resource_is_writable(&active_mailboxes[fd].resource))
		return (-EBADF);

	resource_set_async(&active_mailboxes[fd].resource);

	t1 = clock_read();

		/* Setup asynchronous write. */
		if ((ret = mailbox_awrite(active_mailboxes[fd].hwfd, buffer, size)) < 0)
			return (ret);

	t2 = clock_read();

	/* Update performance statistics. */
	virtual_mailboxes[mbxid].latency += (t2 - t1);
	virtual_mailboxes[mbxid].volume += ret;

	return (ret);
}

/*============================================================================*
 * do_vmailbox_wait()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_vmailbox_wait(int mbxid)
{
	int ret;     /* HAL function return.                     */
	int fd;      /* Active mailbox index of virtual mailbox. */
	uint64_t t1; /* Clock value before wait call.            */
	uint64_t t2; /* Clock value after wait call.             */

	/* Invalid virtual mailbox. */
	if (!do_vmailbox_is_valid(mbxid))
		return (-EINVAL);

	fd = virtual_mailboxes[mbxid].fd;

	/* Bad virtual mailbox. */
	if (!resource_is_async(&active_mailboxes[fd].resource))
		return (-EBADF);

	dcache_invalidate();

	t1 = clock_read();

		/* Wait for asynchronous operation. */
		ret = mailbox_wait(active_mailboxes[fd].hwfd);

	t2 = clock_read();

	/* Update performance statistics. */
	virtual_mailboxes[mbxid].latency += (t2 - t1);

	dcache_invalidate();
	return (ret);
}

/*============================================================================*
 * do_vmailbox_ioctl()                                                        *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
int do_vmailbox_ioctl(int mbxid, unsigned request, va_list args)
{
	int ret = 0;

	/* Invalid virtual mailbox. */
	if (!do_vmailbox_is_valid(mbxid))
		return (-EINVAL);

	/* Bad virtual mailbox. */
	if (!resource_is_used(&active_mailboxes[virtual_mailboxes[mbxid].fd].resource))
		return (-EBADF);

	/* Parse request. */
	switch (request)
	{
		/* Get the amount of data transferred so far. */
		case MAILBOX_IOCTL_GET_VOLUME:
		{
			size_t *volume;
			volume = va_arg(args, size_t *);
			*volume = virtual_mailboxes[mbxid].volume;
		} break;

		/* Get the cumulative transfer latency. */
		case MAILBOX_IOCTL_GET_LATENCY:
		{
			uint64_t *latency;
			latency = va_arg(args, uint64_t *);
			*latency = virtual_mailboxes[mbxid].latency;
		} break;

		/* Operation not supported. */
		default:
			ret = (-ENOTSUP);
			break;
	}

	return (ret);
}

#endif /* __TARGET_HAS_MAILBOX */
