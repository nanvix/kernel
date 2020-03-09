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
#define PORT_STATUS_USED (1 << 0) /**< Used port?   */

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

/**
 * @name Helper Macros for mailboxes operations.
 */
/**@{*/

/**
 * @brief Asserts if the mailbox data buffer is busy.
 */
#define MAILBOX_IS_BUSY(mbxid) \
	(active_mailboxes[mbxid].buffer->flags & MBUFFER_FLAGS_BUSY)

/**
 * @brief Sets the mailbox data buffer as busy / notbusy.
 */
#define MAILBOX_SET_BUSY(mbxid) \
	(active_mailboxes[mbxid].buffer->flags |= MBUFFER_FLAGS_BUSY)

#define MAILBOX_SET_NOTBUSY(mbxid) \
	(active_mailboxes[mbxid].buffer->flags &= ~MBUFFER_FLAGS_BUSY)

/**@}*/

/*============================================================================*
 * Control Structures.                                                        *
 *============================================================================*/

/**
 * @brief Struct that represents a mailbox message buffer.
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
		.message = {
			.dest = KMAILBOX_MAX,
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
	unsigned short status; /**< Status.         */
	int remote;            /**< Remote address. */
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
		.remote = -1
	},
};

/**
 * @brief Table of active mailboxes.
 */
PRIVATE struct mailbox
{
	struct resource resource;              /**< Underlying resource.        */
	int refcount;                          /**< References count.           */
	int hwfd;                              /**< Underlying file descriptor. */
	int nodenum;                           /**< Target node number.         */
	struct port ports[MAILBOX_PORT_NR];    /**< Logic ports.                */
	struct mailbox_message_buffer *buffer; /**< Data Buffer resource.       */
} active_mailboxes[HW_MAILBOX_MAX] = {
	[0 ... (HW_MAILBOX_MAX - 1)] {
		.ports[0 ... (MAILBOX_PORT_NR - 1)] = {
			.status = 0,
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
 * @brief Searches for a free port on a HW mailbox.
 *
 * @param mbxid ID of the target HW mailbox.
 *
 * @returns Upon successful completion, the index of the available
 * port is returned. Upon failure, a negative number is returned
 * instead.
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
	buffer->message.dest = KMAILBOX_MAX;
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
	int mbuffer; /* Allocated mbuffer ID. */

	/* Search target hardware mailbox. */
	if ((mbxid = do_mailbox_search(local, MAILBOX_SEARCH_INPUT)) >= 0)
		return (mbxid);

	/* Allocate data buffer. */
	if ((mbuffer = do_mbuffer_alloc()) < 0)
		return (-EAGAIN);

	/* Allocate resource. */
	if ((mbxid = resource_alloc(&mbxpool)) < 0)
	{
		do_mbuffer_free(&mailbox_message_buffers[mbuffer]);
		return (-EAGAIN);
	}

	/* Create underlying input hardware mailbox. */
	if ((hwfd = mailbox_create(local)) < 0)
	{
		do_mbuffer_free(&mailbox_message_buffers[mbuffer]);
		resource_free(&mbxpool, mbxid);
		return (hwfd);
	}

	/* Initialize hardware mailbox. */
	active_mailboxes[mbxid].hwfd     = hwfd;
	active_mailboxes[mbxid].refcount = 0;
	active_mailboxes[mbxid].nodenum  = local;
	active_mailboxes[mbxid].buffer   = &mailbox_message_buffers[mbuffer];
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

	/* Create hardware mailbox. */
	if ((mbxid = _do_mailbox_create(local)) < 0)
		return (mbxid);

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
	int hwfd = -1; /* File descriptor. */
	int mbxid;     /* Mailbox ID.      */

	/* Search target hardware mailbox. */
	if ((mbxid = do_mailbox_search(remote, MAILBOX_SEARCH_OUTPUT)) >= 0)
		return (mbxid);

	/* Allocate resource. */
	if ((mbxid = resource_alloc(&mbxpool)) < 0)
		return (-EAGAIN);

	/* Checks if the remote is not the local node. */
#ifdef __mppa256__
	if (remote != processor_node_get_num(0) || cluster_is_iocluster(cluster_get_num()))
#else
	if (remote != processor_node_get_num(0))
#endif
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

	/* Create hardware mailbox. */
	if ((mbxid = _do_mailbox_open(remote)) < 0)
		return (mbxid);

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

	/* Check if there is a data buffer allocated. */
	if (active_mailboxes[mbxid].buffer != NULL)
	{
		if ((ret = do_mbuffer_free(active_mailboxes[mbxid].buffer)) < 0)
			return (ret);
	}

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

	/* Release underlying hardware mailbox. */
	if (active_mailboxes[fd].refcount == 0)
		return (_do_mailbox_release(fd, mailbox_close));

	return (0);
}

/*============================================================================*
 * do_vmailbox_aread()                                                        *
 *============================================================================*/

/**
 * @brief Allocs a new message buffer and store the old
 * on message_buffers tab.
 *
 * @param fd Busy HW mailbox.
 *
 * @returns Upon successful completion, zero is returned.
 * A negative error number is returned instead.
 */
PRIVATE int do_message_store(int fd)
{
	int dest;    /* Message destination.   */
	int port;    /* Target port.           */
	int mbuffer; /* New mbuffer allocated. */

	dest = active_mailboxes[fd].buffer->message.dest;
	port = GET_LADDRESS_PORT(dest);

	/* Check if the destination port is opened to receive data. */
	if (PORT_IS_USED(fd, port))
	{
		/* Allocate a message_buffer to hold the message. */
		if ((mbuffer = do_mbuffer_alloc()) < 0)
			return (-EBUSY);

		/* Switch the mailbox buffer to use an empty one. */
		active_mailboxes[fd].buffer->flags &= ~MBUFFER_FLAGS_USED;
		active_mailboxes[fd].buffer = &mailbox_message_buffers[mbuffer];
	}
	/**
	 * XXX - ELSE Discards the message.
	 */

	return (0);
}

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_vmailbox_aread(int mbxid, void *buffer, size_t size)
{
	int ret;             /* HAL function return.           */
	int fd;              /* HW mailbox logic index.        */
	int port;            /* Port used by vmailbox.         */
	int dest;            /* Msg destination address.       */
	int local_hwaddress; /* Vmailbox hardware address.     */
	int mbuffer;         /* New alocated buffer.           */
	struct mailbox_message_buffer *aux_buffer_ptr;
	uint64_t t1;         /* Clock value before aread call. */
	uint64_t t2;         /* Clock value after aread call.  */

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
		aux_buffer_ptr = &mailbox_message_buffers[mbuffer];

		t1 = clock_read();
			kmemcpy(buffer, (void *) &aux_buffer_ptr->message.data, ret = size);
		t2 = clock_read();

		aux_buffer_ptr->flags &= ~MBUFFER_FLAGS_BUSY;

		/* Releases the message buffer. */
		KASSERT(do_mbuffer_free(aux_buffer_ptr) == 0);

		goto finish;
	}

	/* There is a pending read in HW mailbox data buffer. */
	if (MAILBOX_IS_BUSY(fd))
	{
		dest = active_mailboxes[fd].buffer->message.dest;
		if (dest == local_hwaddress) {
			t1 = clock_read();
				kmemcpy(buffer, (void *) &active_mailboxes[fd].buffer->message.data, ret = size);
			t2 = clock_read();

			goto unlock_mailbox;
		}
		else if ((ret = do_message_store(fd)) < 0)
			return (ret);
	}

again:
	/* Sets the mailbox data buffer as busy. */
	MAILBOX_SET_BUSY(fd);

	dcache_invalidate();

	t1 = clock_read();

		/* Setup asynchronous read. */
		if ((ret = mailbox_aread(active_mailboxes[fd].hwfd, (void *) &active_mailboxes[fd].buffer->message, MAILBOX_MSG_SIZE)) < 0)
			goto error;

		if ((ret = mailbox_wait(active_mailboxes[fd].hwfd)) < 0)
			goto error;

	t2 = clock_read();

	/* Checks if the message is addressed for the requesting port. */
	dest = active_mailboxes[fd].buffer->message.dest;
	if (dest != local_hwaddress)
	{
		if ((ret = do_message_store(fd)) < 0)
			return (ret);

		goto again;
	}

	ret = size;

	kmemcpy(buffer, (void *) &active_mailboxes[fd].buffer->message.data, size);

unlock_mailbox:
	/* Sets the mailbox data buffer as not busy. */
	MAILBOX_SET_NOTBUSY(fd);

finish:
	/* Update performance statistics. */
	virtual_mailboxes[mbxid].latency += (t2 - t1);
	virtual_mailboxes[mbxid].volume += ret;

	dcache_invalidate();
	return (ret);

error:
	MAILBOX_SET_NOTBUSY(fd);
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
	int ret;     /* HAL function return.            */
	int fd;      /* Hardware mailbox logic index.   */
	int port;    /* HW port specified to vmailbox.  */
	int mbuffer; /* Message buffer used to write.   */
	uint64_t t1 = 0; /* Clock value before awrite call. */
	uint64_t t2 = 0; /* Clock value after awrite call.  */

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

	if ((mbuffer = do_mbuffer_alloc()) < 0)
		return (mbuffer);

	active_mailboxes[fd].ports[port].mbuffer = &mailbox_message_buffers[mbuffer];

	resource_set_async(&active_mailboxes[fd].resource);

	active_mailboxes[fd].ports[port].mbuffer->message.dest = virtual_mailboxes[mbxid].remote;

	t1 = clock_read();
		kmemcpy((void *) &active_mailboxes[fd].ports[port].mbuffer->message.data, buffer, size);
	t2 = clock_read();

	/* Checks if the destination is the local node. */
	if (active_mailboxes[fd].nodenum == processor_node_get_num(0))
	{
		/* Forwards the message to the mbuffers table. */
		active_mailboxes[fd].ports[port].mbuffer->flags = 0 | MBUFFER_FLAGS_BUSY;
		active_mailboxes[fd].ports[port].mbuffer = NULL;

		goto finish;
	}

write:
	t1 = clock_read();

		/* Setup asynchronous write. */
		if ((ret = mailbox_awrite(active_mailboxes[fd].hwfd, (void *) &active_mailboxes[fd].ports[port].mbuffer->message, MAILBOX_MSG_SIZE)) < 0)
			return (ret);

		if ((ret = mailbox_wait(active_mailboxes[fd].hwfd)) < 0)
			return (ret);

	t2 = clock_read();

	/* Releases the buffer allocated. */
	do_mbuffer_free(active_mailboxes[fd].ports[port].mbuffer);
	active_mailboxes[fd].ports[port].mbuffer = NULL;

finish:
	ret = size;

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

	/* Bad virtual mailbox. */
	if (!VMAILBOX_IS_USED(mbxid))
		return (-EBADF);

	fd = GET_LADDRESS_FD(mbxid);

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

#endif /* __TARGET_HAS_MAILBOX */
