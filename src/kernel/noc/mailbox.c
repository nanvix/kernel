/*
 * MIT License
 *
 * Copyright(c) 2011-2018 Pedro Henrique Penna <pedrohenriquepenna@gmail.com>
 *              2015-2016 Davidson Francis     <davidsondfgl@gmail.com>
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
	int nodenum;              /**< Target's amount.            */
} ALIGN(sizeof(dword_t)) mbxtab[(MAILBOX_CREATE_MAX + MAILBOX_OPEN_MAX)];

/**
 * @brief Resource pool.
 */
PRIVATE const struct resource_pool mbxpool = {
	mbxtab, (MAILBOX_CREATE_MAX + MAILBOX_OPEN_MAX), sizeof(struct mailbox)
};

/*============================================================================*
 * _mailbox_is_valid()                                                        *
 *============================================================================*/

/**
 * @brief Asserts whether or not a synchronization point is valid.
 *
 * @param mbxid ID of the Target Mailbox.
 *
 * @returns One if the target synchronization point is valid, and false
 * otherwise.
 */
PRIVATE int _mailbox_is_valid(int mbxid)
{
	return WITHIN(mbxid, 0, (MAILBOX_CREATE_MAX + MAILBOX_OPEN_MAX));
}

/*============================================================================*
 * _mailbox_create()                                                          *
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
int _mailbox_create(int local)
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

	dcache_invalidate();

	return (mbxid);
}

/*============================================================================*
 * _mailbox_open()                                                            *
 *============================================================================*/

/**
 * @brief Opens a mailbox.
 *
 * @param remote Logic ID of the Target Node.
 *
 * @returns Upon successful completion, the ID of the target mailbox
 * is returned. Upon failure, a negative error code is returned instead.
 */
int _mailbox_open(int remote)
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

	dcache_invalidate();

	return (mbxid);
}

/*============================================================================*
 * _mailbox_unlink()                                                          *
 *============================================================================*/

/**
 * @brief Destroys a mailbox.
 *
 * @param mbxid ID of the Target Mailbox.
 *
 * @returns Upon successful completion, zero is returned. Upon failure,
 * a negative error code is returned instead.
 */
int _mailbox_unlink(int mbxid)
{
	int ret; /* HAL function return. */

	/* Invalid mailbox. */
	if (!_mailbox_is_valid(mbxid))
		return (-EBADF);

	/* Bad mailbox. */
	if (!resource_is_used(&mbxtab[mbxid].resource))
		return (-EAGAIN);

	/* Bad mailbox. */
	if (!resource_is_readable(&mbxtab[mbxid].resource))
		return (-EAGAIN);

	if ((ret = mailbox_unlink(mbxtab[mbxid].fd)) != 0)
		return (ret);

	mbxtab[mbxid].fd = -1;
	resource_free(&mbxpool, mbxid);

	return (0);
}

/*============================================================================*
 * _mailbox_close()                                                           *
 *============================================================================*/

/**
 * @brief Closes a mailbox.
 *
 * @param mbxid ID of the Target Mailbox.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
int _mailbox_close(int mbxid)
{
	int ret; /* HAL function return. */

	/* Invalid mailbox. */
	if (!_mailbox_is_valid(mbxid))
		return (-EBADF);

	/* Bad mailbox. */
	if (!resource_is_used(&mbxtab[mbxid].resource))
		return (-EAGAIN);

	/* Bad mailbox. */
	if (!resource_is_writable(&mbxtab[mbxid].resource))
		return (-EAGAIN);

	/* Closes mailbox. */
	if ((ret = mailbox_close(mbxtab[mbxid].fd)) != 0)
		return (ret);

	mbxtab[mbxid].fd = -1;
	resource_free(&mbxpool, mbxid);

	return (0);
}

/*============================================================================*
 * _mailbox_aread()                                                           *
 *============================================================================*/

/**
 * @brief Reads data from a mailbox.
 *
 * @param mbxid  ID of the Target Mailbox.
 * @param buffer Buffer where the data should be written to.
 * @param size   Number of bytes to read.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
int _mailbox_aread(int mbxid, void * buffer, size_t size)
{
	/* Invalid mailbox. */
	if (!_mailbox_is_valid(mbxid))
		return (-EBADF);

	/* Invalid buffer. */
	if (buffer == NULL)
		return (-EINVAL);

	/* Invalid read size. */
	if (size != MAILBOX_MSG_SIZE)
		return (-EINVAL);

	/* Bad mailbox. */
	if (!resource_is_used(&mbxtab[mbxid].resource))
		return (-EAGAIN);

	/* Bad mailbox. */
	if (!resource_is_readable(&mbxtab[mbxid].resource))
		return (-EBADF);

	return mailbox_aread(mbxtab[mbxid].fd, buffer, size);
}

/*============================================================================*
 * _mailbox_write()                                                           *
 *============================================================================*/

/**
 * @brief Writes data to a mailbox.
 *
 * @param mbxid  ID of the Target Mailbox.
 * @param buffer Buffer where the data should be read from.
 * @param size   Number of bytes to write.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
int _mailbox_awrite(int mbxid, const void * buffer, size_t size)
{
	/* Invalid mailbox. */
	if (!_mailbox_is_valid(mbxid))
		return (-EBADF);

	/* Invalid buffer. */
	if (buffer == NULL)
		return (-EINVAL);

	/* Invalid write size. */
	if (size != MAILBOX_MSG_SIZE)
		return (-EINVAL);

	/* Bad mailbox. */
	if (!resource_is_used(&mbxtab[mbxid].resource))
		return (-EAGAIN);

	/* Bad mailbox. */
	if (!resource_is_writable(&mbxtab[mbxid].resource))
		return (-EBADF);

	return mailbox_awrite(mbxtab[mbxid].fd, buffer, size);
}

/*============================================================================*
 * _mailbox_wait()                                                            *
 *============================================================================*/

/**
 * @brief Waits for an asynchronous operation on a mailbox to complete.
 *
 * @param mbxid ID of the Target Mailbox.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
int _mailbox_wait(int mbxid)
{
	/* Invalid mailbox. */
	if (!_mailbox_is_valid(mbxid))
		return (-EBADF);

	dcache_invalidate();

	/* Waits. */
	return mailbox_wait(mbxtab[mbxid].fd);
}

#endif /* __TARGET_HAS_MAILBOX */
