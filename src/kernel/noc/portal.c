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

#include "active.h"
#include "portal.h"

#if __TARGET_HAS_PORTAL

/**
 * @brief Extracts fd and port from portalid.
 */
/**@{*/
#define GET_LADDRESS_FD(portalid)   (portalid / KPORTAL_PORT_NR)
#define GET_LADDRESS_PORT(portalid) (portalid % KPORTAL_PORT_NR)
/**@}*/

/*============================================================================*
 * Control Structures.                                                        *
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * Portal Message Buffer.                                                     *
 *----------------------------------------------------------------------------*/

/**
 * @brief Portal message buffer.
 */
PRIVATE union ubuffer
{
	struct mbuffer abstract;
	struct
	{
		/*
		* XXX: Don't Touch! This Must Come First!
		*/
		struct resource resource;
		struct portal_message message;
	};
} ubuffers[KPORTAL_MESSAGE_BUFFERS_MAX] = {
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
 * @brief Ubuffer resource pool.
 */
PRIVATE struct mbuffer_pool ubufferpool = {
	ubuffers,
	KPORTAL_MESSAGE_BUFFERS_MAX,
	sizeof(union ubuffer),
	SPINLOCK_UNLOCKED
};

/*----------------------------------------------------------------------------*
 * Physical Portals.                                                          *
 *----------------------------------------------------------------------------*/

int wrapper_portal_open(int, int);
int wrapper_portal_allow(struct active *, int);
int wrapper_portal_copy(struct mbuffer *, const struct comm_config *, int);
int portal_header_config(struct mbuffer *, const struct comm_config *);
int portal_header_check(struct mbuffer *, const struct comm_config *);

/**
 * @brief Table of ports.
 */
PRIVATE struct port ports[HW_PORTAL_MAX][KPORTAL_PORT_NR] = {
	[0 ... (HW_PORTAL_MAX - 1)] = {
		[0 ... (KPORTAL_PORT_NR - 1)] = {
			.resource  = {0, },
			.mbufferid = -1,
		}
	}
};

PRIVATE short fifos[HW_PORTAL_MAX][KPORTAL_PORT_NR] = {
	[0 ... (HW_PORTAL_MAX - 1)] = {
		[0 ... (KPORTAL_PORT_NR - 1)] = -1,
	}
};

/**
 * @brief Table of active portals.
 */
struct active portals[HW_PORTAL_MAX] = {
	[0 ... (HW_PORTAL_MAX - 1)] {
		.resource   = {0, },
		.hwfd       = -1,
		.local      = -1,
		.remote     = -1,
		.refcount   =  0,
		.size       = (KPORTAL_MESSAGE_HEADER_SIZE + HAL_PORTAL_MAX_SIZE),
		.portpool       = {
			.ports      = NULL,
			.nports     = KPORTAL_PORT_NR,
			.used_ports = 0,
		},
		.requests         = {
			.head         = 0,
			.tail         = 0,
			.max_capacity = KPORTAL_PORT_NR,
			.nelements    = 0,
			.fifo         = NULL,
		},
		.mbufferpool      = &ubufferpool,
		.do_create        = portal_create,
		.do_open          = wrapper_portal_open,
		.do_allow         = wrapper_portal_allow,
		.do_aread         = portal_aread,
		.do_awrite        = portal_awrite,
		.do_wait          = portal_wait,
		.do_copy          = wrapper_portal_copy,
		.do_header_config = portal_header_config,
		.do_header_check  = portal_header_check,
	},
};

/**
 * @brief Resource pool.
 */
struct active_pool portalpool = {
	portals, HW_PORTAL_MAX
};

/*============================================================================*
 * do_portal_table_init()                                                    *
 *============================================================================*/


/**
 * @brief Initializes the mbuffers table lock.
 */
void do_portal_table_init(void)
{
	for (int i = 0; i < HW_PORTAL_MAX; ++i)
	{
		spinlock_init(&portals[i].lock);

		portals[i].portpool.ports = ports[i];
		portals[i].requests.fifo  = fifos[i];
	}
}

int wrapper_portal_open(int local, int remote)
{
	return (portal_open(local, remote));
}

int wrapper_portal_allow(struct active * act, int remote)
{
	if (active_is_allowed(act))
	{
		if (act->remote != remote)
			return (-EBUSY);
		else
			return (0);
	}

	return (portal_allow(act->hwfd, remote));
}

int wrapper_portal_copy(struct mbuffer * buf, const struct comm_config * config, int type)
{
	void * to;
	void * from;
	union ubuffer * ubuf;

	ubuf = (union ubuffer *) buf;

	if (type == ACTIVE_COPY_TO_MBUFFER)
	{
		to   = (void *) ubuf->message.data;
		from = (void *) config->buffer;
	}
	else
	{
		to   = (void *) config->buffer;
		from = (void *) ubuf->message.data;
	}

	kmemcpy(to, from, config->size);

	return (0);
}

int portal_header_config(struct mbuffer * mbuf, const struct comm_config * config)
{
	mbuf->message.src  = ACTIVE_LADDRESS_COMPOSE(processor_node_get_num(), GET_LADDRESS_PORT(config->fd), KPORTAL_PORT_NR);
	mbuf->message.dest = config->remote;
	mbuf->message.size = config->size;

	return (0);
}

int portal_header_check(struct mbuffer * mbuf, const struct comm_config * config)
{
	int local_addr = ACTIVE_LADDRESS_COMPOSE(processor_node_get_num(), GET_LADDRESS_PORT(config->fd), KPORTAL_PORT_NR);

	return ((mbuf->message.dest == local_addr) && (mbuf->message.src == config->remote));
}

/*============================================================================*
 * do_portal_alloc()                                                          *
 *============================================================================*/

/**
 * @brief Creates a physical portal.
 *
 * @param local  Local node ID.
 * @param remote Remote node ID.
 * @param port   Target port in target nodenum node.
 *
 * @returns Upon successful completion, the ID of the active portal is
 * returned. Upon failure, a negative error code is returned instead.
 */
PUBLIC int do_portal_alloc(int local, int remote, int port, int type)
{
	return (active_alloc(&portalpool, local, remote, port, type));
}

/*============================================================================*
 * do_portal_release()                                                        *
 *============================================================================*/

/**
 * @brief Releases a physical portal.
 *
 * @param portalid Physical portal ID.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int do_portal_release(int portalid)
{
	return (active_release(&portalpool, portalid));
}

/*============================================================================*
 * do_portal_aread()                                                          *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC ssize_t do_portal_aread(int portalid, const struct comm_config * config, struct pstats * stats)
{
	return (active_aread(&portalpool, portalid, config, stats));
}

/*============================================================================*
 * do_portal_awrite()                                                         *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC ssize_t do_portal_awrite(int portalid, const struct comm_config * config, struct pstats * stats)
{
	return (active_awrite(&portalpool, portalid, config, stats));
}

/*============================================================================*
 * do_portal_wait()                                                           *
 *============================================================================*/

/**
 * @brief Waits on a virtual portal to finish an assynchronous operation.
 *
 * @param portalid Logic ID of the target virtual portal.
 *
 * @returns Upon successful completion, a positive number is returned.
 * Upon failure, a negative error code is returned instead.
 */
PUBLIC int do_portal_wait(int portalid, const struct comm_config * config, struct pstats * stats)
{
	return (active_wait(&portalpool, portalid, config, stats));
}

/*============================================================================*
 * Initialization functions                                                   *
 *============================================================================*/

/*============================================================================*
 * do_portal_init()                                                           *
 *============================================================================*/

/**
 * @todo TODO: Provide a detailed description for this function.
 */
PUBLIC void do_portal_init(void)
{
	int local;

	local = processor_node_get_num();

	/* Create the input portal. */
	KASSERT(active_create(&portalpool, local) >= 0);

	/* Opens all portal interfaces. */
	for (int i = 0; i < PROCESSOR_NOC_NODES_NUM; ++i)
		KASSERT(active_open(&portalpool, local, i) >= 0);

	/* Initializes the active portals locks. */
	do_portal_table_init();
}

#endif /* __TARGET_HAS_PORTAL */
