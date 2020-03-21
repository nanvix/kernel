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

#include <nanvix/hal.h>
#include <nanvix/kernel/mailbox.h>
#include <nanvix/hlib.h>
#include <posix/errno.h>
#include <posix/stdarg.h>

#if __TARGET_HAS_MAILBOX

/**
 * @brief Search types for do_mailbox_search().
 */
enum mailbox_search_type {
	MAILBOX_SEARCH_INPUT = 0,
	MAILBOX_SEARCH_OUTPUT = 1
} resource_type_enum_t;

/**
 * @name Helper Macros for virtual mailboxes flags manipulation.
 */
/**@{*/

/**
 * @brief Virtual mailboxes flags.
 */
#define VMAILBOX_STATUS_USED (1 << 0) /**< Used vmailbox? */

/**
 * @brief Asserts if the virtual mailbox is used.
 */
#define VMAILBOX_IS_USED(vmbxid) \
	(virtual_mailboxes[vmbxid].status & VMAILBOX_STATUS_USED)
/**@}*/

/**
 * @name Helper Macros for logic addresses operations.
 */
/**@{*/

/**
 * @brief Composes the logic address based on @p mbxid @p port.
 */
#define DO_LADDRESS_COMPOSE(mbxid, port) \
	(mbxid * MAILBOX_PORT_NR + port)

/**
 * @brief Extracts mbxid and port from vmbxid.
 */
#define GET_LADDRESS_FD(vmbxid) \
	(vmbxid / MAILBOX_PORT_NR)

#define GET_LADDRESS_PORT(vmbxid) \
	 (vmbxid % MAILBOX_PORT_NR)
/**@}*/

/**
 * @name Helper Macros for ports status manipulation.
 */
/**@{*/

/**
 * @brief Mailbox ports flags.
 */
#define PORT_STATUS_USED (1 << 0) /**< Used port? */

/**
 * @brief Asserts if the port of mbxid is used.
 */
#define PORT_IS_USED(mbxid,port) \
	(active_mailboxes[mbxid].ports[port].status & PORT_STATUS_USED)
/**@}*/

/**
 * @name Helper Macros for message buffers.
 */
/**@{*/

/**
 * @brief Message buffers flags.
 */
#define MBUFFER_FLAGS_USED (1 << 0) /**< Used? */
#define MBUFFER_FLAGS_BUSY (1 << 1) /**< Busy? */

/**
 * @brief Asserts if the message buffer is in use / busy.
 */
#define MBUFFER_IS_USED(mbufferid) \
	(mailbox_message_buffers[mbufferid].flags & MBUFFER_FLAGS_USED)

#define MBUFFER_IS_BUSY(mbufferid) \
	(mailbox_message_buffers[mbufferid].flags & MBUFFER_FLAGS_BUSY)
/**@}*/

/*============================================================================*
 * Control Structures.                                                        *
 *============================================================================*/

/**
 * @brief Mailbox message buffer.
 */
struct mailbox_message_buffer
{
	unsigned short flags; /* Flags. */

	/**
	 * @brief Structure that holds a message.
	 *
	 * @note Parameters aside the data buffer must be included in header size
	 * on include/nanvix/kernel/mailbox.h -> KMAILBOX_MESSAGE_HEADER_SIZE.
	 */
	struct mailbox_message
	{
		int dest; /* Data destination. */
		char data[KMAILBOX_MESSAGE_SIZE];
	} message;
};

struct mailbox_message_buffer mailbox_message_buffers[KMAILBOX_MESSAGE_BUFFERS_MAX] = {
	[0 ... (KMAILBOX_MESSAGE_BUFFERS_MAX - 1)] = {
		.flags   = 0,
		.message = {
			.dest = -1,
			.data = {'\0'},
		},
	},
};

/**
 * @brief Struct that represents a port abstraction.
 */
struct port
{
	unsigned short status;                  /* Port status.           */
	struct mailbox_message_buffer *mbuffer; /* Kernel buffer pointer. */
};

/**
 * @brief Table of virtual mailboxes.
 */
PRIVATE struct
{
	/**
	 * @name Control Variables
	 */
	/**@{*/
	unsigned short status; /**< Status.            */
	int remote;            /**< Remote address.    */
	void *user_buffer;     /**< User level buffer. */
	/**@}*/

	/**
	 * @name Performance Statistics
	 */
	/**@{*/
	size_t volume;    /**< Amount of data transferred. */
	uint64_t latency; /**< Transfer latency.           */
	/**@}*/
} ALIGN(sizeof(dword_t)) virtual_mailboxes[KMAILBOX_MAX] = {
	[0 ... (KMAILBOX_MAX - 1)] = {
		.status =  0,
		.remote = -1,
		.user_buffer = NULL
	},
};

/**
 * @brief Table of active mailboxes.
 */
PRIVATE struct mailbox
{
	struct resource resource;           /**< Underlying resource.        */
	int refcount;                       /**< References count.           */
	int hwfd;                           /**< Underlying file descriptor. */
	int nodenum;                        /**< Target node number.         */
	struct port ports[MAILBOX_PORT_NR]; /**< Logic ports.                */
} active_mailboxes[HW_MAILBOX_MAX] = {
	[0 ... (HW_MAILBOX_MAX - 1)] {
		.ports[0 ... (MAILBOX_PORT_NR - 1)] = {
			.status  = 0,
			.mbuffer = NULL,
		},
		.nodenum = -1,
	},
};

/**
 * @brief Resource pool.
 */
PRIVATE const struct resource_pool mbxpool = {
	active_mailboxes, HW_MAILBOX_MAX, sizeof(struct mailbox)
};

/*============================================================================*
 * do_vmailbox_alloc()                                                        *
 *============================================================================*/

/**
 * @brief Searches for a free virtual mailbox.
 *
 * @param mbxid Target mailbox ID.
 * @param port  Target port number on @p mbxid.
 *
 * @returns Upon successful completion, the index of the virtual
 * mailbox in virtual_mailboxes tab is returned. Upon failure,
 * a negative number is returned instead.
 */
PRIVATE int do_vmailbox_alloc(int mbxid, int port)
{
	int vmbxid = DO_LADDRESS_COMPOSE(mbxid, port);

	if (VMAILBOX_IS_USED(vmbxid))
		return (-1);

	/* Initialize the vmailbox. */
	virtual_mailboxes[vmbxid].status |= VMAILBOX_STATUS_USED;
	virtual_mailboxes[vmbxid].volume  = 0ULL;
	virtual_mailboxes[vmbxid].latency = 0ULL;

	return (vmbxid);
}

/*============================================================================*
 * do_port_alloc()                                                            *
 *============================================================================*/

/**
 * @brief Searches for a free port on @p mbxid.
 *
 * @param mbxid ID of the target HW mailbox.
 *
 * @returns Upon successful completion, the index of the available port is
 * returned. A negative number is returned instead.
 */
PRIVATE int do_port_alloc(int mbxid)
{
	/* Checks if can exist an available port. */
	if (active_mailboxes[mbxid].refcount == MAILBOX_PORT_NR)
		goto error;

	/* Searches for a free port on the target mailbox. */
	for (unsigned int i = 0; i < MAILBOX_PORT_NR; ++i)
	{
		if (!PORT_IS_USED(mbxid, i))
			return (i);
	}

error:
	return (-1);
}

/*============================================================================*
 * do_mbuffer_alloc()                                                         *
 *============================================================================*/

/**
 * @brief Allocates a message buffer from message_buffers tab.
 *
 * @return Upon successful completion, the index of the allocated buffer
 * on message_buffer tab is returned. Upon failure, a negative number is
 * returned instead.
 */
PRIVATE int do_mbuffer_alloc(void)
{
	for (unsigned int i = 0; i < KMAILBOX_MESSAGE_BUFFERS_MAX; ++i)
	{
		if (!(MBUFFER_IS_USED(i) || MBUFFER_IS_BUSY(i)))
		{
			mailbox_message_buffers[i].flags |= MBUFFER_FLAGS_USED;

			return (i);
		}
	}

	return (-1);
}

/*============================================================================*
 * do_mbuffer_free()                                                          *
 *============================================================================*/

/**
 * @brief Releases a message buffer from message_buffers tab.
 *
 * @param buffer The target buffer to be freed.
 *
 * @return Upon successful completion, zero is returned. Upon failure,
 * a negative number is returned instead.
 */
PRIVATE int do_mbuffer_free(struct mailbox_message_buffer * buffer)
{
	/* Bad mbuffer. */
	if (buffer == NULL)
		return (-EINVAL);

	/* Buffer have data to be read. */
	if (buffer->flags & MBUFFER_FLAGS_BUSY)
		return (-EBUSY);

	/* Resets mbuffer original status. */
	buffer->flags = 0;
	buffer->message.dest = -1;
	buffer->message.data[0] = '\0';

	return (0);
}

/*============================================================================*
 * do_message_search()                                                        *
 *============================================================================*/

/**
 * @brief Searches for a stored message destinated to @p local_address.
 *
 * @param local_address Local HW address for which the messages come.
 *
 * @returns Upon successful completion, the mbuffer that contains the first
 * message found is returned. A negative error number is returned instead.
 */
PRIVATE int do_message_search(int local_address)
{
	for (unsigned int i = 0; i < KMAILBOX_MESSAGE_BUFFERS_MAX; ++i)
	{
		/* Is the buffer being used by another mailbox? */
		if (MBUFFER_IS_USED(i))
			continue;

		/* The buffer contains a stored message? */
		if (!MBUFFER_IS_BUSY(i))
			continue;

		/* Is this message addressed to the local_address? */
		if (mailbox_message_buffers[i].message.dest != local_address)
			continue;

		return (i);
	}

	/* No message encountered. */
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
 * @brief Searches for an active HW mailbox.
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
	for (int i = 0; i < HW_MAILBOX_MAX; ++i)
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
	int hwfd;    /* File descriptor.      */
	int mbxid;   /* Mailbox ID.           */

	/* Search target hardware mailbox. */
	if ((mbxid = do_mailbox_search(local, MAILBOX_SEARCH_INPUT)) >= 0)
		return (-EBUSY);

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
 * @param port  Target port in @p local node.
 *
 * @returns Upon successful completion, the ID of the newly created
 * virtual mailbox is returned. Upon failure, a negative error code
 * is returned instead.
 */
PUBLIC int do_vmailbox_create(int local, int port)
{
	int mbxid;  /* Hardware mailbox ID. */
	int vmbxid; /* Virtual mailbox ID.  */

	/* Checks if the input mailbox is local. */
	if (!node_is_local(local))
		return (-EINVAL);

	/* Search target hardware mailbox. */
	if ((mbxid = do_mailbox_search(local, MAILBOX_SEARCH_INPUT)) < 0)
		return (-EAGAIN);

	/* Allocate a virtual mailbox. */
	if ((vmbxid = do_vmailbox_alloc(mbxid, port)) < 0)
		return (-EBUSY);

	/* Initialize virtual mailbox. */
	active_mailboxes[mbxid].ports[port].status |= PORT_STATUS_USED;
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

	hwfd = -1;

	if (!node_is_local(remote))
	{
		/* Open underlying output hardware mailbox. */
		if ((hwfd = mailbox_open(remote)) < 0)
		{
			resource_free(&mbxpool, mbxid);
			return (hwfd);
		}
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
 * @param remote      Logic ID of the target remote node.
 * @param remote_port Target port in @p remote node.
 *
 * @returns Upon successful completion, the ID of the newly opened
 * virtual mailbox is returned. Upon failure, a negative error code
 * is returned instead.
 */
PUBLIC int do_vmailbox_open(int remote, int remote_port)
{
	int mbxid;  /* Hardware mailbox ID.          */
	int vmbxid; /* Virtual mailbox ID.           */
	int port;   /* Port allocated in local node. */

	/* Search target hardware mailbox. */
	if ((mbxid = do_mailbox_search(remote, MAILBOX_SEARCH_OUTPUT)) < 0)
		return (-EAGAIN);

	/* Allocates a free port in the HW mailbox. */
	if ((port = do_port_alloc(mbxid)) < 0)
		return (-EAGAIN);

	/* Allocate a virtual mailbox. */
	if ((vmbxid = do_vmailbox_alloc(mbxid, port)) < 0)
		return (-EBUSY);

	/* Initialize virtual mailbox. */
	virtual_mailboxes[vmbxid].remote = DO_LADDRESS_COMPOSE(remote, remote_port);
	active_mailboxes[mbxid].ports[port].status |= PORT_STATUS_USED;
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
	int ret;  /* HAL function return.   */
	int hwfd; /* HWFD allocated on HAL. */

	/* Checks if there is a hwfd allocated to this mailbox. */
	if ((hwfd = active_mailboxes[mbxid].hwfd) >= 0)
	{
		if ((ret = release_fn(hwfd)) < 0)
			return (ret);
	}

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
	int fd;              /* Active mailbox logic ID. */
	int port;            /* Vmbx logic port ID.      */
	int local_hwaddress; /* Local HW address.        */
	int mbuffer;         /* Busy mbuffer.            */

	/* Bad virtual mailbox. */
	if (!VMAILBOX_IS_USED(mbxid))
		return (-EBADF);

	fd = GET_LADDRESS_FD(mbxid);

	/* Bad mailbox. */
	if (!resource_is_used(&active_mailboxes[fd].resource))
		return (-EBADF);

	/* Bad mailbox. */
	if (!resource_is_readable(&active_mailboxes[fd].resource))
		return (-EBADF);

	port = GET_LADDRESS_PORT(mbxid);

	local_hwaddress = DO_LADDRESS_COMPOSE(active_mailboxes[fd].nodenum, port);

	/* Check if exist pending messages for this port. */
	if ((mbuffer = do_message_search(local_hwaddress)) >= 0)
		return (-EBUSY);

	/* Unlink virtual mailbox. */
	virtual_mailboxes[mbxid].status = 0;
	active_mailboxes[fd].ports[port].status &= ~PORT_STATUS_USED;
	active_mailboxes[fd].refcount--;

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
	int fd;   /* Active mailbox logic ID.       */
	int port; /* Virtual mailbox logic port ID. */

	/* Bad virtual mailbox. */
	if (!VMAILBOX_IS_USED(mbxid))
		return (-EBADF);

	fd = GET_LADDRESS_FD(mbxid);

	/* Bad mailbox. */
	if (!resource_is_used(&active_mailboxes[fd].resource))
		return (-EBADF);

	/* Bad mailbox. */
	if (!resource_is_writable(&active_mailboxes[fd].resource))
		return (-EBADF);

	port = GET_LADDRESS_PORT(mbxid);

	/* Unlink virtual mailbox. */
	virtual_mailboxes[mbxid].status =  0;
	virtual_mailboxes[mbxid].remote = -1;
	active_mailboxes[fd].ports[port].status &= ~PORT_STATUS_USED;
	active_mailboxes[fd].refcount--;

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
	int ret;             /* HAL function return.           */
	int fd;              /* HW mailbox logic index.        */
	int port;            /* Port used by vmailbox.         */
	int local_hwaddress; /* Vmailbox hardware address.     */
	int mbuffer;         /* New alocated buffer.           */
	uint64_t t1;         /* Clock value before aread call. */
	uint64_t t2;         /* Clock value after aread call.  */
	struct mailbox_message_buffer *buffer_ptr;

	/* Bad virtual mailbox. */
	if (!VMAILBOX_IS_USED(mbxid))
		return (-EBADF);

	fd = GET_LADDRESS_FD(mbxid);

	/* Bad mailbox. */
	if (!resource_is_used(&active_mailboxes[fd].resource))
		return (-EBADF);

	/* Bad mailbox. */
	if (!resource_is_readable(&active_mailboxes[fd].resource))
		return (-EBADF);

	port = GET_LADDRESS_PORT(mbxid);

	local_hwaddress = DO_LADDRESS_COMPOSE(active_mailboxes[fd].nodenum, port);

	resource_set_async(&active_mailboxes[fd].resource);

	/* Is there a pending message for this vmailbox? */
	if ((mbuffer = do_message_search(local_hwaddress)) >= 0)
	{
		buffer_ptr = &mailbox_message_buffers[mbuffer];

		t1 = clock_read();
			kmemcpy(buffer, (void *) &buffer_ptr->message.data, ret = size);
		t2 = clock_read();

		ret = size;

		/* Update performance statistics. */
		virtual_mailboxes[mbxid].latency += (t2 - t1);
		virtual_mailboxes[mbxid].volume  += ret;

		goto release_buffer;
	}

	/* Alloc data buffer to receive data. */
	if ((mbuffer = do_mbuffer_alloc()) < 0)
		return (-EAGAIN);

	active_mailboxes[fd].ports[port].mbuffer = &mailbox_message_buffers[mbuffer];
	buffer_ptr = active_mailboxes[fd].ports[port].mbuffer;

	/* Sets the receiver buffer as busy. */
	buffer_ptr->flags |= MBUFFER_FLAGS_BUSY;

	dcache_invalidate();

	t1 = clock_read();

		/* Setup asynchronous read. */
		if ((ret = mailbox_aread(active_mailboxes[fd].hwfd, (void *) &buffer_ptr->message, HAL_MAILBOX_MSG_SIZE)) < 0)
			goto release_buffer;

	t2 = clock_read();

	virtual_mailboxes[mbxid].user_buffer = buffer;

	/* Update performance statistics. */
	virtual_mailboxes[mbxid].latency += (t2 - t1);

	return (ret);

release_buffer:
	/* Sets the mailbox data buffer as not busy. */
	buffer_ptr->flags &= ~MBUFFER_FLAGS_BUSY;

	KASSERT(do_mbuffer_free(buffer_ptr) == 0);
	active_mailboxes[fd].ports[port].mbuffer = NULL;

	dcache_invalidate();
	return (ret);
}

/*============================================================================*
 * do_vmailbox_awrite()                                                       *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_vmailbox_awrite(int mbxid, const void * buffer, size_t size)
{
	int ret;     /* HAL function return.            */
	int fd;      /* Hardware mailbox logic index.   */
	int port;    /* HW port specified to vmailbox.  */
	int mbuffer; /* Message buffer used to write.   */
	uint64_t t1; /* Clock value before awrite call. */
	uint64_t t2; /* Clock value after awrite call.  */

	/* Bad virtual mailbox. */
	if (!VMAILBOX_IS_USED(mbxid))
		return (-EBADF);

	fd = GET_LADDRESS_FD(mbxid);

	/* Bad virtual mailbox. */
	if (!resource_is_used(&active_mailboxes[fd].resource))
		return (-EBADF);

	/* Bad virtual mailbox. */
	if (!resource_is_writable(&active_mailboxes[fd].resource))
		return (-EBADF);

	port = GET_LADDRESS_PORT(mbxid);

	/* Allocate the message buffer that will be used to write. */
	if (active_mailboxes[fd].ports[port].mbuffer != NULL)
		goto write;

	/* Allocates a message buffer to send the message. */
	if ((mbuffer = do_mbuffer_alloc()) < 0)
		return (mbuffer);

	active_mailboxes[fd].ports[port].mbuffer = &mailbox_message_buffers[mbuffer];

	resource_set_async(&active_mailboxes[fd].resource);

	active_mailboxes[fd].ports[port].mbuffer->message.dest = virtual_mailboxes[mbxid].remote;

	t1 = clock_read();
		kmemcpy((void *) &active_mailboxes[fd].ports[port].mbuffer->message.data, buffer, size);
	t2 = clock_read();

	/* Checks if the destination is the local node. */
	if (node_is_local(active_mailboxes[fd].nodenum))
	{
		/* Forwards the message to the mbuffers table. */
		active_mailboxes[fd].ports[port].mbuffer->flags = 0 | MBUFFER_FLAGS_BUSY;
		active_mailboxes[fd].ports[port].mbuffer = NULL;

		goto finish;
	}

write:
	t1 = clock_read();

		/* Setup asynchronous write. */
		if ((ret = mailbox_awrite(active_mailboxes[fd].hwfd, (void *) &active_mailboxes[fd].ports[port].mbuffer->message, HAL_MAILBOX_MSG_SIZE)) < 0)
			return (ret);

	t2 = clock_read();

finish:
	ret = size;

	/* Update performance statistics. */
	virtual_mailboxes[mbxid].latency += (t2 - t1);
	virtual_mailboxes[mbxid].volume  += ret;

	return (ret);
}

/*============================================================================*
 * do_vmailbox_wait()                                                         *
 *============================================================================*/

/**
 * @brief Wait implementation for input mailboxes. Waits on a virtual
 * mailbox to finish an asynchronous read.
 *
 * @param mbxid Logic ID of the target virtual mailbox.
 *
 * @returns Upon successful completion, zero is returned if the operation
 * finished on @p mbxid. If the operation incurred in a re-addressing, ONE
 * is returned. Upon failure, a negative error code is returned instead.
 */
PRIVATE int do_vmailbox_receiver_wait(int mbxid)
{
	int ret;             /* HAL function return.          */
	int fd;              /* Vmailbox file descriptor.     */
	uint64_t t1;         /* Clock value before wait call. */
	uint64_t t2;         /* Clock value after wait call.  */
	int port;            /* Port used by vmailbox.        */
	int dest;            /* Msg destination address.      */
	int local_hwaddress; /* Vmailbox hardware address.    */
	struct mailbox_message_buffer *buffer_ptr;

	fd = GET_LADDRESS_FD(mbxid);
	port = GET_LADDRESS_PORT(mbxid);

	buffer_ptr = active_mailboxes[fd].ports[port].mbuffer;

	t1 = clock_read();

		/* Wait for asynchronous read to finish. */
		if ((ret = mailbox_wait(active_mailboxes[fd].hwfd)) < 0)
			goto release_buffer;

	t2 = clock_read();

	local_hwaddress = DO_LADDRESS_COMPOSE(active_mailboxes[fd].nodenum, port);

	/* Checks if the message is addressed for the requesting port. */
	dest = buffer_ptr->message.dest;

	if (dest != local_hwaddress)
	{
		/* Check if the destination port is opened to receive data. */
		if (PORT_IS_USED(fd, GET_LADDRESS_PORT(dest)))
		{
			/* Marks the buffer as not used for threads look for this message. */
			buffer_ptr->flags &= ~MBUFFER_FLAGS_USED;
		}
		else
		{
			/* Discards the message. */
			buffer_ptr->flags &= ~MBUFFER_FLAGS_BUSY;

			KASSERT(do_mbuffer_free(buffer_ptr) == 0);
		}

		active_mailboxes[fd].ports[port].mbuffer = NULL;

		/* Returns sinalizing that a message was read, but not for local port. */
		return (1);
	}
	else
	{
		kmemcpy(virtual_mailboxes[mbxid].user_buffer, (void *) &buffer_ptr->message.data, KMAILBOX_MESSAGE_SIZE);

		/* Update performance statistics. */
		virtual_mailboxes[mbxid].latency += (t2 - t1);
		virtual_mailboxes[mbxid].volume  += KMAILBOX_MESSAGE_SIZE;

		virtual_mailboxes[mbxid].user_buffer = NULL;

		ret = 0;
	}

release_buffer:
	/* Sets the mailbox data buffer as not busy. */
	buffer_ptr->flags &= ~MBUFFER_FLAGS_BUSY;

	KASSERT(do_mbuffer_free(buffer_ptr) == 0);
	active_mailboxes[fd].ports[port].mbuffer = NULL;

	return (ret);
}

/**
 * @brief Wait implementation for output mailboxes. Waits on a virtual
 * mailbox to finish an asynchronous write.
 *
 * @param mbxid Logic ID of the sender virtual mailbox.
 *
 * @returns Upon successful completion, zero is returned. Upon failure,
 * a negative error code is returned instead.
 */
PRIVATE int do_vmailbox_sender_wait(int mbxid)
{
	int ret;     /* HAL function return.          */
	int fd;      /* Vmailbox file descriptor.     */
	int port;    /* Port used by vmailbox.        */
	uint64_t t1; /* Clock value before wait call. */
	uint64_t t2; /* Clock value after wait call.  */

	fd = GET_LADDRESS_FD(mbxid);
	port = GET_LADDRESS_PORT(mbxid);

	t1 = clock_read();

		/* Wait for asynchronous write to finish. */
		if ((ret = mailbox_wait(active_mailboxes[fd].hwfd)) < 0)
			goto release_buffer;

	t2 = clock_read();

	/* Update performance statistics. */
	virtual_mailboxes[mbxid].latency += (t2 - t1);

release_buffer:
	/* Releases the buffer allocated to write. */
	KASSERT(do_mbuffer_free(active_mailboxes[fd].ports[port].mbuffer) == 0);
	active_mailboxes[fd].ports[port].mbuffer = NULL;

	return (ret);
}

/**
 * @brief Waits on a virtual mailbox to finish an assynchronous operation.
 *
 * @param mbxid Logic ID of the target virtual mailbox.
 *
 * @returns Upon successful completion, a positive number is returned.
 * Upon failure, a negative error code is returned instead.
 */
PUBLIC int do_vmailbox_wait(int mbxid)
{
	int ret;             /* HAL function return.      */
	int fd;              /* Vmailbox file descriptor. */
	int port;            /* Port used by vmailbox.    */
	int (*wait_fn)(int); /* Underlying wait function. */

	/* Bad virtual mailbox. */
	if (!VMAILBOX_IS_USED(mbxid))
		return (-EBADF);

	fd = GET_LADDRESS_FD(mbxid);
	port = GET_LADDRESS_PORT(mbxid);

	/* Bad mailbox. */
	if (!resource_is_used(&active_mailboxes[fd].resource))
		return (-EBADF);

	/* Checks the underlying mailbox type. */
	if (resource_is_readable(&active_mailboxes[fd].resource))
		wait_fn = do_vmailbox_receiver_wait;
	else if (resource_is_writable(&active_mailboxes[fd].resource))
		wait_fn = do_vmailbox_sender_wait;
	else
		return (-EBADF);

	/* Bad virtual mailbox. */
	if (!resource_is_async(&active_mailboxes[fd].resource))
		return (-EBADF);

	/* Previous operation already completed. */
	if (active_mailboxes[fd].ports[port].mbuffer == NULL)
		return (0);

	dcache_invalidate();

	/* Calls the underlying wait function according to the type of the mailbox. */
	ret = wait_fn(mbxid);

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
	int fd;

	/* Bad virtual mailbox. */
	if (!VMAILBOX_IS_USED(mbxid))
		return (-EBADF);

	fd = GET_LADDRESS_FD(mbxid);

	/* Bad virtual mailbox. */
	if (!resource_is_used(&active_mailboxes[fd].resource))
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

/*============================================================================*
 * kmailbox_init()                                                            *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC void kmailbox_init(void)
{
	int local;

	kprintf("[kernel][noc] initializing the kmailbox facility");

	local = processor_node_get_num();

	/* Create the input mailbox. */
	KASSERT(_do_mailbox_create(local) >= 0);

	/* Opens all mailbox interfaces. */
	for (int i = 0; i < PROCESSOR_NOC_NODES_NUM; ++i)
		KASSERT(_do_mailbox_open(i) >= 0);
}

#endif /* __TARGET_HAS_MAILBOX */
