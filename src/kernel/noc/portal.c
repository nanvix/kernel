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
PRIVATE union portal_mbuffer portalbuffers[KPORTAL_MESSAGE_BUFFERS_MAX] = {
	[0 ... (KPORTAL_MESSAGE_BUFFERS_MAX - 1)] = MBUFFER_INITIALIZER
};

/**
 * @brief Ubuffer resource pool.
 */
PRIVATE struct mbuffer_pool bufferpool = {
	portalbuffers,
	(KPORTAL_MESSAGE_BUFFERS_MAX - KPORTAL_AUX_BUFFERS_MAX),
	sizeof(union portal_mbuffer),
	SPINLOCK_UNLOCKED
};

/**
 * @brief Ubuffer resource pool.
 */
PRIVATE struct mbuffer_pool aux_bufferpool = {
	portalbuffers + (KPORTAL_MESSAGE_BUFFERS_MAX - KPORTAL_AUX_BUFFERS_MAX),
	KPORTAL_AUX_BUFFERS_MAX,
	sizeof(union portal_mbuffer),
	SPINLOCK_UNLOCKED
};

/*----------------------------------------------------------------------------*
 * Physical Portals.                                                          *
 *----------------------------------------------------------------------------*/

int wrapper_portal_allow(struct active *, int);
int wrapper_portal_copy(struct mbuffer *, const struct active_config *, int);
int portal_header_config(struct mbuffer *, const struct active_config *);
int portal_header_check(struct mbuffer *, const struct active_config *);

/**
 * @brief Portal ports.
 */
PRIVATE struct port ports[HW_PORTAL_MAX][KPORTAL_PORT_NR] = {
	[0 ... (HW_PORTAL_MAX - 1)] = {
		[0 ... (KPORTAL_PORT_NR - 1)] = {
			.resource    = {0, },
			.mbufferid   = -1,
			.mbufferpool = NULL,
		}
	}
};

/**
 * @brief Portal FIFOs.
 */
PRIVATE short fifos[HW_PORTAL_MAX][KPORTAL_PORT_NR] = {
	[0 ... (HW_PORTAL_MAX - 1)] = {
		[0 ... (KPORTAL_PORT_NR - 1)] = -1,
	}
};

/**
 * @brief Portales.
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
		.mbufferpool      = &bufferpool,
		.aux_bufferpool   = &aux_bufferpool,
		.do_create        = portal_create,
		.do_open          = portal_open,
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
 * @brief Portal pool.
 */
struct active_pool portalpool = {
	portals, HW_PORTAL_MAX
};

/*============================================================================*
 * do_portal_table_init()                                                     *
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

/*============================================================================*
 * wrapper_portal_allow()                                                     *
 *============================================================================*/

/**
 * @brief Allow a physical portal communication.
 *
 * @param act    Active resource.
 * @param remote Remote node ID.
 *
 * @returns Value return by portal_allow function.
 */
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

/*============================================================================*
 * wrapper_portal_copy()                                                      *
 *============================================================================*/

/**
 * @brief Copy a message.
 *
 * @param buf    Mbuffer resource.
 * @param config Communication's configuration.
 * @param type   Direction of the copy.
 *
 * @returns Zero is returned.
 */
int wrapper_portal_copy(struct mbuffer * buf, const struct active_config * config, int type)
{
	void * to;
	void * from;
	union portal_mbuffer * pbuf;

	pbuf = (union portal_mbuffer *) buf;

	if (type == ACTIVE_COPY_TO_MBUFFER)
	{
		to   = (void *) pbuf->message.data;
		from = (void *) config->buffer;
	}
	else
	{
		to   = (void *) config->buffer;
		from = (void *) pbuf->message.data;
	}

	kmemcpy(to, from, config->size);

	return (0);
}

/*============================================================================*
 * portal_header_config()                                                     *
 *============================================================================*/

/**
 * @brief Configurate a message header.
 *
 * @param buf    Mbuffer resource.
 * @param config Communication's configuration.
 *
 * @returns Zero is returned.
 */
int portal_header_config(struct mbuffer * mbuf, const struct active_config * config)
{
	mbuf->message.header.src  = config->local_addr;
	mbuf->message.header.dest = config->remote_addr;
	mbuf->message.header.size = config->size;

	return (0);
}

/*============================================================================*
 * portal_header_check()                                                      *
 *============================================================================*/

/**
 * @brief Checks if a message is to current configuration.
 *
 * @param buf    Mbuffer resource.
 * @param config Communication's configuration.
 *
 * @returns Non-zero if the mbuffer is destinate to current configuration.
 */
int portal_header_check(struct mbuffer * mbuf, const struct active_config * config)
{
	return ((mbuf->message.header.dest == config->local_addr) && (mbuf->message.header.src == config->remote_addr));
}

/*============================================================================*
 * do_portal_alloc()                                                          *
 *============================================================================*/

/**
 * @brief Allocate a physical portal.
 *
 * @param local  Local node ID.
 * @param remote Remote node ID (It can be -1).
 * @param port   Port ID.
 * @param type   Communication type (INPUT or OUTPUT).
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
 * @param mbxid Active portal ID.
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
 * @brief Async reads from an active.
 *
 * @param mbxid  Active portal ID.
 * @param config Communication's configuration.
 * @param stats  Structure to store statstics.
 *
 * @returns Upon successful completion, positive number is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC ssize_t do_portal_aread(int portalid, const struct active_config * config, struct pstats * stats)
{
	return (active_aread(&portalpool, portalid, config, stats));
}

/*============================================================================*
 * do_portal_awrite()                                                         *
 *============================================================================*/

/**
 * @brief Async writes from an active.
 *
 * @param mbxid  Active portal ID.
 * @param config Communication's configuration.
 * @param stats  Structure to store statstics.
 *
 * @returns Upon successful completion, positive number is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC ssize_t do_portal_awrite(int portalid, const struct active_config * config, struct pstats * stats)
{
	return (active_awrite(&portalpool, portalid, config, stats));
}

/*============================================================================*
 * do_portal_wait()                                                           *
 *============================================================================*/

/**
 * @brief Waits on a portal to finish an assynchronous operation.
 *
 * @param mbxid  Active portal ID.
 * @param config Communication's configuration.
 * @param stats  Structure to store statstics.
 *
 * @returns Upon successful completion, zero is returned.
 * Upon failure, a negative error code is returned instead.
 */
PUBLIC int do_portal_wait(int portalid, const struct active_config * config, struct pstats * stats)
{
	return (active_wait(&portalpool, portalid, config, stats));
}

/*============================================================================*
 * do_portal_init()                                                           *
 *============================================================================*/

/**
 * @todo Initializtion of the active portales and structures.
 */
PUBLIC void do_portal_init(void)
{
	int local;

	local = processor_node_get_num();

	/* Create the input portal. */
	KASSERT(_active_create(&portalpool, local) >= 0);

	/* Opens all portal interfaces. */
	for (int i = 0; i < PROCESSOR_NOC_NODES_NUM; ++i)
		KASSERT(_active_open(&portalpool, local, i) >= 0);

	/* Initializes the active portals locks. */
	do_portal_table_init();
}

#endif /* __TARGET_HAS_PORTAL */
