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
#include "portal.h"

#if __TARGET_HAS_PORTAL && !__NANVIX_IKC_USES_ONLY_MAILBOX

/**
 * @brief Extracts fd and port from portalid.
 */
/**@{*/
#define VPORTAL_GET_LADDRESS_PORT(portalid) (portalid % KPORTAL_PORT_NR)
/**@}*/

/*============================================================================*
 * Virtual portal structure                                                   *
 *============================================================================*/

/**
 * @name Virtual portal resources.
 */
/**@{*/
PRIVATE struct communicator_counters vportal_counters;                    /**< Virtual mailbox counters. */
PRIVATE struct communicator ALIGN(sizeof(dword_t)) vportals[KPORTAL_MAX]; /**< Table of virtual portals. */
PRIVATE struct communicator_functions vportal_functions;                  /**< Portal functions.         */
PRIVATE struct communicator_pool vportalpool;                             /**< Virtual portal pool.      */
/**@}*/

/*============================================================================*
 * do_vportal_init()                                                          *
 *============================================================================*/

/**
 * @brief Initializes the mbuffers table lock.
 */
PRIVATE void do_vportal_init(void)
{
	spinlock_init(&vportal_counters.lock);
	vportal_counters.ncreates = 0ULL;
	vportal_counters.nunlinks = 0ULL;
	vportal_counters.nopens   = 0ULL;
	vportal_counters.ncloses  = 0ULL;
	vportal_counters.nreads   = 0ULL;
	vportal_counters.nwrites  = 0ULL;

	vportal_functions.do_release = do_portal_release;
	vportal_functions.do_comm    = do_portal_aread;
	vportal_functions.do_wait    = do_portal_wait;

	for (int i = 0; i < KPORTAL_MAX; ++i)
	{
		spinlock_init(&vportals[i].lock);
		vportals[i].resource   = RESOURCE_INITIALIZER;
		vportals[i].config     = ACTIVE_CONFIG_INITIALIZER;
		vportals[i].stats      = PSTATS_INITIALIZER;
		vportals[i].fn         = &vportal_functions;
		vportals[i].counters   = &vportal_counters;
	}

	vportalpool.communicators  = vportals;
	vportalpool.ncommunicators = KPORTAL_MAX;
}

/*============================================================================*
 * do_vportal_alloc()                                                         *
 *============================================================================*/

/**
 * @brief Searches for a free virtual portal.
 *
 * @param local  Local node ID.
 * @param remote Remote node ID (It can be -1).
 * @param port   Port ID.
 * @param type   Communication type (INPUT or OUTPUT).
 *
 * @returns Upon successful completion, the index of the virtual
 * portal in vportals tab is returned. Upon failure, a
 * negative number is returned instead.
 */
PRIVATE int do_vportal_alloc(int local, int remote, int port, int type)
{
	int fd;       /* Active portal ID.  */
	int portalid; /* Virtual portal ID. */
	struct active_config config;

	/* Allocates a physical portal port. */
	if ((fd = do_portal_alloc(local, remote, port, type)) < 0)
		return (fd);

	config.fd          = fd;
	config.local_addr  = ACTIVE_LADDRESS_COMPOSE(local, VPORTAL_GET_LADDRESS_PORT(fd), KPORTAL_PORT_NR);
	config.remote_addr = (type == ACTIVE_TYPE_OUTPUT) ?
		(ACTIVE_LADDRESS_COMPOSE(remote, port, KPORTAL_PORT_NR)) :
		(-1);

	/* Allocates a communicator. */
	if ((portalid = communicator_alloc(&vportalpool, &config, type)) < 0)
	{
		if (do_portal_release(fd) < 0)
			kpanic("[portal] Failed on release a portal port!");
	}
	else
	{
		spinlock_lock(&vportal_counters.lock);
			if (type == ACTIVE_TYPE_INPUT)
				vportal_counters.ncreates++;
			else
				vportal_counters.nopens++;
		spinlock_unlock(&vportal_counters.lock);
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
			ACTIVE_TYPE_INPUT
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
			ACTIVE_TYPE_OUTPUT
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
			ACTIVE_TYPE_INPUT
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
			ACTIVE_TYPE_OUTPUT
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
	spinlock_lock(&vportals[portalid].lock);

		ret = -EBADF;

		/* Bad virtual portal. */
		if (!resource_is_used(&vportals[portalid].resource))
			goto unlock;

		/* Bad virtual portal. */
		if (!resource_is_readable(&vportals[portalid].resource))
			goto unlock;

		ret = -EBUSY;

		/* Vportal already allowed a write. */
		if (communicator_is_allowed(&vportals[portalid]))
			goto unlock;

		/* Allows the virtual portal to read. */
		communicator_set_allowed(&vportals[portalid]);
		vportals[portalid].config.remote_addr =
			ACTIVE_LADDRESS_COMPOSE(remote, remote_port, KPORTAL_PORT_NR);

		ret = 0;

unlock:
	spinlock_unlock(&vportals[portalid].lock);

	return (ret);
}

/*============================================================================*
 * do_vportal_aread()                                                         *
 *============================================================================*/

/**
 * @brief Async reads from an virtual portal.
 *
 * @param mbxid  Virtual portal ID.
 * @param buffer User buffer.
 * @param size   Size of the buffer.
 *
 * @returns Upon successful completion, positive number is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int do_vportal_aread(int portalid, void * buffer, size_t size)
{
	vportals[portalid].config.buffer = buffer;
	vportals[portalid].config.size   = size;
	vportal_functions.do_comm        = do_portal_aread;

	return (communicator_operate(&vportals[portalid], ACTIVE_TYPE_INPUT));
}

/*============================================================================*
 * do_vportal_awrite()                                                        *
 *============================================================================*/

/**
 * @brief Async write from an virtual mailbox.
 *
 * @param mbxid  Virtual mailbox ID.
 * @param buffer User buffer.
 * @param size   Size of the buffer.
 *
 * @returns Upon successful completion, positive number is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int do_vportal_awrite(int portalid, const void * buffer, size_t size)
{
	vportals[portalid].config.buffer = buffer;
	vportals[portalid].config.size   = size;
	vportal_functions.do_comm        = do_portal_awrite;

	return (communicator_operate(&vportals[portalid], ACTIVE_TYPE_OUTPUT));
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
	return (communicator_wait(&vportals[portalid]));
}

/*============================================================================*
 * do_vportal_ioctl()                                                         *
 *============================================================================*/

/**
 * @brief Request an I/O operation on a virtual portal.
 *
 * @param mbxid   Virtual portal ID.
 * @param request Type of request.
 * @param args    Arguments of the request.
 *
 * @returns Upon successful completion, zero is returned.
 * Upon failure, a negative error code is returned instead.
 */
int do_vportal_ioctl(int portalid, unsigned request, va_list args)
{
	return (communicator_ioctl(&vportals[portalid], request, args));
}

/*============================================================================*
 * do_vportal_get_port()                                                      *
 *============================================================================*/

/**
 * @brief Gets a releated port id to a virtual portal.
 *
 * @param mbxid Logic ID of the target virtual portal.
 *
 * @returns Upon successful completion, a positive number is returned.
 * Upon failure, a negative error code is returned instead.
 */
int do_vportal_get_port(int portalid)
{
	int ret;

	if (!WITHIN(portalid, 0, KPORTAL_MAX))
		return (-EINVAL);

	spinlock_lock(&vportals[portalid].lock);

		if (!resource_is_used(&vportals[portalid].resource))
			ret = (-EBADF);
		else
			ret = (VPORTAL_GET_LADDRESS_PORT(vportals[portalid].config.fd));

	spinlock_unlock(&vportals[portalid].lock);

	return (ret);
}

/*============================================================================*
 * vportal_init()                                                             *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC void vportal_init(void)
{
	kprintf("[kernel][noc] initializing the kportal facility");

	/* Initializes the active portals structures. */
	do_portal_init();

	/* Initializes the virtual portals structures. */
	do_vportal_init();
}

#endif /* __TARGET_HAS_PORTAL && !__NANVIX_IKC_USES_ONLY_MAILBOX */
