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
#define VPORTAL_FLAGS_USED    (1 << 0) /**< Used vportal? */
#define VPORTAL_FLAGS_ALLOWED (1 << 1) /**< Allowed?      */

/**
 * @brief Asserts if the virtual portal is used.
 */
#define VPORTAL_IS_USED(vportalid) \
	(virtual_portals[vportalid].flags & VPORTAL_FLAGS_USED)

/**
 * @brief Asserts if the virtual portal is allowed.
 */
#define VPORTAL_IS_ALLOWED(vportalid) \
	(virtual_portals[vportalid].flags & VPORTAL_FLAGS_ALLOWED)
/**@}*/

/**
 * @name Helper Macros for logic addresses operations.
 */
/**@{*/

/**
 * @brief Composes the logic address based on portalid and one of it's ports.
 */
#define DO_LADDRESS_COMPOSE(portalid, port) \
	(portalid * PORTAL_PORT_NR + port)

/**
 * @brief Extracts the portalid from vportalid.
 */
#define GET_LADDRESS_FD(vportalid) \
	(vportalid / PORTAL_PORT_NR)

/**
 * @brief Extracts the port number from vportalid.
 */
#define GET_LADDRESS_PORT(vportalid) \
	 (vportalid % PORTAL_PORT_NR)
/**@}*/

/**
 * @name Helper Macros for ports status manipulation.
 */
/**@{*/

/**
 * @brief Portal ports flags.
 */
#define PORT_STATUS_USED        (1 << 0) /**< Used port?   */
#define PORT_STATUS_INITIALIZED (1 << 1) /**< Initialized? */

/**
 * @brief Asserts if the port of portalid is used.
 */
#define PORT_IS_USED(portalid,port) \
	(active_portals[portalid].ports[port].status & PORT_STATUS_USED)

/**
 * @brief Asserts if the port of portalid was already initialized.
 */
#define PORT_IS_INITIALIZED(portalid,port) \
	(active_portals[portalid].ports[port].status & PORT_STATUS_INITIALIZED)
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
	(message_buffers[mbufferid].flags & MBUFFER_FLAGS_USED)

/**
 * @brief Asserts if the message buffer is busy.
 */
#define MBUFFER_IS_BUSY(mbuffer_ptr) \
	(mbuffer_ptr->flags & MBUFFER_FLAGS_BUSY)
/**@}*/

/**
 * @name Helper Macros for portal status manipulation.
 */
/**@{*/

/**
 * @brief Portal status flags.
 */
#define PORTAL_STATUS_INITIALIZED (1 << 0) /**< Initialized? */
#define PORTAL_STATUS_BUSY        (1 << 1) /**< Busy?        */

/**
 * @brief Asserts if the portal lock was already initialized.
 */
#define PORTAL_IS_INITIALIZED(portalid) \
	(active_portals[portalid].status & PORTAL_STATUS_INITIALIZED)

/**
 * @brief Asserts if the portal buffer is busy.
 */
#define PORTAL_IS_BUSY(portalid) \
	(active_portals[portalid].status & PORTAL_STATUS_BUSY)
/**@}*/

/*============================================================================*
 * Control Structures.                                                        *
 *============================================================================*/

/**
 * @brief Struct that represents a portal message buffer.
 */
struct portal_message_buffer
{
	unsigned flags; /* Flags. */

	/**
	 * @brief Structure that holds a message.
	 */
	struct portal_message
	{
		int src;   /* Data sender.       */
		int dest;  /* Data destination.  */
		int size;  /* Message data size. */
		char data[PORTAL_MAX_SIZE];
	} message;
};

struct portal_message_buffer message_buffers[KPORTAL_MESSAGE_BUFFERS_MAX] = {
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
	spinlock_t lock;                       /* Port control lock.     */
	unsigned status;                       /* Port status.           */
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
	unsigned flags; /**< Flags.          */
	int remote;     /**< Remote address. */
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
		.flags = 0
	},
};

/**
 * @brief Table of active portals.
 */
PRIVATE struct portal
{
	struct resource resource; /**< Underlying resource. */

	/* Control parameters. */
	int refcount;                      /**< References count.           */
	int hwfd;                          /**< Underlying file descriptor. */
	int local;                         /**< Local node number.          */
	int remote;                        /**< Target node number.         */
	struct port ports[PORTAL_PORT_NR]; /**< HW ports.                   */
	unsigned status;                   /**< Portal status.              */

	/* Data buffer. */
	struct portal_message_buffer buffer; /**< Buffer resource.  */
	spinlock_t lock;                     /**< Data buffer lock. */
} ALIGN(sizeof(dword_t)) active_portals[(PORTAL_CREATE_MAX + PORTAL_OPEN_MAX)] = {
	[0 ... (PORTAL_CREATE_MAX + PORTAL_OPEN_MAX - 1)] {
		.ports[0 ... (PORTAL_PORT_NR - 1)] = {
			.status = 0,
			.mbuffer = NULL
		},
		.status = 0,
	},
};

/**
 * @brief Resource pool.
 */
PRIVATE const struct resource_pool portalpool = {
	active_portals, (PORTAL_CREATE_MAX + PORTAL_OPEN_MAX), sizeof(struct portal)
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
	if (active_portals[portalid].refcount == PORTAL_PORT_NR)
		return (-1);

	for (unsigned int i = 0; i < PORTAL_PORT_NR; ++i)
	{
		if (!PORT_IS_USED(portalid, i))
			return (i);
	}

	/* Uncaught error. */
	return (-1);
}

/*============================================================================*
 * do_portal_lock_init()                                                      *
 *============================================================================*/

/**
 * @brief Initializes the portal lock.
 *
 * @param portalid ID of the target HW portal.
 */
PRIVATE void do_portal_lock_init(int portalid)
{
	if (!PORTAL_IS_INITIALIZED(portalid))
	{
		spinlock_init(&active_portals[portalid].lock);
		active_portals[portalid].status |= PORTAL_STATUS_INITIALIZED;
	}
}

/*============================================================================*
 * do_port_lock_init()                                                        *
 *============================================================================*/

/**
 * @brief Initializes the specified port lock.
 *
 * @param portalid ID of the target portal.
 * @param port     Port to be initialized.
 */
PRIVATE void do_port_lock_init(int portalid, int port)
{
	if (!PORT_IS_INITIALIZED(portalid, port))
	{
		spinlock_init(&active_portals[portalid].ports[port].lock);
		active_portals[portalid].ports[port].status |= PORT_STATUS_INITIALIZED;
	}
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
		if (!MBUFFER_IS_USED(i))
		{
			message_buffers[i].flags = 0 | MBUFFER_FLAGS_USED;
			message_buffers[i].message.src  = -1;
			message_buffers[i].message.dest = -1;
			message_buffers[i].message.size =  0;
			message_buffers[i].message.data[0] = '\0';
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
	if (MBUFFER_IS_BUSY(buffer))
		return (-EBUSY);

	buffer->flags = 0;

	return (0);
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
	for (unsigned i = 0; i < (PORTAL_CREATE_MAX + PORTAL_OPEN_MAX); ++i)
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
		return (portalid);

	/* Allocate resource. */
	if ((portalid = resource_alloc(&portalpool)) < 0)
		return (-EAGAIN);

	if ((hwfd = portal_create(local)) < 0)
	{
		resource_free(&portalpool, portalid);
		return (hwfd);
	}

	/* Initializes portal lock. */
	do_portal_lock_init(portalid);

	/* Initialize portal. */
	active_portals[portalid].hwfd         = hwfd;
	active_portals[portalid].local        = local;
	active_portals[portalid].remote       = -1;
	active_portals[portalid].refcount     = 0;
	active_portals[portalid].buffer.flags = 0 | MBUFFER_FLAGS_USED;
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
	int portalid;  /* Hardware portal ID.  */
	int vportalid; /* Virtual portal ID.   */
	int mbuffer;   /* Alocated Mbuffer ID. */

	/* Creates a hardware portal. */
	if ((portalid = _do_portal_create(local)) < 0)
		return (portalid);

	/* Allocate a virtual portal. */
	if ((vportalid = do_vportal_alloc(portalid, port)) < 0)
		return (-EBUSY);

	/* Initializes the port lock. */
	do_port_lock_init(portalid, port);

	/* Allocs a message buffer on the port to hold received data. */
	if ((mbuffer = do_mbuffer_alloc()) < 0)
		return (-EAGAIN);

	/* Initialize the new virtual portal. */
	virtual_portals[vportalid].flags   = 0 | VPORTAL_FLAGS_USED;
	virtual_portals[vportalid].remote  = -1;
	virtual_portals[vportalid].volume  = 0ULL;
	virtual_portals[vportalid].latency = 0ULL;
	active_portals[portalid].ports[port].status |= PORT_STATUS_USED;
	active_portals[portalid].ports[port].mbuffer = &message_buffers[mbuffer];
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

	virtual_portals[portalid].flags |= VPORTAL_FLAGS_ALLOWED;
	virtual_portals[portalid].remote = DO_LADDRESS_COMPOSE(remote, remote_port);

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
		return (portalid);

	/* Allocate resource. */
	if ((portalid = resource_alloc(&portalpool)) < 0)
		return (-EAGAIN);

	if ((hwfd = portal_open(local, remote)) < 0)
	{
		resource_free(&portalpool, portalid);
		return (hwfd);
	}

	/* Initializes portal lock. */
	do_portal_lock_init(portalid);

	active_portals[portalid].hwfd         = hwfd;
	active_portals[portalid].local        = local;
	active_portals[portalid].remote       = remote;
	active_portals[portalid].refcount     = 0;
	active_portals[portalid].buffer.flags = 0 | MBUFFER_FLAGS_USED;
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

	/* Opens a hardware portal. */
	if ((portalid = _do_portal_open(local, remote)) < 0)
		return (portalid);

	/* Allocates a free port in the HW portal. */
	if ((port = do_port_alloc(portalid)) < 0)
		return (-EAGAIN);

	/* Allocate a virtual portal. */
	if ((vportalid = do_vportal_alloc(portalid, port)) < 0)
		return (-EBUSY);

	/* Initializes the port lock. */
	do_port_lock_init(portalid, port);

	/* Initialize the new virtual portal. */
	virtual_portals[vportalid].flags   = 0 | VPORTAL_FLAGS_USED;
	virtual_portals[vportalid].remote  = DO_LADDRESS_COMPOSE(remote, remote_port);
	virtual_portals[vportalid].volume  = 0ULL;
	virtual_portals[vportalid].latency = 0ULL;
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
	int ret; /* HAL function return. */

	if ((ret = do_mbuffer_free(&active_portals[portalid].buffer)) < 0)
		return (ret);

	if ((ret = release_fn(active_portals[portalid].hwfd)) < 0)
		return (ret);

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
	int fd;   /* Active portal logic ID.          */
	int port; /* Port designed to vportal.        */
	int ret;  /* Mbuffer release function return. */

	fd = GET_LADDRESS_FD(portalid);

	/* Bad portal. */
	if (!resource_is_used(&active_portals[fd].resource))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_readable(&active_portals[fd].resource))
		return (-EBADF);

	port = GET_LADDRESS_PORT(portalid);

	/* Release mbuffer resource. */
	if (active_portals[fd].ports[port].mbuffer != NULL)
	{
		if ((ret = do_mbuffer_free(active_portals[fd].ports[port].mbuffer)) < 0)
			return (ret);
	}

	/* Unlink virtual portal. */
	virtual_portals[portalid].flags = 0;
	active_portals[fd].ports[port].status &= ~PORT_STATUS_USED;
	active_portals[fd].ports[port].mbuffer = NULL;
	active_portals[fd].refcount--;

	/* Release underlying hardware portal. */
	if (active_portals[fd].refcount == 0)
		return (_do_portal_release(fd, portal_unlink));

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
	int fd;   /* Active portal logic ID.          */
	int port; /* Port designed to vportal.        */
	int ret;  /* Mbuffer release function return. */

	fd = GET_LADDRESS_FD(portalid);

	/* Bad portal. */
	if (!resource_is_used(&active_portals[fd].resource))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_writable(&active_portals[fd].resource))
		return (-EBADF);

	port = GET_LADDRESS_PORT(portalid);

	/* Release mbuffer resource. */
	if (active_portals[fd].ports[port].mbuffer != NULL)
	{
		if ((ret = do_mbuffer_free(active_portals[fd].ports[port].mbuffer)) < 0)
			return (ret);
	}

	/* Close virtual portal. */
	virtual_portals[portalid].flags        =  0;
	active_portals[fd].ports[port].status &= ~PORT_STATUS_USED;
	active_portals[fd].ports[port].mbuffer = NULL;
	active_portals[fd].refcount--;

	/* Release underlying hardware portal. */
	if (active_portals[fd].refcount == 0)
		return (_do_portal_release(fd, portal_close));

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
	int dest;
	uint64_t t1;         /* Clock value before aread call. */
	uint64_t t2;         /* Clock value after aread call.  */

	fd = GET_LADDRESS_FD(portalid);
	port = GET_LADDRESS_PORT(portalid);

	/* Bad portal. */
	if (!resource_is_used(&active_portals[fd].resource))
		return (-EBADF);

	/* Bad portal. */
	if (!resource_is_readable(&active_portals[fd].resource))
		return (-EBADF);

	/* Unallowed operation. */
	if (!VPORTAL_IS_ALLOWED(portalid))
		return (-EACCES);

	local_hwaddress = DO_LADDRESS_COMPOSE(active_portals[fd].local, port);

	resource_set_async(&active_portals[fd].resource);

	/* Check if already exists data on mbuffer. */
	if (MBUFFER_IS_BUSY(active_portals[fd].ports[port].mbuffer))
	{
		t1 = clock_read();
			kmemcpy(buffer, (void *) &active_portals[fd].ports[port].mbuffer->message.data, ret = size);
		t2 = clock_read();

		/* Set mbuffer as not busy. */
		active_portals[fd].ports[port].mbuffer->flags &= ~MBUFFER_FLAGS_BUSY;

		goto finish;
	}

	/* There is a pending read in mbuffer for the requesting thread. */
	if (PORTAL_IS_BUSY(fd))
	{
		dest = active_portals[fd].buffer.message.dest;
		if (dest == local_hwaddress) {
			t1 = clock_read();
				kmemcpy(buffer, (void *) &active_portals[fd].buffer.message.data, ret = size);
			t2 = clock_read();

			goto unlock_portal;
		} else
			return (-EBUSY);
	}

	/* Sets the portal mbuffer as busy. */
	active_portals[fd].status |= PORTAL_STATUS_BUSY;

again:

	dcache_invalidate();

	/* Allows async write from remote. */
	if ((ret = portal_allow(active_portals[fd].hwfd, GET_LADDRESS_FD(virtual_portals[portalid].remote))) < 0)
		return (ret);

	t1 = clock_read();

		/* Configures async aread. */
		if ((ret = portal_aread(active_portals[fd].hwfd, (void *) &active_portals[fd].buffer.message, (KPORTAL_MESSAGE_HEADER_SIZE + PORTAL_MAX_SIZE))) < 0)
			return (ret);

		if ((ret = portal_wait(active_portals[fd].hwfd)) < 0)
			return (ret);

	t2 = clock_read();

	/* Checks if the message is addressed for the requesting port. */
	dest = active_portals[fd].buffer.message.dest;
	if (!(dest == local_hwaddress))
	{
		int aux_port = GET_LADDRESS_PORT(dest);

		/* Check if the destination port is opened to receive data. */
		if (PORT_IS_USED(fd, aux_port))
		{
			/* Redirects the message to the correct port. */
			kmemcpy((void *) &active_portals[fd].ports[aux_port].mbuffer->message, (void *) &active_portals[fd].buffer.message, (KPORTAL_MESSAGE_HEADER_SIZE + active_portals[fd].buffer.message.size));
			active_portals[fd].ports[aux_port].mbuffer->flags |= MBUFFER_FLAGS_BUSY;
			goto again;
		}
		else
		{
			/* Returns sinalizing the portal is busy. */
			return (-EBUSY);
		}
	}

	ret = size;

	kmemcpy(buffer, (void *) &active_portals[fd].buffer.message.data, size);

unlock_portal:
	/* Sets the portal mbuffer as not busy. */
	active_portals[fd].status &= ~PORTAL_STATUS_BUSY;

finish:
	/* Updates performance statistics. */
	virtual_portals[portalid].latency += (t2 - t1);
	virtual_portals[portalid].volume  += ret;

	/* Revoke allow. */
	virtual_portals[portalid].flags &= ~VPORTAL_FLAGS_ALLOWED;
	virtual_portals[portalid].remote = -1;

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
	uint64_t t1;       /* Clock value before awrite call. */
	uint64_t t2;       /* Clock value after awrite call.  */

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

	if ((mbuffer = do_mbuffer_alloc()) < 0)
		return (mbuffer);

	active_portals[fd].ports[port].mbuffer = &message_buffers[mbuffer];

	/* Calculate the addresses to be included in the message header. */
	local_address  = DO_LADDRESS_COMPOSE(active_portals[fd].local, port);

	resource_set_async(&active_portals[fd].resource);

	active_portals[fd].ports[port].mbuffer->message.src = local_address;
	active_portals[fd].ports[port].mbuffer->message.dest = virtual_portals[portalid].remote;
	active_portals[fd].ports[port].mbuffer->message.size = size;
	kmemcpy((void *) &active_portals[fd].ports[port].mbuffer->message.data, buffer, size);

write:
	t1 = clock_read();

		/* Configures async aread. */
		if ((ret = portal_awrite(active_portals[fd].hwfd, (void *) &active_portals[fd].ports[port].mbuffer->message, (KPORTAL_MESSAGE_HEADER_SIZE + PORTAL_MAX_SIZE))) < 0) {
			if (ret == -EAGAIN)
				return (ret);
			else
				goto finish;
		}

		if ((ret = portal_wait(active_portals[fd].hwfd)) < 0)
			return (ret);

	t2 = clock_read();

	ret = size;

	/* Update performance statistics. */
	virtual_portals[portalid].latency += (t2 - t1);
	virtual_portals[portalid].volume += ret;

finish:
	/* Releases the buffer allocated. */
	do_mbuffer_free(active_portals[fd].ports[port].mbuffer);
	active_portals[fd].ports[port].mbuffer = NULL;

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
	int port;      /* Port designed to vportal.     */
	uint64_t t1;   /* Clock value before wait call. */
	uint64_t t2;   /* Clock value after wait call.  */

	UNUSED(port);

	fd = GET_LADDRESS_FD(portalid);

	/* Bad virtual portal. */
	if (!resource_is_async(&active_portals[fd].resource))
		return (-EBADF);

	port = GET_LADDRESS_PORT(portalid);

	dcache_invalidate();

	t1 = clock_read();

		/* Wait for asynchronous operation. */
		ret = portal_wait(active_portals[fd].hwfd);

	t2 = clock_read();

	/* Updates performance statistics. */
	virtual_portals[portalid].latency += (t2 - t1);

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

	fd = GET_LADDRESS_FD(portalid);

	/* Bad portal. */
	if (!resource_is_used(&active_portals[fd].resource))
		return (-EBADF);

	/* Server request. */
	switch (request)
	{
		/* Get the amount of data transfered so far. */
		case PORTAL_IOCTL_GET_VOLUME:
		{
			size_t *volume;
			volume = va_arg(args, size_t *);
			*volume = virtual_portals[portalid].volume;
		} break;

		/* Get the cummulative transfer latency. */
		case PORTAL_IOCTL_GET_LATENCY:
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

#endif /* __TARGET_HAS_PORTAL */
