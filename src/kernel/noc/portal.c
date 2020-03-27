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
#define VPORTAL_STATUS_USED     (1 << 0) /**< Used vportal?       */
#define VPORTAL_STATUS_BUSY     (1 << 1) /**< Busy vportal?       */
#define VPORTAL_STATUS_ALLOWED  (1 << 2) /**< Allowed?            */
#define VPORTAL_STATUS_FINISHED (1 << 3) /**< Finished operation? */

/**
 * @brief Asserts if the virtual portal is used.
 */
#define VPORTAL_IS_USED(vportalid) \
	(virtual_portals[vportalid].status & VPORTAL_STATUS_USED)

/**
 * @brief Asserts if the virtual portal is busy.
 */
#define VPORTAL_IS_BUSY(vportalid) \
	(virtual_portals[vportalid].status & VPORTAL_STATUS_BUSY)

/**
 * @brief Asserts if the virtual portal is allowed.
 */
#define VPORTAL_IS_ALLOWED(vportalid) \
	(virtual_portals[vportalid].status & VPORTAL_STATUS_ALLOWED)

/**
 * @brief Asserts if the virtual portal already finished last operation.
 */
#define VPORTAL_IS_FINISHED(vportalid) \
	(virtual_portals[vportalid].status & VPORTAL_STATUS_FINISHED)

/**
 * @brief VPortal Busy status operation macros.
 */
#define VPORTAL_SET_BUSY(vportalid) \
	(virtual_portals[vportalid].status |= VPORTAL_STATUS_BUSY)

#define VPORTAL_SET_NOTBUSY(vportalid) \
	(virtual_portals[vportalid].status &= ~VPORTAL_STATUS_BUSY)
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
#define PORT_STATUS_USED (1 << 0) /**< Used port? */

/**
 * @brief Asserts if the port of portalid is used.
 */
#define PORT_IS_USED(portalid,port) \
	(active_portals[portalid].ports[port].status & PORT_STATUS_USED)
/**@}*/

/*============================================================================*
 * Control Structures.                                                        *
 *============================================================================*/

/**
 * @brief Portal message buffer.
 */
PRIVATE struct mbuffer
{
	/*
	 * XXX: Don't Touch! This Must Come First!
	 */
	struct resource resource; /**< Generic resource information. */

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
} mbuffers[KPORTAL_MESSAGE_BUFFERS_MAX] = {
	[0 ... (KPORTAL_MESSAGE_BUFFERS_MAX - 1)] = {
		.message = {
			.src  = -1,
			.dest = -1,
			.size =  0,
			.data = {'\0'},
		},
	},
};

PRIVATE spinlock_t mbuffers_lock;

/**
 * @brief Mbuffer resource pool.
 */
PRIVATE const struct resource_pool mbufferpool = {
	mbuffers, KPORTAL_MESSAGE_BUFFERS_MAX, sizeof(struct mbuffer)
};

/**
 * @brief Struct that represents a port abstraction.
 */
struct port
{
	unsigned short status; /* Port status.      */
	short mbufferid;       /* Kernel mbufferid. */
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
	unsigned short status; /**< Status.            */
	int remote;            /**< Remote address.    */
	void *user_buffer;     /**< User level buffer. */
	spinlock_t lock;       /**< Protection.        */
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
		.remote = -1,
		.user_buffer = NULL
	},
};

/**
 * @brief Table of active portals.
 */
PRIVATE struct portal
{
	/*
	 * XXX: Don't Touch! This Must Come First!
	 */
	struct resource resource;           /**< Generic resource information. */

	int refcount;                       /**< References count.             */
	int hwfd;                           /**< Underlying file descriptor.   */
	int local;                          /**< Local node number.            */
	int remote;                         /**< Target node number.           */
	spinlock_t lock;                    /**< Protection.                   */
	struct port ports[KPORTAL_PORT_NR]; /**< HW ports.                     */
	int allowed;
} ALIGN(sizeof(dword_t)) active_portals[HW_PORTAL_MAX] = {
	[0 ... (HW_PORTAL_MAX - 1)] {
		.ports[0 ... (KPORTAL_PORT_NR - 1)] = {
			.status    =  0,
			.mbufferid = -1
		},
		.local  = -1,
		.remote = -1,
		.allowed = 0
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
 * do_active_portals_locks_init()                                             *
 *============================================================================*/

/**
 * @brief Initializes the mbuffers table lock.
 */
PRIVATE void do_active_portals_locks_init(void)
{
	for (int i = 0; i < HW_PORTAL_MAX; ++i)
		spinlock_init(&active_portals[i].lock);
}

/*============================================================================*
 * do_virtual_portals_locks_init()                                            *
 *============================================================================*/

/**
 * @brief Initializes the mbuffers table lock.
 */
PRIVATE void do_virtual_portals_locks_init(void)
{
	for (int i = 0; i < KPORTAL_MAX; ++i)
		spinlock_init(&virtual_portals[i].lock);
}

/*============================================================================*
 * do_mbuffers_lock_init()                                                    *
 *============================================================================*/

/**
 * @brief Initializes the mbuffers table lock.
 */
PRIVATE void do_mbuffers_lock_init(void)
{
	spinlock_init(&mbuffers_lock);
}

/*============================================================================*
 * do_vportal_release_mbuffer()                                               *
 *============================================================================*/

/**
 * @brief Mbuffer release keep/discard message constants.
 */
#define DISCARD_MESSAGE 0
#define KEEP_MESSAGE    1

/**
 * @brief Releases the message buffer allocated to @p portalid.
 *
 * @param mbufferid mbuffer ID to release.
 * @param keep_msg  Keep / Discard the mbuffer message?
 *
 * @return Upon successful completion, zero is returned. Upon failure,
 * a negative number is returned instead.
 */
PRIVATE int do_vportal_release_mbuffer(int mbufferid, int keep_msg)
{
	/* Invalid portalid. */
	if (!WITHIN(mbufferid, 0, KPORTAL_MESSAGE_BUFFERS_MAX))
		return (-EINVAL);

	/* Locks the mbuffers table. */
	spinlock_lock(&mbuffers_lock);

		if (keep_msg)
		{
			/* Sets the mbuffer as not used keeping its message. */
			resource_set_busy(&mbuffers[mbufferid].resource);
		}
		else
		{
			/* Frees the mbuffer resource. */
			mbuffers[mbufferid].message.src  = -1;
			mbuffers[mbufferid].message.dest = -1;
			mbuffers[mbufferid].message.size =  0;
			mbuffers[mbufferid].message.data[0] = '\0';
			resource_free(&mbufferpool, mbufferid);
		}

	/* Unlocks the mbuffers table. */
	spinlock_unlock(&mbuffers_lock);

	return (0);
}

/**
 * @brief Allocates a message buffer.
 *
 * @return Upon successful completion, mbufferid returned. Upon failure,
 * a negative number is returned instead.
 */
PRIVATE int do_vportal_alloc_mbuffer(void)
{
	int mbufferid;

	spinlock_lock(&mbuffers_lock);

		/* Allocates a data buffer to receive data. */
		mbufferid = resource_alloc(&mbufferpool);

	spinlock_unlock(&mbuffers_lock);

	return (mbufferid);
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
	int ret;

	ret = -1;

	/* Locks the mbuffers table. */
	spinlock_lock(&mbuffers_lock);

	for (unsigned int i = 0; i < KPORTAL_MESSAGE_BUFFERS_MAX; ++i)
	{
		/* Is the buffer being used? */
		if (!resource_is_used(&mbuffers[i].resource))
			continue;

		/* The buffer contains a stored message? */
		if (!resource_is_busy(&mbuffers[i].resource))
			continue;

		/* Is this message addressed to the local_address? */
		if (mbuffers[i].message.dest != local_address)
			continue;

		/* Is the message sender the expected? */
		if (remote_address != -1)
		{
			if (mbuffers[i].message.src != remote_address)
				continue;
		}

		ret = i;
		break;
	}

	/* Unlocks the mbuffers table. */
	spinlock_unlock(&mbuffers_lock);

	return (ret);
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
	active_portals[portalid].allowed  = 0;
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
	int fd;       /* Hardware portal ID. */
	int portalid; /* Virtual portal ID.  */

	/* Checks if the input portal is local. */
	if (!node_is_local(local))
		return (-EINVAL);

	/* Search target hardware portal. */
	if ((fd = do_portal_search(local, -1, PORTAL_SEARCH_INPUT)) < 0)
		return (-EAGAIN);

	/* Allocate a virtual portal. */
	if ((portalid = do_vportal_alloc(fd, port)) < 0)
		return (-EBUSY);

	/* Initialize the new virtual portal. */
	virtual_portals[portalid].remote  = -1;
	active_portals[fd].ports[port].status |= PORT_STATUS_USED;
	active_portals[fd].refcount++;

	dcache_invalidate();
	return (portalid);
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
		if (!node_is_local(local))
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
	int fd;       /* Hardware portal ID.  */
	int portalid; /* Virtual portal ID.   */
	int port;     /* Free port available. */

	/* Checks if the portal sender is local. */
	if (!node_is_local(local))
		return (-EINVAL);

	/* Search target hardware portal. */
	if ((fd = do_portal_search(local, remote, PORTAL_SEARCH_OUTPUT)) < 0)
		return (-EAGAIN);

	/* Allocates a free port in the HW portal. */
	if ((port = do_port_alloc(fd)) < 0)
		return (-EAGAIN);

	/* Allocate a virtual portal. */
	if ((portalid = do_vportal_alloc(fd, port)) < 0)
		return (-EBUSY);

	/* Initialize the new virtual portal. */
	virtual_portals[portalid].remote = DO_LADDRESS_COMPOSE(remote, remote_port);
	active_portals[fd].ports[port].status |= PORT_STATUS_USED;
	active_portals[fd].refcount++;

	dcache_invalidate();
	return (portalid);
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

	spinlock_lock(&virtual_portals[portalid].lock);

		/* Bad virtual portal. */
		if (!VPORTAL_IS_USED(portalid))
		{
			spinlock_unlock(&virtual_portals[portalid].lock);
			return (-EBADF);
		}

		/* Busy virtual portal. */
		if (VPORTAL_IS_BUSY(portalid))
		{
			spinlock_unlock(&virtual_portals[portalid].lock);
			return (-EBUSY);
		}

	spinlock_unlock(&virtual_portals[portalid].lock);

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

	spinlock_lock(&virtual_portals[portalid].lock);

		/* Bad virtual portal. */
		if (!VPORTAL_IS_USED(portalid))
		{
			spinlock_unlock(&virtual_portals[portalid].lock);
			return (-EBADF);
		}

		/* Busy virtual portal. */
		if (VPORTAL_IS_BUSY(portalid))
		{
			spinlock_unlock(&virtual_portals[portalid].lock);
			return (-EBUSY);
		}

	spinlock_unlock(&virtual_portals[portalid].lock);

	fd = GET_LADDRESS_FD(portalid);

	/* Bad portal. */
	if (!resource_is_used(&active_portals[fd].resource))
	{
		spinlock_unlock(&active_portals[fd].lock);
		return (-EBADF);
	}

	/* Bad portal. */
	if (!resource_is_writable(&active_portals[fd].resource))
	{
		spinlock_unlock(&active_portals[fd].lock);
		return (-EBADF);
	}

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
	int mbufferid;       /* New alocated buffer.           */
	uint64_t t1;         /* Clock value before aread call. */
	uint64_t t2;         /* Clock value after aread call.  */

	fd = GET_LADDRESS_FD(portalid);

	spinlock_lock(&virtual_portals[portalid].lock);

		/* Bad virtual portal. */
		if (!VPORTAL_IS_USED(portalid))
		{
			spinlock_unlock(&virtual_portals[portalid].lock);
			return (-EBADF);
		}

		/* Busy virtual portal. */
		if (VPORTAL_IS_BUSY(portalid))
		{
			spinlock_unlock(&virtual_portals[portalid].lock);
			return (-EBUSY);
		}

		/* Sets the virtual portal as busy. */
		VPORTAL_SET_BUSY(portalid);

	spinlock_unlock(&virtual_portals[portalid].lock);

	port = GET_LADDRESS_PORT(portalid);
	local_hwaddress = DO_LADDRESS_COMPOSE(active_portals[fd].local, port);

	ret = (-EBADF);

	/* Bad portal. */
	if (!resource_is_used(&active_portals[fd].resource))
		goto release_virtual;

	/* Bad portal. */
	if (!resource_is_readable(&active_portals[fd].resource))
		goto release_virtual;

	/* Is there a pending message for this vportal? */
	if ((mbufferid = do_message_search(local_hwaddress, virtual_portals[portalid].remote)) >= 0)
	{
		t1 = clock_read();
			kmemcpy(buffer, (void *) &mbuffers[mbufferid].message.data, ret = size);
		t2 = clock_read();

		/* Update performance statistics. */
		virtual_portals[portalid].latency += (t2 - t1);
		virtual_portals[portalid].volume += ret;

		/* Revoke allow. */
		virtual_portals[portalid].status &= ~VPORTAL_STATUS_ALLOWED;
		virtual_portals[portalid].remote  = -1;

		/* Marks that the virtual portal already finished its read. */
		virtual_portals[portalid].status |= VPORTAL_STATUS_FINISHED;

		KASSERT(do_vportal_release_mbuffer(mbufferid, DISCARD_MESSAGE) == 0);

		return (size);
	}

	if (active_portals[fd].local == active_portals[fd].remote)
	{
		ret = -ENOMSG;
		goto release_virtual;
	}

	spinlock_lock(&active_portals[fd].lock);

		ret = (-EACCES);

		/* Unallowed operation. */
		if (!VPORTAL_IS_ALLOWED(portalid))
		{
			spinlock_unlock(&active_portals[fd].lock);
			goto release_virtual;
		}

		ret = (-EBUSY);

		/* Bad portal. */
		if (resource_is_busy(&active_portals[fd].resource))
		{
			spinlock_unlock(&active_portals[fd].lock);
			goto release_virtual;
		}

		/* Sets the portal as busy. */
		resource_set_busy(&active_portals[fd].resource);

	spinlock_unlock(&active_portals[fd].lock);

	/* Allocates a data buffer to receive data. */
	if ((mbufferid = do_vportal_alloc_mbuffer()) < 0)
	{
		ret = mbufferid;
		goto release_active;
	}

	active_portals[fd].ports[port].mbufferid = mbufferid;

	if (active_portals[fd].allowed)
		goto read;

	/* Allows async write from remote. */
	if ((ret = portal_allow(active_portals[fd].hwfd, GET_LADDRESS_FD(virtual_portals[portalid].remote))) < 0)
		goto discard_message;

	active_portals[fd].allowed = 1;

read:
	t1 = clock_read();

		/* Configures async aread. */
		if ((ret = portal_aread(active_portals[fd].hwfd, (void *) &mbuffers[mbufferid].message, (KPORTAL_MESSAGE_HEADER_SIZE + HAL_PORTAL_MAX_SIZE))) < 0)
			goto discard_message;

	t2 = clock_read();

	virtual_portals[portalid].user_buffer = buffer;

	active_portals[fd].allowed = 0;

	ret = size;

	/* Update performance statistics. */
	virtual_portals[portalid].latency += (t2 - t1);

	return (ret);

discard_message:
	KASSERT(do_vportal_release_mbuffer(mbufferid, DISCARD_MESSAGE) == 0);
	active_portals[fd].ports[port].mbufferid = -1;

release_active:
	spinlock_lock(&active_portals[fd].lock);
		resource_set_notbusy(&active_portals[fd].resource);
	spinlock_unlock(&active_portals[fd].lock);

release_virtual:
	spinlock_lock(&virtual_portals[portalid].lock);
		VPORTAL_SET_NOTBUSY(portalid);
	spinlock_unlock(&virtual_portals[portalid].lock);

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
	int mbufferid;     /* Message buffer used to write.   */
	int local_address; /* HW portal + port address.       */
	uint64_t t1;       /* Clock value before awrite call. */
	uint64_t t2;       /* Clock value after awrite call.  */

	spinlock_lock(&virtual_portals[portalid].lock);

		/* Bad virtual portal. */
		if (!VPORTAL_IS_USED(portalid))
		{
			spinlock_unlock(&virtual_portals[portalid].lock);
			return (-EBADF);
		}

		/* Busy virtual portal. */
		if (VPORTAL_IS_BUSY(portalid))
		{
			spinlock_unlock(&virtual_portals[portalid].lock);
			return (-EBUSY);
		}

		VPORTAL_SET_BUSY(portalid);

	spinlock_unlock(&virtual_portals[portalid].lock);

	fd = GET_LADDRESS_FD(portalid);
	port = GET_LADDRESS_PORT(portalid);

	ret = (-EBADF);

	/* Bad portal. */
	if (!resource_is_used(&active_portals[fd].resource))
		goto release_virtual;

	/* Bad portal. */
	if (!resource_is_writable(&active_portals[fd].resource))
		goto release_virtual;

	/* Checks if there is already a mbuffer allocated. */
	if ((mbufferid = active_portals[fd].ports[port].mbufferid) < 0)
	{
		/* Allocates a message buffer to send the message. */
		if ((mbufferid = do_vportal_alloc_mbuffer()) < 0)
		{
			ret = mbufferid;
			goto release_virtual;
		}

		/* Calculate the addresses to be included in the message header. */
		local_address = DO_LADDRESS_COMPOSE(active_portals[fd].local, port);

		/* Configure the message header. */
		mbuffers[mbufferid].message.src  = local_address;
		mbuffers[mbufferid].message.dest = virtual_portals[portalid].remote;
		mbuffers[mbufferid].message.size = size;

		t1 = clock_read();
			kmemcpy((void *) &mbuffers[mbufferid].message.data, buffer, size);
		t2 = clock_read();

		active_portals[fd].ports[port].mbufferid = mbufferid;

		/* Checks if the destination is the local node. */
		if (active_portals[fd].remote == active_portals[fd].local)
		{
			/* Forwards the message to the mbuffers table. */
			do_vportal_release_mbuffer(mbufferid, KEEP_MESSAGE);
			active_portals[fd].ports[port].mbufferid = -1;

			/* Update performance statistics. */
			virtual_portals[portalid].latency += (t2 - t1);
			virtual_portals[portalid].volume  += size;

			/* Marks that the virtual portal already finished its read. */
			virtual_portals[portalid].status |= VPORTAL_STATUS_FINISHED;

			return (size);
		}
	}

	spinlock_lock(&active_portals[fd].lock);

		ret = (-EBUSY);

		/* Bad portal. */
		if (resource_is_busy(&active_portals[fd].resource))
		{
			spinlock_unlock(&active_portals[fd].lock);
			goto release_virtual;
		}

		/* Sets the portal as busy. */
		resource_set_busy(&active_portals[fd].resource);

	spinlock_unlock(&active_portals[fd].lock);

	t1 = clock_read();

		/* Configures asynchronous write. */
		if ((ret = portal_awrite(active_portals[fd].hwfd, (void *) &mbuffers[mbufferid].message, (KPORTAL_MESSAGE_HEADER_SIZE + HAL_PORTAL_MAX_SIZE))) < 0)
		{
			do_vportal_release_mbuffer(mbufferid, DISCARD_MESSAGE);
			active_portals[fd].ports[port].mbufferid = -1;

			spinlock_lock(&active_portals[fd].lock);
				resource_set_notbusy(&active_portals[fd].resource);
			spinlock_unlock(&active_portals[fd].lock);

			goto release_virtual;
		}

	t2 = clock_read();

	/* Update performance statistics. */
	virtual_portals[portalid].latency += (t2 - t1);
	virtual_portals[portalid].volume  += size;

	return (size);

release_virtual:
	spinlock_lock(&virtual_portals[portalid].lock);
		VPORTAL_SET_NOTBUSY(portalid);
	spinlock_unlock(&virtual_portals[portalid].lock);

	return (ret);
}

/*============================================================================*
 * do_vportal_wait()                                                          *
 *============================================================================*/

/**
 * @brief Wait implementation for input portals. Waits on a virtual
 * portal to finish an asynchronous read.
 *
 * @param portalid Logic ID of the target virtual portal.
 *
 * @returns Upon successful completion, zero is returned if the operation
 * finished on @p portalid. If the operation incurred in a re-addressing, ONE
 * is returned. Upon failure, a negative error code is returned instead.
 */
PRIVATE int do_vportal_receiver_wait(int portalid)
{
	int ret;             /* HAL function return.          */
	int fd;              /* Hardware portal logic index.  */
	int port;            /* Port used by vportal.         */
	int src;             /* Msg source address.           */
	int dest;            /* Msg destination address.      */
	int size;            /* Underlying message size.      */
	int keep_rule;       /* Discard rule.                 */
	int local_hwaddress; /* Vportal hardware address.     */
	int mbufferid;       /* Allocated mbufferid.          */
	uint64_t t1;         /* Clock value before wait call. */
	uint64_t t2;         /* Clock value after wait call.  */

	keep_rule = DISCARD_MESSAGE;

	fd = GET_LADDRESS_FD(portalid);
	port = GET_LADDRESS_PORT(portalid);

	mbufferid = active_portals[fd].ports[port].mbufferid;

	t1 = clock_read();

		/* Wait for asynchronous read to finish. */
		if ((ret = portal_wait(active_portals[fd].hwfd)) < 0)
			goto release_buffer;

	t2 = clock_read();

	local_hwaddress = DO_LADDRESS_COMPOSE(active_portals[fd].local, port);

	/* Checks if the message is addressed for the requesting port. */
	dest = mbuffers[mbufferid].message.dest;
	src  = mbuffers[mbufferid].message.src;

	if ((dest != local_hwaddress) || (src != virtual_portals[portalid].remote))
	{
		keep_rule = PORT_IS_USED(fd, GET_LADDRESS_PORT(dest));

		/* Returns sinalizing that a message was read, but not for local port. */
		ret = 1;
	}
	else
	{
		size = mbuffers[mbufferid].message.size;

		kmemcpy(virtual_portals[portalid].user_buffer, (void *) &mbuffers[mbufferid].message.data, size);

		/* Update performance statistics. */
		virtual_portals[portalid].latency += (t2 - t1);
		virtual_portals[portalid].volume  += size;

		virtual_portals[portalid].user_buffer = NULL;

		/* Revoke allow. */
		virtual_portals[portalid].status &= ~VPORTAL_STATUS_ALLOWED;
		virtual_portals[portalid].remote  = -1;

		ret = 0;
	}

release_buffer:
	do_vportal_release_mbuffer(mbufferid, keep_rule);
	active_portals[fd].ports[port].mbufferid = -1;

	return (ret);
}

/**
 * @brief Wait implementation for output portals. Waits on a virtual
 * portal to finish an asynchronous write.
 *
 * @param portalid Logic ID of the sender virtual portal.
 *
 * @returns Upon successful completion, zero is returned. Upon failure,
 * a negative error code is returned instead.
 */
PRIVATE int do_vportal_sender_wait(int portalid)
{
	int ret;       /* HAL function return.          */
	int fd;        /* Vportal file descriptor.      */
	int port;      /* Port used by vportal.         */
	int mbufferid; /* Allocated mbufferid.          */
	uint64_t t1;   /* Clock value before wait call. */
	uint64_t t2;   /* Clock value after wait call.  */

	fd = GET_LADDRESS_FD(portalid);
	port = GET_LADDRESS_PORT(portalid);

	mbufferid = active_portals[fd].ports[port].mbufferid;

	t1 = clock_read();

		/* Wait for asynchronous write to finish. */
		if ((ret = portal_wait(active_portals[fd].hwfd)) < 0)
			goto release_buffer;

	t2 = clock_read();

	/* Update performance statistics. */
	virtual_portals[portalid].latency += (t2 - t1);

release_buffer:
	do_vportal_release_mbuffer(mbufferid, DISCARD_MESSAGE);
	active_portals[fd].ports[port].mbufferid = -1;

	return (ret);
}

/**
 * @brief Waits on a virtual portal to finish an assynchronous operation.
 *
 * @param portalid Logic ID of the target virtual portal.
 *
 * @returns Upon successful completion, a positive number is returned.
 * Upon failure, a negative error code is returned instead.
 */
PUBLIC int do_vportal_wait(int portalid)
{
	int ret;             /* Underlying function return. */
	int fd;              /* HW portal logic index.      */
	int port;            /* Port used by vportal.       */
	int (*wait_fn)(int); /* Underlying wait function.   */

	spinlock_lock(&virtual_portals[portalid].lock);

		/* Bad virtual portal. */
		if (!VPORTAL_IS_USED(portalid))
		{
			spinlock_unlock(&virtual_portals[portalid].lock);
			return (-EBADF);
		}

		/* Unconfigured operation over this virtual portal. */
		if (!VPORTAL_IS_BUSY(portalid))
		{
			spinlock_unlock(&virtual_portals[portalid].lock);
			return (-EBADF);
		}
	
	spinlock_unlock(&virtual_portals[portalid].lock);

	/* Virtual portal already finished its last operation. */
	if (VPORTAL_IS_FINISHED(portalid))
	{
		virtual_portals[portalid].status &= ~VPORTAL_STATUS_FINISHED;
		ret = 0;
		goto release_virtual;
	}

	fd = GET_LADDRESS_FD(portalid);
	port = GET_LADDRESS_PORT(portalid);

	spinlock_lock(&active_portals[fd].lock);

		/* Bad portal. */
		if (!resource_is_busy(&active_portals[fd].resource))
		{
			spinlock_unlock(&active_portals[fd].lock);
			return (-EBADF);
		}

	spinlock_unlock(&active_portals[fd].lock);

	/* Invalid mbufferid. */
	if (active_portals[fd].ports[port].mbufferid < 0)
		return (-EBADF);

	/* Checks the underlying portal type. */
	if (resource_is_readable(&active_portals[fd].resource))
		wait_fn = do_vportal_receiver_wait;
	else if (resource_is_writable(&active_portals[fd].resource))
		wait_fn = do_vportal_sender_wait;
	else
		return (-EBADF);

	/* Calls the underlying wait function according to the type of the portal. */
	ret = wait_fn(portalid);

	spinlock_lock(&active_portals[fd].lock);
		resource_set_notbusy(&active_portals[fd].resource);
	spinlock_unlock(&active_portals[fd].lock);

release_virtual:
	spinlock_lock(&virtual_portals[portalid].lock);
		VPORTAL_SET_NOTBUSY(portalid);
	spinlock_unlock(&virtual_portals[portalid].lock);

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

	local = processor_node_get_num();

	/* Create the input portal. */
	KASSERT(_do_portal_create(local) >= 0);

	/* Opens all portal interfaces. */
	for (int i = 0; i < PROCESSOR_NOC_NODES_NUM; ++i)
		KASSERT(_do_portal_open(local, i) >= 0);

	/* Initializes the mbuffers table lock. */
	do_mbuffers_lock_init();

	/* Initializes the active portals locks. */
	do_active_portals_locks_init();

	/* Initializes the virtual portals locks. */
	do_virtual_portals_locks_init();
}

#endif /* __TARGET_HAS_PORTAL */
