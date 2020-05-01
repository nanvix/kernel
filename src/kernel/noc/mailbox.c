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
#include <nanvix/hlib.h>
#include <nanvix/kernel/mailbox.h>
#include <posix/errno.h>
#include <posix/stdarg.h>

#include "active.h"
#include "mailbox.h"

#if __TARGET_HAS_MAILBOX

/**
 * @name Auxiliary macros.
 */
/**@{*/
#define GET_LADDRESS_FD(mbxid)   (mbxid / MAILBOX_PORT_NR) /**< Extracts fd from mbxid.   */
#define GET_LADDRESS_PORT(mbxid) (mbxid % MAILBOX_PORT_NR) /**< Extracts port from mbxid. */
/**@}*/

/*============================================================================*
 * Control Structures.                                                        *
 *============================================================================*/

/**
 * @name Pool variables.
 */
/**@{*/
PRIVATE union mailbox_mbuffer mbuffers[KMAILBOX_MESSAGE_BUFFERS_MAX]; /**< Mailbox message buffer.                       */
PRIVATE uint64_t mbuffers_age;                                        /**< Counter used to set mbuffer age.              */
PRIVATE spinlock_t mbuffers_lock;                                     /**< Protection of the mbuffer pools.              */
PRIVATE struct mbuffer_pool mbufferpool;                              /**< Pool with all of mbuffer available.           */
PRIVATE struct mbuffer_pool mbufferpool_aux;                          /**< Pool with a subset of mbuffer in mbufferpool. */
/**@}*/

/**
 * @name Physical Mailbox variables.
 */
/**@{*/
PRIVATE struct port mbxports[HW_MAILBOX_MAX][MAILBOX_PORT_NR]; /**< Mailbox ports. */
PRIVATE short fifos[HW_MAILBOX_MAX][MAILBOX_PORT_NR];          /**< Mailbox FIFOs. */
PRIVATE struct active mailboxes[HW_MAILBOX_MAX];               /**< Mailboxes.    */
PRIVATE struct active_pool mbxpool;                            /**< Mailbox pool. */
/**@}*/

/**
 * @name Prototype functions.
 */
/**@{*/
int wrapper_mailbox_open(int, int);
int wrapper_mailbox_allow(struct active *, int);
int wrapper_mailbox_copy(struct mbuffer *, const struct active_config *, int);
int mailbox_header_config(struct mbuffer *, const struct active_config *);
int mailbox_header_check(struct mbuffer *, const struct active_config *);
/**@}*/

/*============================================================================*
 * do_mailbox_table_init()                                                    *
 *============================================================================*/

/**
 * @brief Initializes the mbuffers table lock.
 */
void do_mailbox_table_init(void)
{
	/* Initializes the mbuffers. */
	for (int i = 0; i < KMAILBOX_MESSAGE_BUFFERS_MAX; ++i)
	{
		mbuffers[i].abstract.resource = RESOURCE_INITIALIZER;
		mbuffers[i].abstract.age      = ~(0ULL);
		mbuffers[i].abstract.message  = MBUFFER_MESSAGE_INITIALIZER;
	}

	/* Initializes shared pool variables. */
	mbuffers_age = 0ULL;
	spinlock_init(&mbuffers_lock);

	/* Initializes principal mbuffers pool. */
	mbufferpool.mbuffers     = mbuffers;
	mbufferpool.nmbuffers    = KMAILBOX_MESSAGE_BUFFERS_MAX;
	mbufferpool.mbuffer_size = sizeof(union mailbox_mbuffer);
	mbufferpool.curr_age     = &mbuffers_age;
	mbufferpool.lock         = &mbuffers_lock;

	/* Initializes auxiliary mbuffers pool. */
	mbufferpool_aux.mbuffers     = mbuffers + (KMAILBOX_MESSAGE_BUFFERS_MAX - KMAILBOX_AUX_BUFFERS_MAX);
	mbufferpool_aux.nmbuffers    = KMAILBOX_AUX_BUFFERS_MAX;
	mbufferpool_aux.mbuffer_size = sizeof(union mailbox_mbuffer);
	mbufferpool_aux.curr_age     = &mbuffers_age;
	mbufferpool_aux.lock         = &mbuffers_lock;

	/* Initializes the mailboxes. */
	for (int i = 0; i < HW_MAILBOX_MAX; ++i)
	{
		/* Initializes main variables. */
		spinlock_init(&mailboxes[i].lock);
		mailboxes[i].hwfd           = -1;
		mailboxes[i].local          = -1;
		mailboxes[i].remote         = -1;
		mailboxes[i].refcount       =  0;
		mailboxes[i].size           = HAL_MAILBOX_MSG_SIZE;

		/* Initializes port pool. */
		mailboxes[i].portpool.ports      = NULL;
		mailboxes[i].portpool.nports     = MAILBOX_PORT_NR;
		mailboxes[i].portpool.used_ports = 0;
		mailboxes[i].portpool.ports      = mbxports[i];

		/* Initializes request fifo. */
		mailboxes[i].requests.head         = 0;
		mailboxes[i].requests.tail         = 0;
		mailboxes[i].requests.max_capacity = MAILBOX_PORT_NR;
		mailboxes[i].requests.nelements    = 0;
		mailboxes[i].requests.fifo         = fifos[i];

		/* Initializes the mailboxes ports and FIFOs. */
		for (int j = 0; j < MAILBOX_PORT_NR; ++j)
		{
			mbxports[i][j].resource    = RESOURCE_INITIALIZER;
			mbxports[i][j].mbufferid   = -1;
			mbxports[i][j].mbufferpool = NULL;

			fifos[i][j] = -1;
		}

		/* Initializes auxiliary functions. */
		mailboxes[i].mbufferpool      = &mbufferpool;
		mailboxes[i].mbufferpool_aux  = &mbufferpool_aux;
		mailboxes[i].do_create        = mailbox_create;
		mailboxes[i].do_open          = wrapper_mailbox_open;
		mailboxes[i].do_allow         = wrapper_mailbox_allow;
		mailboxes[i].do_aread         = mailbox_aread;
		mailboxes[i].do_awrite        = mailbox_awrite;
		mailboxes[i].do_wait          = mailbox_wait;
		mailboxes[i].do_copy          = wrapper_mailbox_copy;
		mailboxes[i].do_header_config = mailbox_header_config;
		mailboxes[i].do_header_check  = mailbox_header_check;
	}

	/* Initializes mailbox pool. */
	mbxpool.actives  = mailboxes;
	mbxpool.nactives = HW_MAILBOX_MAX;
}

/*============================================================================*
 * wrapper_mailbox_open()                                                     *
 *============================================================================*/

/**
 * @brief Open a physical mailbox.
 *
 * @param local  Local node ID.
 * @param remote Remote node ID.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
int wrapper_mailbox_open(int local, int remote)
{
	UNUSED(local);

	return (mailbox_open(remote));
}

/*============================================================================*
 * wrapper_mailbox_allow()                                                     *
 *============================================================================*/

/**
 * @brief Allow a physical mailbox communication.
 *
 * @param act    Active resource.
 * @param remote Remote node ID.
 *
 * @returns Zero is returned.
 */
int wrapper_mailbox_allow(struct active * act, int remote)
{
	UNUSED(act);
	UNUSED(remote);

	return (0);
}

/*============================================================================*
 * wrapper_mailbox_copy()                                                     *
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
int wrapper_mailbox_copy(struct mbuffer * buf, const struct active_config * config, int type)
{
	void * to;
	void * from;
	union mailbox_mbuffer * mbuf;

	mbuf = (union mailbox_mbuffer *) buf;

	if (type == ACTIVE_COPY_TO_MBUFFER)
	{
		to   = (void *) mbuf->message.data;
		from = (void *) config->buffer;
	}
	else
	{
		to   = (void *) config->buffer;
		from = (void *) mbuf->message.data;
	}

	kmemcpy(to, from, config->size);

	return (0);
}

/*============================================================================*
 * mailbox_header_config()                                                    *
 *============================================================================*/

/**
 * @brief Configurate a message header.
 *
 * @param buf    Mbuffer resource.
 * @param config Communication's configuration.
 *
 * @returns Zero is returned.
 */
int mailbox_header_config(struct mbuffer * mbuf, const struct active_config * config)
{
	mbuf->message.header.dest = config->remote_addr;

	return (0);
}

/*============================================================================*
 * mailbox_header_check()                                                     *
 *============================================================================*/

/**
 * @brief Checks if a message is to current configuration.
 *
 * @param buf    Mbuffer resource.
 * @param config Communication's configuration.
 *
 * @returns Non-zero if the mbuffer is destinate to current configuration.
 */
int mailbox_header_check(struct mbuffer * mbuf, const struct active_config * config)
{
	return (mbuf->message.header.dest == config->local_addr);
}

/*============================================================================*
 * do_mailbox_alloc()                                                         *
 *============================================================================*/

/**
 * @brief Allocate a physical mailbox.
 *
 * @param local  Local node ID.
 * @param remote Remote node ID (It can be -1).
 * @param port   Port ID.
 * @param type   Communication type (INPUT or OUTPUT).
 *
 * @returns Upon successful completion, the ID of the active mailbox is
 * returned. Upon failure, a negative error code is returned instead.
 */
PUBLIC int do_mailbox_alloc(int local, int remote, int port, int type)
{
	return (active_alloc(&mbxpool, local, remote, port, type));
}

/*============================================================================*
 * do_mailbox_release()                                                       *
 *============================================================================*/

/**
 * @brief Releases a physical mailbox.
 *
 * @param mbxid Active mailbox ID.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int do_mailbox_release(int mbxid)
{
	return (active_release(&mbxpool, mbxid));
}

/*============================================================================*
 * do_mailbox_aread()                                                         *
 *============================================================================*/

/**
 * @brief Async reads from an active.
 *
 * @param mbxid  Active mailbox ID.
 * @param config Communication's configuration.
 * @param stats  Structure to store statstics.
 *
 * @returns Upon successful completion, positive number is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC ssize_t do_mailbox_aread(int mbxid, const struct active_config * config, struct pstats * stats)
{
	return (active_aread(&mbxpool, mbxid, config, stats));
}

/*============================================================================*
 * do_mailbox_awrite()                                                        *
 *============================================================================*/

/**
 * @brief Async writes from an active.
 *
 * @param mbxid  Active mailbox ID.
 * @param config Communication's configuration.
 * @param stats  Structure to store statstics.
 *
 * @returns Upon successful completion, positive number is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC ssize_t do_mailbox_awrite(int mbxid, const struct active_config * config, struct pstats * stats)
{
	return (active_awrite(&mbxpool, mbxid, config, stats));
}

/*============================================================================*
 * do_mailbox_wait()                                                          *
 *============================================================================*/

/**
 * @brief Waits on a mailbox to finish an assynchronous operation.
 *
 * @param mbxid  Active mailbox ID.
 * @param config Communication's configuration.
 * @param stats  Structure to store statstics.
 *
 * @returns Upon successful completion, zero is returned.
 * Upon failure, a negative error code is returned instead.
 */
PUBLIC int do_mailbox_wait(int mbxid, const struct active_config * config, struct pstats * stats)
{
	return (active_wait(&mbxpool, mbxid, config, stats));
}

/*============================================================================*
 * do_mailbox_init()                                                          *
 *============================================================================*/

/**
 * @todo Initializtion of the active mailboxes and structures.
 */
PUBLIC void do_mailbox_init(void)
{
	int local;

	local = processor_node_get_num();

	/* Initializes the mailboxes structures. */
	do_mailbox_table_init();

	/* Create the input mailbox. */
	KASSERT(_active_create(&mbxpool, local) >= 0);

	/* Opens all mailbox interfaces. */
	for (int i = 0; i < PROCESSOR_NOC_NODES_NUM; ++i)
		KASSERT(_active_open(&mbxpool, local, i) >= 0);

}

#endif /* __TARGET_HAS_MAILBOX */
