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
 * @brief Extracts fd and port from mbxid.
 */
/**@{*/
#define GET_LADDRESS_FD(mbxid)   (mbxid / MAILBOX_PORT_NR)
#define GET_LADDRESS_PORT(mbxid) (mbxid % MAILBOX_PORT_NR)
/**@}*/

#define MAILBOX_MBUFFER_SRC (-1)

/*============================================================================*
 * Control Structures.                                                        *
 *============================================================================*/

/*----------------------------------------------------------------------------*
 * Mailbox Message Buffer.                                                    *
 *----------------------------------------------------------------------------*/

/**
 * @brief Mailbox message buffer.
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
		struct mailbox_message message;
	};
} ubuffers[KMAILBOX_MESSAGE_BUFFERS_MAX] = {
	[0 ... (KMAILBOX_MESSAGE_BUFFERS_MAX - 1)] = {
		.message = {
			.dest = -1,
			.data = {'\0'},
		},
	},
};

/**
 * @brief Mbuffer resource pool.
 */
PRIVATE struct mbuffer_pool ubufferpool = {
	ubuffers,
	KMAILBOX_MESSAGE_BUFFERS_MAX,
	sizeof(union ubuffer),
	SPINLOCK_UNLOCKED
};

/*----------------------------------------------------------------------------*
 * Physical Mailboxes.                                                        *
 *----------------------------------------------------------------------------*/

int wrapper_mailbox_open(int, int);
int wrapper_mailbox_allow(struct active *, int);
int wrapper_mailbox_copy(struct mbuffer *, const struct active_config *, int);
int mailbox_header_config(struct mbuffer *, const struct active_config *);
int mailbox_header_check(struct mbuffer *, const struct active_config *);

/**
 * @brief Mailbox ports.
 */
PRIVATE struct port ports[HW_MAILBOX_MAX][MAILBOX_PORT_NR] = {
	[0 ... (HW_MAILBOX_MAX - 1)] = {
		[0 ... (MAILBOX_PORT_NR - 1)] = {
			.resource  = {0, },
			.mbufferid = -1,
		}
	}
};

/**
 * @brief Mailbox FIFOs.
 */
PRIVATE short fifos[HW_MAILBOX_MAX][MAILBOX_PORT_NR] = {
	[0 ... (HW_MAILBOX_MAX - 1)] = {
		[0 ... (MAILBOX_PORT_NR - 1)] = -1,
	}
};

/**
 * @brief Mailboxes.
 */
struct active mailboxes[HW_MAILBOX_MAX] = {
	[0 ... (HW_MAILBOX_MAX - 1)] {
		.resource   = {0, },
		.hwfd       = -1,
		.local      = -1,
		.remote     = -1,
		.refcount   =  0,
		.size       = HAL_MAILBOX_MSG_SIZE,
		.portpool       = {
			.ports      = NULL,
			.nports     = MAILBOX_PORT_NR,
			.used_ports = 0,
		},
		.requests         = {
			.head         = 0,
			.tail         = 0,
			.max_capacity = MAILBOX_PORT_NR,
			.nelements    = 0,
			.fifo         = NULL,
		},
		.mbufferpool      = &ubufferpool,
		.do_create        = mailbox_create,
		.do_open          = wrapper_mailbox_open,
		.do_allow         = wrapper_mailbox_allow,
		.do_aread         = mailbox_aread,
		.do_awrite        = mailbox_awrite,
		.do_wait          = mailbox_wait,
		.do_copy          = wrapper_mailbox_copy,
		.do_header_config = mailbox_header_config,
		.do_header_check  = mailbox_header_check,
	},
};

/**
 * @brief Mailbox pool.
 */
struct active_pool mbxpool = {
	mailboxes, HW_MAILBOX_MAX
};

/*============================================================================*
 * do_mailbox_table_init()                                                    *
 *============================================================================*/

/**
 * @brief Initializes the mbuffers table lock.
 */
void do_mailbox_table_init(void)
{
	for (int i = 0; i < HW_MAILBOX_MAX; ++i)
	{
		spinlock_init(&mailboxes[i].lock);

		mailboxes[i].portpool.ports = ports[i];
		mailboxes[i].requests.fifo  = fifos[i];
	}
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
	mbuf->message.dest = config->remote_addr;

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
	int local_addr = ACTIVE_LADDRESS_COMPOSE(processor_node_get_num(), GET_LADDRESS_PORT(config->fd), MAILBOX_PORT_NR);

	return (mbuf->message.dest == local_addr);
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

	/* Create the input mailbox. */
	KASSERT(_active_create(&mbxpool, local) >= 0);

	/* Opens all mailbox interfaces. */
	for (int i = 0; i < PROCESSOR_NOC_NODES_NUM; ++i)
		KASSERT(_active_open(&mbxpool, local, i) >= 0);

	/* Initializes the active mailboxes locks. */
	do_mailbox_table_init();
}

#endif /* __TARGET_HAS_MAILBOX */
