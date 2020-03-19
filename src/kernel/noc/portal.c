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
#include <nanvix/kernel/portal.h>
#include <nanvix/hlib.h>
#include <posix/errno.h>

#if __TARGET_HAS_PORTAL

/**
 * @brief Search types for do_portal_search().
 */
enum portal_search_type {
	PORTAL_SEARCH_INPUT = 0,
	PORTAL_SEARCH_OUTPUT = 1
} resource_type_enum_t;

/**
 * @name Helper Macros for virtual portals flags manipulation.
 */
/**@{*/

/**
 * @brief Virtual portal flags.
 */
#define VPORTAL_STATUS_USED    (1 << 0) /**< Used vportal? */
#define VPORTAL_STATUS_ALLOWED (1 << 1) /**< Allowed?      */

/**
 * @brief Asserts if the virtual portal is used.
 */
#define VPORTAL_IS_USED(vportalid) \
	(virtual_portals[vportalid].status & VPORTAL_STATUS_USED)

/**
 * @brief Asserts if the virtual portal is allowed.
 */
#define VPORTAL_IS_ALLOWED(vportalid) \
	(virtual_portals[vportalid].status & VPORTAL_STATUS_ALLOWED)
/**@}*/

/**
 * @name Helper Macros for logic addresses operations.
 */
/**@{*/

/**
 * @brief Composes the logic address based on portalid and one of it's ports.
 */
#define DO_LADDRESS_COMPOSE(portalid, port) \
	(portalid * KPORTAL_PORT_NR + port)

/**
 * @brief Extracts the portalid from vportalid.
 */
#define GET_LADDRESS_FD(vportalid) \
	(vportalid / KPORTAL_PORT_NR)

/**
 * @brief Extracts the port number from vportalid.
 */
#define GET_LADDRESS_PORT(vportalid) \
	 (vportalid % KPORTAL_PORT_NR)
/**@}*/

/**
 * @name Helper Macros for ports status manipulation.
 */
/**@{*/

/**
 * @brief Portal ports flags.
 */
#define PORT_STATUS_USED (1 << 0) /**< Used port?   */

/**
 * @brief Asserts if the port of portalid is used.
 */
#define PORT_IS_USED(portalid,port) \
	(active_portals[portalid].ports[port].status & PORT_STATUS_USED)
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
 * @brief Asserts if the message buffer is in use.
 */
#define MBUFFER_IS_USED(mbufferid) \
	(portal_message_buffers[mbufferid].flags & MBUFFER_FLAGS_USED)

/**
 * @brief Asserts if the message buffer is busy.
 */
#define MBUFFER_IS_BUSY(mbufferid) \
	(portal_message_buffers[mbufferid].flags & MBUFFER_FLAGS_BUSY)
/**@}*/

/**
 * @name Helper Macros for portal status manipulation.
 */
/**@{*/

/**
 * @brief Asserts if the portal data buffer is busy.
 */
#define PORTAL_IS_BUSY(portalid) \
	(active_portals[portalid].buffer->flags & MBUFFER_FLAGS_BUSY)

/**
 * @brief Sets the portal data buffer as busy / notbusy.
 */
#define PORTAL_SET_BUSY(portalid) \
	(active_portals[portalid].buffer->flags |= MBUFFER_FLAGS_BUSY)

#define PORTAL_SET_NOTBUSY(portalid) \
	(active_portals[portalid].buffer->flags &= ~MBUFFER_FLAGS_BUSY)
/**@}*/

/*============================================================================*
 * Control Structures.                                                        *
 *============================================================================*/

/**
 * @brief Struct that represents a portal message buffer.
 */
struct portal_message_buffer
{
	unsigned short flags; /* Flags. */

	/**
	 * @brief Structure that holds a message.
	 *
	 * @note Parameters aside the data buffer must be included in header size
	 * on include/nanvix/kernel/portal.h -> KPORTAL_MESSAGE_HEADER_SIZE.
	 */
	struct portal_message
	{
		int src;           /* Data sender.       */
		int dest;          /* Data destination.  */
		unsigned int size; /* Message data size. */
		char data[HAL_PORTAL_MAX_SIZE];
	} message;
};

struct portal_message_buffer portal_message_buffers[KPORTAL_MESSAGE_BUFFERS_MAX] = {
	[0 ... (KPORTAL_MESSAGE_BUFFERS_MAX - 1)] = {
		.message = {
			.src  = -1,
			.dest = -1,
			.size =  0,
			.data = {'\0'},
		},
	},
};

/**
 * @brief Struct that represents a port abstraction.
 */
struct port
{
	unsigned short status;                 /* Port status.           */
	struct portal_message_buffer *mbuffer; /* Kernel buffer pointer. */
};

/**
 * @brief Table of virtual portals.
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
} ALIGN(sizeof(dword_t)) virtual_portals[KPORTAL_MAX] = {
	[0 ... (KPORTAL_MAX - 1)] = {
		.status = 0,
		.remote = -1
	},
};

/**
 * @brief Table of active portals.
 */
PRIVATE struct portal
{
	struct resource resource;             /**< Underlying resource.        */
	int refcount;                         /**< References count.           */
	int hwfd;                             /**< Underlying file descriptor. */
	int local;                            /**< Local node number.          */
	int remote;                           /**< Target node number.         */
	struct port ports[KPORTAL_PORT_NR];   /**< HW ports.                   */
	struct portal_message_buffer *buffer; /**< Data buffer resource.       */
} ALIGN(sizeof(dword_t)) active_portals[HW_PORTAL_MAX] = {
	[0 ... (HW_PORTAL_MAX - 1)] {
		.ports[0 ... (KPORTAL_PORT_NR - 1)] = {
			.status = 0,
			.mbuffer = NULL
		},
		.local  = -1,
		.remote = -1,
	},
};

/**
 * @brief Resource pool.
 */
PRIVATE const struct resource_pool portalpool = {
	active_portals, HW_PORTAL_MAX, sizeof(struct portal)
};

/*============================================================================*
 * do_vportal_alloc()                                                         *
 *============================================================================*/

/**
 * @brief Searches for a free virtual portal.
 *
 * @param portalid ID of the target HW portal.
 * @param port     Port number in the specified portalid.
 *
 * @returns Upon successful completion, the index of the virtual
 * portal in virtual_portals tab is returned. Upon failure, a
 * negative number is returned instead.
 */
PRIVATE int do_vportal_alloc(int portalid, int port)
{
	int vportalid = DO_LADDRESS_COMPOSE(portalid, port);

	if (VPORTAL_IS_USED(vportalid))
		return (-1);

	/* Initialize the virtual portal. */
	virtual_portals[vportalid].status |= VPORTAL_STATUS_USED;
	virtual_portals[vportalid].volume  = 0ULL;
	virtual_portals[vportalid].latency = 0ULL;

	return (vportalid);
}

/*============================================================================*
 * do_port_alloc()                                                            *
 *============================================================================*/

/**
 * @brief Searches for a free port on a HW portal.
 *
 * @param portalid ID of the target HW portal.
 *
 * @returns Upon successful completion, the index of the available
 * port is returned. Upon failure, a negative number is returned
 * instead.
 */
PRIVATE int do_port_alloc(int portalid)
{
	/* Checks if can exist an available port. */
	if (active_portals[portalid].refcount == KPORTAL_PORT_NR)
		goto error;

	/* Searches for a free port on the target portal. */
	for (unsigned int i = 0; i < KPORTAL_PORT_NR; ++i)
	{
		if (!PORT_IS_USED(portalid, i))
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
	for (unsigned int i = 0; i < KPORTAL_MESSAGE_BUFFERS_MAX; ++i)
	{
		if (!(MBUFFER_IS_USED(i) || MBUFFER_IS_BUSY(i)))
		{
			portal_message_buffers[i].flags |= MBUFFER_FLAGS_USED;

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
PRIVATE int do_mbuffer_free(struct portal_message_buffer * buffer)
{
	/* Bad mbuffer. */
	if (buffer == NULL)
		return (-EINVAL);

	/* Buffer have data to be read. */
	if (buffer->flags & MBUFFER_FLAGS_BUSY)
		return (-EBUSY);

	/* Resets mbuffer original status. */
	buffer->flags = 0;
	buffer->message.src  = -1;
	buffer->message.dest = -1;
	buffer->message.size =  0;
	buffer->message.data[0] = '\0';

	return (0);
}

/*============================================================================*
 * do_message_search()                                                        *
 *============================================================================*/

/**
 * @brief Searches for a stored message sended by @p remote_address
 * to @p local_address.
 *
 * @param local_address  Local HW address for which the messages come.
 * @param remote_address Remote HW address from where the messages come.
 *
 * @returns Upon successful completion, the mbuffer that contains the first
 * message found is returned. A negative error number is returned instead.
 */
PRIVATE int do_message_search(int local_address, int remote_address)
{
	for (unsigned int i = 0; i < KPORTAL_MESSAGE_BUFFERS_MAX; ++i)
	{
		/* Is the buffer being used by another portal? */
		if (MBUFFER_IS_USED(i))
			continue;

		/* The buffer contains a stored message? */
		if (!MBUFFER_IS_BUSY(i))
			continue;

		/* Is this message addressed to the local_address? */
		if (portal_message_buffers[i].message.dest != local_address)
			continue;

		/* Is the message sender the expected? */
		if (remote_address != -1)
		{
			if (portal_message_buffers[i].message.src != remote_address)
				continue;
		}

		return (i);
	}

	/* No message encountered. */
	return (-1);
}

/*============================================================================*
 * portal_nodenum_is_local()                                                  *
 *============================================================================*/

/**
 * @brief Evaluates if the given nodenum is local to the cluster.
 *
 * @param nodenum Nodenum to be evaluated.
 *
 * @returns Non zero if @p nodenum is local to the current cluster. Zero is
 * returned instead.
 */
PUBLIC int portal_nodenum_is_local(int nodenum)
{
	int local;

	local = processor_node_get_num(0);

	if (cluster_is_ccluster(cluster_get_num()))
		return (nodenum == local);
	else
		return (WITHIN(nodenum, local, (local + (PROCESSOR_NOC_IONODES_NUM / PROCESSOR_IOCLUSTERS_NUM))));
}

/*============================================================================*
 * do_portal_search()                                                         *
 *============================================================================*/

/**
 * @name Helper Macros for do_portal_search()
 */
/**@{*/

/**
 * @brief Asserts an input portal.
 */
#define PORTAL_SEARCH_IS_INPUT(portalid,type) \
	((type == PORTAL_SEARCH_INPUT) && !resource_is_readable(&active_portals[portalid].resource))

/**
 * @brief Asserts an output portal.
 */
#define PORTAL_SEARCH_IS_OUTPUT(portalid,type) \
	 ((type == PORTAL_SEARCH_OUTPUT) && !resource_is_writable(&active_portals[portalid].resource))
/**@}*/

/**
 * @brief Searches for a hardware portal in active_portals table.
 *
 * @param local       Logic ID of the local node.
 * @param remote      Logic ID of the remote node.
 * @param search_type Type of the searched resource.
 *
 * @returns Upon successful completion, the ID of the found portal is
 * returned. Upon failure, a negative error code is returned instead.
 */
PRIVATE int do_portal_search(int local, int remote, enum portal_search_type search_type)
{
	for (unsigned i = 0; i < HW_PORTAL_MAX; ++i)
	{
		if (!resource_is_used(&active_portals[i].resource))
			continue;

		if (PORTAL_SEARCH_IS_INPUT(i, search_type))
			continue;

		else if (PORTAL_SEARCH_IS_OUTPUT(i, search_type))
			continue;

		/* Not the same local node? */
		if (active_portals[i].local != local)
			continue;

		/* Does it have a remote allowed? */
		if (active_portals[i].remote != remote)
			continue;

		return (i);
	}

	return (-1);
}

/*============================================================================*
 * do_vportal_create()                                                        *
 *============================================================================*/

/**
 * @brief Creates a hardware portal.
 *
 * @param local Logic ID of the Local Node.
 *
 * @returns Upon successful completion, the ID of a newly created hardware
 * portal is returned. Upon failure, a negative error code is returned
 * instead.
 */
PRIVATE int _do_portal_create(int local)
{
	int hwfd;     /* File descriptor. */
	int portalid; /* Portal ID.       */

	/* Search target hardware portal. */
	if ((portalid = do_portal_search(local, -1, PORTAL_SEARCH_INPUT)) >= 0)
		return (-EBUSY);

	/* Allocate resource. */
	if ((portalid = resource_alloc(&portalpool)) < 0)
		return (-EAGAIN);

	/* Create underlying input hardware portal. */
	if ((hwfd = portal_create(local)) < 0)
	{
		resource_free(&portalpool, portalid);
		return (hwfd);
	}

	/* Initialize portal. */
	active_portals[portalid].hwfd     = hwfd;
	active_portals[portalid].local    = local;
	active_portals[portalid].remote   = -1;
	active_portals[portalid].refcount = 0;
	resource_set_rdonly(&active_portals[portalid].resource);
	resource_set_notbusy(&active_portals[portalid].resource);

	return (portalid);
}

/**
 * @brief Creates a virtual portal.
 *
 * @param local Logic ID of the Local Node.
 * @param port  Logic ID of the Local Node port used.
 *
 * @returns Upon successful completion, the ID of a newly created virtual
 * portal is returned. Upon failure, a negative error code is returned
 * instead.
 */
PUBLIC int do_vportal_create(int local, int port)
{
	int portalid;  /* Hardware portal ID. */
	int vportalid; /* Virtual portal ID.  */

	/* Checks if the input portal is local. */
	if (!portal_nodenum_is_local(local))
		return (-EINVAL);

	/* Search target hardware portal. */
	if ((portalid = do_portal_search(local, -1, PORTAL_SEARCH_INPUT)) < 0)
		return (-EAGAIN);

	/* Allocate a virtual portal. */
	if ((vportalid = do_vportal_alloc(portalid, port)) < 0)
		return (-EBUSY);

	/* Initialize the new virtual portal. */
	virtual_portals[vportalid].remote  = -1;
	active_portals[portalid].ports[port].status |= PORT_STATUS_USED;
	active_portals[portalid].refcount++;

	dcache_invalidate();
	return (vportalid);
}

/*============================================================================*
 * do_vportal_allow()                                                         *
 *============================================================================*/

/**
 * @brief Enables read operations from a remote.
 *
 * @param portalid    ID of the target virtual portal.
 * @param remote      Logic ID of target node.
 * @param remote_port Target port number in @p remote.
 *
 * @returns Upons successful completion zero is returned. Upon failure,
 * a negative error code is returned instead.
 */
PUBLIC int do_vportal_allow(int portalid, int remote, int remote_port)
{
	int fd;  /* Active portal logic ID. */

	/* Bad virtual portal. */
	if (!VPORTAL_IS_USED(portalid))
		return (-EBADF);

	fd = GET_LADDRESS_FD(portalid);

	/* Bad portal. */
	if (!resource_is_used(&active_portals[fd].resource))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_readable(&active_portals[fd].resource))
		return (-EBADF);

	/* Vportal already allowed a write. */
	if (VPORTAL_IS_ALLOWED(portalid))
		return(-EBUSY);

	virtual_portals[portalid].status |= VPORTAL_STATUS_ALLOWED;
	virtual_portals[portalid].remote  = DO_LADDRESS_COMPOSE(remote, remote_port);

	return (0);
}

/*============================================================================*
 * do_vportal_open()                                                          *
 *============================================================================*/

/**
 * @brief Opens a hardware portal.
 *
 * @param local  Logic ID of the local node.
 * @param remote Logic ID of the target node.
 *
 * @returns Upon successful completion, the ID of the target portal is
 * returned. Upon failure, a negative error code is returned instead.
 */
PRIVATE int _do_portal_open(int local, int remote)
{
	int hwfd;     /* File descriptor. */
	int portalid; /* Portal ID.       */

	/* Search target hardware portal. */
	if ((portalid = do_portal_search(local, remote, PORTAL_SEARCH_OUTPUT)) >= 0)
		return (-EBUSY);

	/* Allocate resource. */
	if ((portalid = resource_alloc(&portalpool)) < 0)
		return (-EAGAIN);

	hwfd = -1;

	/* Checks if the remote is the local node. */
	if (local == remote)
	{
		/* Asserts that @p local is local to the cluster. */
		if (!portal_nodenum_is_local(local))
			return (-EINVAL);
	}
	else
	{
		/* Open underlying output hardware portal. */
		if ((hwfd = portal_open(local, remote)) < 0)
		{
			resource_free(&portalpool, portalid);
			return (hwfd);
		}
	}

	/* Initialize hardware portal. */
	active_portals[portalid].hwfd     = hwfd;
	active_portals[portalid].local    = local;
	active_portals[portalid].remote   = remote;
	active_portals[portalid].refcount = 0;
	resource_set_wronly(&active_portals[portalid].resource);
	resource_set_notbusy(&active_portals[portalid].resource);

	return (portalid);
}

/**
 * @brief Opens a virtual portal.
 *
 * @param local       Logic ID of the local node.
 * @param remote      Logic ID of the target node.
 * @param remote_port Target port number in remote.
 *
 * @returns Upon successful completion, the ID of the newly opened virtual
 * portal is returned. Upon failure, a negative error code is returned instead.
 */
PUBLIC int do_vportal_open(int local, int remote, int remote_port)
{
	int portalid;  /* Hardware portal ID.  */
	int vportalid; /* Virtual portal ID.   */
	int port;      /* Free port available. */

	/* Checks if the portal sender is local. */
	if (!portal_nodenum_is_local(local))
		return (-EINVAL);

	/* Search target hardware portal. */
	if ((portalid = do_portal_search(local, remote, PORTAL_SEARCH_OUTPUT)) < 0)
		return (-EAGAIN);

	/* Allocates a free port in the HW portal. */
	if ((port = do_port_alloc(portalid)) < 0)
		return (-EAGAIN);

	/* Allocate a virtual portal. */
	if ((vportalid = do_vportal_alloc(portalid, port)) < 0)
		return (-EBUSY);

	/* Initialize the new virtual portal. */
	virtual_portals[vportalid].remote = DO_LADDRESS_COMPOSE(remote, remote_port);
	active_portals[portalid].ports[port].status |= PORT_STATUS_USED;
	active_portals[portalid].refcount++;

	dcache_invalidate();
	return (vportalid);
}

/*============================================================================*
 * _do_portal_release()                                                       *
 *============================================================================*/

/**
 * @brief Releases a hardware portal.
 *
 * @param portalid   ID of the target hardware portal.
 * @param release_fn Underlying release function.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PRIVATE int _do_portal_release(int portalid, int (*release_fn)(int))
{
	int ret;  /* HAL function return. */
	int hwfd; /* HWFD allocated on HAL. */

	/* Checks if there is a hwfd allocated to this portal. */
	if ((hwfd = active_portals[portalid].hwfd) >= 0)
	{
		if ((ret = release_fn(active_portals[portalid].hwfd)) < 0)
			return (ret);
	}

	active_portals[portalid].hwfd   = -1;
	active_portals[portalid].local  = -1;
	active_portals[portalid].remote = -1;
	resource_free(&portalpool, portalid);

	dcache_invalidate();
	return (0);
}

/*============================================================================*
 * do_vportal_unlink()                                                        *
 *============================================================================*/

/**
 * @brief Unlinks a created (input) virtual portal.
 *
 * @param portalid Logic ID of the target virtual portal.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int do_vportal_unlink(int portalid)
{
	int fd;              /* Active portal logic ID. */
	int port;            /* Vportal logic port.     */
	int local_hwaddress; /* Local HW address.       */
	int mbuffer;         /* Busy mbuffer.           */

	/* Bad virtual portal. */
	if (!VPORTAL_IS_USED(portalid))
		return (-EBADF);

	fd = GET_LADDRESS_FD(portalid);

	/* Bad portal. */
	if (!resource_is_used(&active_portals[fd].resource))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_readable(&active_portals[fd].resource))
		return (-EBADF);

	port = GET_LADDRESS_PORT(portalid);

	local_hwaddress = DO_LADDRESS_COMPOSE(active_portals[fd].local, port);

	/* Check if exist pending messages for this port. */
	if ((mbuffer = do_message_search(local_hwaddress, -1)) >= 0)
		return (-EBUSY);

	/* Unlink virtual portal. */
	virtual_portals[portalid].status = 0;
	active_portals[fd].ports[port].status &= ~PORT_STATUS_USED;
	active_portals[fd].refcount--;

	return (0);
}

/*============================================================================*
 * do_vportal_close()                                                         *
 *============================================================================*/

/**
 * @brief Closes an opened (output) virtual portal.
 *
 * @param portalid Logic ID of the target virtual portal.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int do_vportal_close(int portalid)
{
	int fd;   /* Active portal logic ID.   */
	int port; /* Port designed to vportal. */

	/* Bad virtual portal. */
	if (!VPORTAL_IS_USED(portalid))
		return (-EBADF);

	fd = GET_LADDRESS_FD(portalid);

	/* Bad portal. */
	if (!resource_is_used(&active_portals[fd].resource))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_writable(&active_portals[fd].resource))
		return (-EBADF);

	port = GET_LADDRESS_PORT(portalid);

	/* Close virtual portal. */
	virtual_portals[portalid].status =  0;
	virtual_portals[portalid].remote = -1;
	active_portals[fd].ports[port].status &= ~PORT_STATUS_USED;
	active_portals[fd].refcount--;

	return (0);
}

/*============================================================================*
 * do_vportal_aread()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 *
 * @todo See what happens when a message comes to a closed port.
 */
PUBLIC int do_vportal_aread(int portalid, void * buffer, size_t size)
{
	int ret;             /* HAL function return.           */
	int fd;              /* Hardware portal logic index.   */
	int port;            /* Port used by vportal.          */
	int local_hwaddress; /* Vportal hardware address.      */
	int dest;            /* Message destination address.   */
	int src;             /* Message sender address.        */
	int mbuffer;         /* New alocated buffer.           */
	uint64_t t1;         /* Clock value before aread call. */
	uint64_t t2;         /* Clock value after aread call.  */
	struct portal_message_buffer *buffer_ptr;

	/* Bad virtual portal. */
	if (!VPORTAL_IS_USED(portalid))
		return (-EBADF);

	fd = GET_LADDRESS_FD(portalid);

	/* Bad portal. */
	if (!resource_is_used(&active_portals[fd].resource))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_readable(&active_portals[fd].resource))
		return (-EBADF);

	/* Unallowed operation. */
	if (!VPORTAL_IS_ALLOWED(portalid))
		return (-EACCES);

	port = GET_LADDRESS_PORT(portalid);

	local_hwaddress = DO_LADDRESS_COMPOSE(active_portals[fd].local, port);

	resource_set_async(&active_portals[fd].resource);

	/* Is there a pending message for this vportal? */
	if ((mbuffer = do_message_search(local_hwaddress, virtual_portals[portalid].remote)) >= 0)
	{
		buffer_ptr = &portal_message_buffers[mbuffer];

		t1 = clock_read();
			kmemcpy(buffer, (void *) &buffer_ptr->message.data, ret = size);
		t2 = clock_read();

		buffer_ptr->flags &= ~MBUFFER_FLAGS_BUSY;

		/* Releases the message buffer. */
		KASSERT(do_mbuffer_free(buffer_ptr) == 0);

		goto finish;
	}

again:
	/* Alloc data buffer to receive data. */
	if ((mbuffer = do_mbuffer_alloc()) < 0)
		return (-EAGAIN);

	active_portals[fd].ports[port].mbuffer = &portal_message_buffers[mbuffer];
	buffer_ptr = active_portals[fd].ports[port].mbuffer;

keep_buffer:
	/* Allows async write from remote. */
	if ((ret = portal_allow(active_portals[fd].hwfd, GET_LADDRESS_FD(virtual_portals[portalid].remote))) < 0)
		goto release_buffer;

	/* Sets the receiver buffer as busy. */
	buffer_ptr->flags |= MBUFFER_FLAGS_BUSY;

	dcache_invalidate();

	t1 = clock_read();

		/* Configures async aread. */
		if ((ret = portal_aread(active_portals[fd].hwfd, (void *) &buffer_ptr->message, (KPORTAL_MESSAGE_HEADER_SIZE + HAL_PORTAL_MAX_SIZE))) < 0)
			goto release_buffer;

		if ((ret = portal_wait(active_portals[fd].hwfd)) < 0)
			goto release_buffer;

	t2 = clock_read();

	/* Checks if the message is addressed for the requesting port. */
	dest = buffer_ptr->message.dest;
	src = buffer_ptr->message.src;

	if ((dest != local_hwaddress) || (src != virtual_portals[portalid].remote))
	{
		/* Check if the destination port is opened to receive data. */
		if (PORT_IS_USED(fd, GET_LADDRESS_PORT(dest)))
		{
			/* Marks the buffer as not used for threads look for this message. */
			buffer_ptr->flags &= ~MBUFFER_FLAGS_USED;
			active_portals[fd].ports[port].mbuffer = NULL;

			goto again;
		}
		else
		{
		/**
		 * XXX - ELSE Discards the message.
		 */
			goto keep_buffer;
		}
	}

	ret = size;

	kmemcpy(buffer, (void *) &buffer_ptr->message.data, size);

finish:
	/* Updates performance statistics. */
	virtual_portals[portalid].latency += (t2 - t1);
	virtual_portals[portalid].volume  += ret;

	/* Revoke allow. */
	virtual_portals[portalid].status &= ~VPORTAL_STATUS_ALLOWED;
	virtual_portals[portalid].remote = -1;

release_buffer:
	/* Sets the portal data buffer as not busy. */
	buffer_ptr->flags &= ~MBUFFER_FLAGS_BUSY;

	KASSERT(do_mbuffer_free(buffer_ptr) == 0);
	active_portals[fd].ports[port].mbuffer = NULL;

	dcache_invalidate();
	return (ret);
}

/*============================================================================*
 * do_vportal_awrite()                                                        *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_vportal_awrite(int portalid, const void * buffer, size_t size)
{
	int ret;           /* HAL function return.            */
	int fd;            /* Hardware portal logic index.    */
	int port;          /* HW port specified to vportal.   */
	int mbuffer;       /* Message buffer used to write.   */
	int local_address; /* HW portal + port address.       */
	uint64_t t1 = 0;   /* Clock value before awrite call. */
	uint64_t t2 = 0;   /* Clock value after awrite call.  */

	/* Bad virtual portal. */
	if (!VPORTAL_IS_USED(portalid))
		return (-EBADF);

	fd = GET_LADDRESS_FD(portalid);

	/* Bad portal. */
	if (!resource_is_used(&active_portals[fd].resource))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_writable(&active_portals[fd].resource))
		return (-EBADF);

	port = GET_LADDRESS_PORT(portalid);

	/* Allocate the message buffer that will be used to write. */
	if (active_portals[fd].ports[port].mbuffer != NULL)
		goto write;

	/* Allocates a message buffer to send the message. */
	if ((mbuffer = do_mbuffer_alloc()) < 0)
		return (mbuffer);

	active_portals[fd].ports[port].mbuffer = &portal_message_buffers[mbuffer];

	/* Calculate the addresses to be included in the message header. */
	local_address = DO_LADDRESS_COMPOSE(active_portals[fd].local, port);

	resource_set_async(&active_portals[fd].resource);

	/* Configure the message header. */
	active_portals[fd].ports[port].mbuffer->message.src  = local_address;
	active_portals[fd].ports[port].mbuffer->message.dest = virtual_portals[portalid].remote;
	active_portals[fd].ports[port].mbuffer->message.size = size;

	t1 = clock_read();
		kmemcpy((void *) &active_portals[fd].ports[port].mbuffer->message.data, buffer, size);
	t2 = clock_read();

	/* Checks if the destination is the local node. */
	if (active_portals[fd].remote == processor_node_get_num(0))
	{
		/* Forwards the message to the mbuffers table. */
		active_portals[fd].ports[port].mbuffer->flags = 0 | MBUFFER_FLAGS_BUSY;
		active_portals[fd].ports[port].mbuffer = NULL;

		goto finish;
	}

write:
	t1 = clock_read();

		/* Configures async aread. */
		if ((ret = portal_awrite(active_portals[fd].hwfd, (void *) &active_portals[fd].ports[port].mbuffer->message, (KPORTAL_MESSAGE_HEADER_SIZE + HAL_PORTAL_MAX_SIZE))) < 0) {
			if (ret == -EAGAIN)
				return (ret);
			else
				goto finish;
		}

		if ((ret = portal_wait(active_portals[fd].hwfd)) < 0)
			return (ret);

	t2 = clock_read();

	/* Releases the buffer allocated. */
	KASSERT(do_mbuffer_free(active_portals[fd].ports[port].mbuffer) == 0);
	active_portals[fd].ports[port].mbuffer = NULL;

finish:
	ret = size;

	/* Update performance statistics. */
	virtual_portals[portalid].latency += (t2 - t1);
	virtual_portals[portalid].volume += ret;

	return (ret);
}

/*============================================================================*
 * do_vportal_wait()                                                          *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_vportal_wait(int portalid)
{
	int ret;       /* HAL function return.          */
	int fd;        /* Hardware portal logic index.  */
	uint64_t t1;   /* Clock value before wait call. */
	uint64_t t2;   /* Clock value after wait call.  */

	/* Bad virtual portal. */
	if (!VPORTAL_IS_USED(portalid))
		return (-EBADF);

	fd = GET_LADDRESS_FD(portalid);

	/* Bad virtual portal. */
	if (!resource_is_async(&active_portals[fd].resource))
		return (-EBADF);

	dcache_invalidate();

	t1 = clock_read();

		/* Wait for asynchronous operation. */
		ret = portal_wait(active_portals[fd].hwfd);

	t2 = clock_read();

	/* Updates performance statistics. */
	virtual_portals[portalid].latency += (t2 - t1);

	dcache_invalidate();
	return (ret);
}

/*============================================================================*
 * do_vportal_ioctl()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
int do_vportal_ioctl(int portalid, unsigned request, va_list args)
{
	int ret = 0;
	int fd;

	/* Bad virtual portal. */
	if (!VPORTAL_IS_USED(portalid))
		return (-EBADF);

	fd = GET_LADDRESS_FD(portalid);

	/* Bad portal. */
	if (!resource_is_used(&active_portals[fd].resource))
		return (-EBADF);

	/* Server request. */
	switch (request)
	{
		/* Get the amount of data transfered so far. */
		case KPORTAL_IOCTL_GET_VOLUME:
		{
			size_t *volume;
			volume = va_arg(args, size_t *);
			*volume = virtual_portals[portalid].volume;
		} break;

		/* Get the cummulative transfer latency. */
		case KPORTAL_IOCTL_GET_LATENCY:
		{
			uint64_t *latency;
			latency = va_arg(args, uint64_t *);
			*latency = virtual_portals[portalid].latency;
		} break;

		/* Operation not supported. */
		default:
			ret = (-ENOTSUP);
			break;
	}

	return (ret);
}

/*============================================================================*
 * kportal_init()                                                             *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC void kportal_init(void)
{
	int local;

	kprintf("[kernel][noc] initializing the kportal facility");

	local = processor_node_get_num(core_get_id());

	/* Create the input portal. */
	KASSERT(_do_portal_create(local) >= 0);

	/* Opens all portal interfaces. */
	for (int i = 0; i < PROCESSOR_NOC_NODES_NUM; ++i)
		KASSERT(_do_portal_open(local, i) >= 0);
}

#endif /* __TARGET_HAS_PORTAL */
