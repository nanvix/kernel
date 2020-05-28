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

#if __TARGET_HAS_PORTAL && !__NANVIX_IKC_USES_ONLY_MAILBOX

/**
 * @name Auxiliary macros.
 */
/**@{*/
#define GET_LADDRESS_FD(portalid)   (portalid / KPORTAL_PORT_NR) /**< Extracts fd from portalid.   */
#define GET_LADDRESS_PORT(portalid) (portalid % KPORTAL_PORT_NR) /**< Extracts port from portalid. */
/**@}*/

/*============================================================================*
 * Control Structures.                                                        *
 *============================================================================*/

/**
 * @name Pool variables.
 */
/**@{*/
PRIVATE union portal_mbuffer pbuffers[KPORTAL_MESSAGE_BUFFERS_MAX]; /**< Portal message buffer.                        */
PRIVATE uint64_t pbuffers_age;                                      /**< Counter used to set mbuffer age.              */
PRIVATE spinlock_t pbuffers_lock;                                   /**< Protection of the mbuffer pools.              */
PRIVATE struct mbuffer_pool pbufferpool;                            /**< Pool with all of mbuffer available.           */
PRIVATE struct mbuffer_pool pbufferpool_aux;                        /**< Pool with a subset of mbuffer in mbufferpool. */
/**@}*/

/**
 * @name Physical Portal variables.
 */
/**@{*/
PRIVATE struct port portalports[HW_PORTAL_MAX][KPORTAL_PORT_NR]; /**< Portal ports. */
PRIVATE short fifos[HW_PORTAL_MAX][KPORTAL_PORT_NR];             /**< Portal FIFOs. */
PRIVATE struct active portals[HW_PORTAL_MAX];                    /**< Portals.      */
PRIVATE struct active_pool portalpool;                           /**< Portal pool.  */
/**@}*/

/**
 * @name Prototype functions.
 */
/**@{*/
int wrapper_portal_allow(struct active *, int);
int portal_header_config(struct mbuffer *, const struct active_config *);
int portal_header_check(struct mbuffer *, const struct active_config *);
/**@}*/

/*============================================================================*
 * do_portal_table_init()                                                     *
 *============================================================================*/

/**
 * @brief Initializes the mbuffers table lock.
 */
void do_portal_table_init(void)
{
	/* Initializes the mbuffers. */
	for (int i = 0; i < KPORTAL_MESSAGE_BUFFERS_MAX; ++i)
	{
		pbuffers[i].abstract.resource = RESOURCE_INITIALIZER;
		pbuffers[i].abstract.age      = ~(0ULL);
		pbuffers[i].abstract.actid    = (-1);
		pbuffers[i].abstract.portid   = (-1);
		pbuffers[i].abstract.message  = MBUFFER_MESSAGE_INITIALIZER;
	}

	/* Initializes shared pool variables. */
	pbuffers_age = (0ULL);
	spinlock_init(&pbuffers_lock);

	/* Initializes principal mbuffers pool. */
	pbufferpool.mbuffers     = pbuffers;
	pbufferpool.nmbuffers    = KPORTAL_MESSAGE_BUFFERS_MAX;
	pbufferpool.mbuffer_size = sizeof(union portal_mbuffer);
	pbufferpool.curr_age     = &pbuffers_age;
	pbufferpool.lock         = &pbuffers_lock;

	/* Initializes auxiliary mbuffers pool. */
	pbufferpool_aux.mbuffers     = pbuffers + (KPORTAL_MESSAGE_BUFFERS_MAX - KPORTAL_AUX_BUFFERS_MAX);
	pbufferpool_aux.nmbuffers    = KPORTAL_AUX_BUFFERS_MAX;
	pbufferpool_aux.mbuffer_size = sizeof(union portal_mbuffer);
	pbufferpool_aux.curr_age     = &pbuffers_age;
	pbufferpool_aux.lock         = &pbuffers_lock;

	/* Initializes the portals. */
	for (int i = 0; i < HW_PORTAL_MAX; ++i)
	{
		/* Initializes main variables. */
		spinlock_init(&portals[i].lock);
		portals[i].hwfd     = -1;
		portals[i].local    = -1;
		portals[i].remote   = -1;
		portals[i].refcount =  0;
		portals[i].size     = (KPORTAL_MESSAGE_HEADER_SIZE + KPORTAL_MESSAGE_DATA_SIZE);

		/* Initializes port pool. */
		portals[i].portpool.ports      = NULL;
		portals[i].portpool.nports     = KPORTAL_PORT_NR;
		portals[i].portpool.used_ports = 0;
		portals[i].portpool.ports      = portalports[i];

		/* Initializes request fifo. */
		portals[i].requests.head         = 0;
		portals[i].requests.tail         = 0;
		portals[i].requests.max_capacity = KPORTAL_PORT_NR;
		portals[i].requests.nelements    = 0;
		portals[i].requests.fifo         = fifos[i];

		/* Initializes the portals ports and FIFOs. */
		for (int j = 0; j < KPORTAL_PORT_NR; ++j)
		{
			portalports[i][j].resource    = RESOURCE_INITIALIZER;
			portalports[i][j].mbufferid   = -1;
			portalports[i][j].mbufferpool = NULL;

			fifos[i][j] = -1;
		}

		/* Initializes auxiliary functions. */
		portals[i].mbufferpool      = &pbufferpool;
		portals[i].mbufferpool_aux  = &pbufferpool_aux;
		portals[i].do_create        = portal_create;
		portals[i].do_open          = portal_open;
		portals[i].do_allow         = wrapper_portal_allow;
		portals[i].do_aread         = portal_aread;
		portals[i].do_awrite        = portal_awrite;
		portals[i].do_wait          = portal_wait;
		portals[i].do_header_config = portal_header_config;
		portals[i].do_header_check  = portal_header_check;
	}

	/* Initializes portal pool. */
	portalpool.actives    = portals;
	portalpool.nactives   = HW_PORTAL_MAX;
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
int portal_header_config(struct mbuffer * buf, const struct active_config * config)
{
	buf->message.header.src  = config->local_addr;
	buf->message.header.dest = config->remote_addr;
	buf->message.header.size = config->size;

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
int portal_header_check(struct mbuffer * buf, const struct active_config * config)
{
	return ((buf->message.header.dest == config->local_addr) && (buf->message.header.src == config->remote_addr));
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
 * @param portalid Active portal ID.
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
 * @param portalid  Active portal ID.
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
 * @param portalid  Active portal ID.
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
 * @param portalid  Active portal ID.
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

	/* Initializes the portals structures. */
	do_portal_table_init();

	/* Create the input portal. */
	KASSERT(_active_create(&portalpool, local) >= 0);

	/* Opens all portal interfaces. */
	for (int i = 0; i < PROCESSOR_NOC_NODES_NUM; ++i)
		KASSERT(_active_open(&portalpool, local, i) >= 0);
}

#endif /* __TARGET_HAS_PORTAL && !__NANVIX_IKC_USES_ONLY_MAILBOX */
