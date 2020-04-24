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

#include "communicator.h"
#include "active.h"
#include "portal.h"

#if __TARGET_HAS_PORTAL

/*============================================================================*
 * Virtual portal structure                                                   *
 *============================================================================*/

/**
 * @brief Table of virtual portals.
 */
PRIVATE struct communicator ALIGN(sizeof(dword_t)) virtual_portals[KPORTAL_MAX] = {
	[0 ... (KPORTAL_MAX - 1)] = COMMUNICATOR_INITIALIZER
};

/**
 * @brief Virtual communicator pool.
 */
PRIVATE const struct communicator_pool vportalpool = {
	virtual_portals, KPORTAL_MAX
};

/*============================================================================*
 * do_vportal_alloc()                                                         *
 *============================================================================*/

/**
 * @brief Searches for a free virtual portal.
 *
 * @param local  Local nodenum.
 * @param remote Remote nodenum.
 * @param port   Target port.
 * @param type   COMM_TYPE_INPUT ? COMM_TYPE_OUTPUT.
 *
 * @returns Upon successful completion, the index of the virtual
 * portal in virtual_portals tab is returned. Upon failure, a
 * negative number is returned instead.
 */
PRIVATE int do_vportal_alloc(int local, int remote, int port, int type)
{
	int fd;       /* Active portal ID.  */
	int portalid; /* Virtual portal ID. */
	struct comm_config config;

	if (type == COMM_TYPE_OUTPUT)
		config.remote = (ACTIVE_LADDRESS_COMPOSE(remote, port, KPORTAL_PORT_NR));
	else
		config.remote = (-1);

	/* Allocates a physical portal port. */
	if ((fd = do_portal_alloc(local, remote, port, type)) < 0)
		return (fd);

	config.fd = fd;

	/* Allocates a communicator. */
	if ((portalid = communicator_alloc(&vportalpool, &config, type)) < 0)
	{
		if (do_portal_release(fd) < 0)
			kpanic("[portal] Failed on release a portal port!");
	}

	return (portalid);
}

/*============================================================================*
 * do_vportal_create()                                                        *
 *============================================================================*/

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
	/* Checks if the input portal is local. */
	if (!node_is_local(local))
		return (-EBADF);

	return (
		do_vportal_alloc(
			local,
			-1,
			port,
			COMM_TYPE_INPUT
		)
	);
}

/*============================================================================*
 * do_vportal_open()                                                          *
 *============================================================================*/

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
	/* Checks if the portal sender is local. */
	if (!node_is_local(local))
		return (-EBADF);

	return (
		do_vportal_alloc(
			local,
			remote,
			remote_port,
			COMM_TYPE_OUTPUT
		)
	);
}

/*============================================================================*
 * do_vportal_unlink()                                                        *
 *============================================================================*/

/**
 * @brief Unlinks a created virtual portal.
 *
 * @param portalid Logic ID of the target virtual portal.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int do_vportal_unlink(int portalid)
{
	return (
		communicator_free(
			&vportalpool,
			portalid,
			COMM_TYPE_INPUT,
			do_portal_release
		)
	);
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
	return (
		communicator_free(
			&vportalpool,
			portalid,
			COMM_TYPE_OUTPUT,
			do_portal_release
		)
	);
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
	int ret; /* Function return. */

	/* Locks the virtual portal to operate over it. */
	spinlock_lock(&virtual_portals[portalid].lock);

		ret = -EBADF;

		/* Bad virtual portal. */
		if (!resource_is_used(&virtual_portals[portalid].resource))
			goto unlock;

		/* Bad virtual portal. */
		if (!resource_is_readable(&virtual_portals[portalid].resource))
			goto unlock;

		ret = -EBUSY;

		/* Vportal already allowed a write. */
		if (communicator_is_allowed(&virtual_portals[portalid]))
			goto unlock;

		/* Allows the virtual portal to read. */
		communicator_set_allowed(&virtual_portals[portalid]);
		virtual_portals[portalid].config.remote = DO_LADDRESS_COMPOSE(remote, remote_port);

		ret = 0;

unlock:
	spinlock_unlock(&virtual_portals[portalid].lock);

	return (ret);
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
	virtual_portals[portalid].config.buffer = buffer;
	virtual_portals[portalid].config.size   = size;

	return (
		communicator_operate(
			&virtual_portals[portalid],
			COMM_TYPE_INPUT,
			do_portal_aread
		)
	);
}

/*============================================================================*
 * do_vportal_awrite()                                                        *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC int do_vportal_awrite(int portalid, const void * buffer, size_t size)
{
	virtual_portals[portalid].config.buffer = buffer;
	virtual_portals[portalid].config.size   = size;

	return (
		communicator_operate(
			&virtual_portals[portalid],
			COMM_TYPE_OUTPUT,
			do_portal_awrite
		)
	);
}

/*============================================================================*
 * do_vportal_wait()                                                          *
 *============================================================================*/

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
	return (
		communicator_wait(
			&virtual_portals[portalid],
			do_portal_wait
		)
	);
}

/*============================================================================*
 * do_vportal_ioctl()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
int do_vportal_ioctl(int portalid, unsigned request, va_list args)
{
	return (
		communicator_ioctl(
			&virtual_portals[portalid],
			request,
			args
		)
	);
}

/*============================================================================*
 * do_virtual_portals_locks_init()                                            *
 *============================================================================*/

/**
 * @brief Initializes the virtual_portals locks.
 */
PRIVATE void do_virtual_portals_locks_init(void)
{
	for (int i = 0; i < KPORTAL_MAX; ++i)
		spinlock_init(&virtual_portals[i].lock);
}

/*============================================================================*
 * kportal_init()                                                             *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC void kportal_init(void)
{
	kprintf("[kernel][noc] initializing the kportal facility");

	do_portal_init();

	/* Initializes the virtual portals locks. */
	do_virtual_portals_locks_init();
}

#endif /* __TARGET_HAS_PORTAL */
