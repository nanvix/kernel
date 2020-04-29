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
#include <posix/errno.h>
#include <posix/stdarg.h>

#include "active.h"

/**
 * @name Auxiliar macros
 */
/**@{*/
#define ACTIVE_GET_NR_PORTS(_act) (_act->portpool.nports)                                            /**< Extracts #ports. */
#define ACTIVE_GET_LADDRESS_FD(_act, _id)   ((_id >= 0) ? (_id / ACTIVE_GET_NR_PORTS(_act)) : (_id)) /**< Extracts actid.  */
#define ACTIVE_GET_LADDRESS_PORT(_act, _id) ((_id >= 0) ? (_id % ACTIVE_GET_NR_PORTS(_act)) : (_id)) /**< Extracts portid. */
/**@}*/

/**
 * @brief Any source identification
 */
#define ACTIVE_ANY_SRC (-1)

/*============================================================================*
 * do_register_request()                                                      *
 *============================================================================*/

/**
 * @brief Compute @p x % @p y where @p y must be a power-of-2 (1, 2, 4, 8, ...).
 *
 * @param n Operator
 * @param m Module factor.
 *
 * @returns @p x % @p y.
 */
PRIVATE int modulus_power2(int x, int y)
{
	/* y must be a power-of-2. */
	KASSERT((y & (y - 1)) == 0);

	return (x & (y - 1));
}

/*============================================================================*
 * do_register_request()                                                      *
 *============================================================================*/

/**
 * @brief Request a operation for @p active on the physical port id @p port.
 *
 * @param active Active resource.
 * @param port   Port ID.
 *
 * @returns Upon successful register, zero is returned. Upon failure,
 * a negative error code is returned instead.
 */
PRIVATE int do_request_operation(struct active * active, int port)
{
	int head;                        /* FIFO head. */
	struct requests_fifo * requests; /* FIFO.      */

	requests = &active->requests;

	/* Is the FIFO full? */
	if (requests->nelements == requests->max_capacity)
		return (-EBUSY);

	/* Register request. */
	head = requests->head;
	requests->fifo[head] = port;

	/* Updates head. */
	requests->head = modulus_power2((head + 1), requests->max_capacity);
	requests->nelements++;

	return (0);
}

/*============================================================================*
 * do_request_complete()                                                      *
 *============================================================================*/

/**
 * @brief Complete a operation on the physical port @p port.
 *
 * @param active Active resource.
 * @param port   Port ID.
 *
 * @returns Upon successful register, zero is returned. Upon failure,
 * a negative error code is returned instead.
 */
PRIVATE int do_request_complete(struct active * active, int port)
{
	int tail;                        /* FIFO tail. */
	struct requests_fifo * requests; /* FIFO.      */

	requests = &active->requests;

	tail = requests->tail;

	/* Isn't it his turn? */
	if (requests->fifo[tail] != port)
		return (-EINVAL);

	/* Complete the request. */
	requests->fifo[tail] = -1;
	requests->tail = modulus_power2((tail + 1), requests->max_capacity);
	requests->nelements--;

	return (0);
}

/*============================================================================*
 * do_request_verify()                                                        *
 *============================================================================*/

/**
 * @brief Verifies if is the turns of the @p active.
 *
 * @param active Active resource.
 * @param port   Port ID.
 *
 * @returns Non-zero if is its turns, zero otherwise.
 */
PRIVATE int do_request_verify(struct active * active, int port)
{
	if (!active->requests.nelements)
		return (0);

	return (active->requests.fifo[active->requests.tail] == port);
}

/*============================================================================*
 * Physical active functions                                                 *
 *============================================================================*/

/*============================================================================*
 * active_search()                                                            *
 *============================================================================*/

/**
 * @brief Validate a call.
 *
 * @param pool Active resource pool.
 * @param fd   Compose address by active and port id.
 *
 * @returns Non-zero if its valid, zero otherwize.
 */
PRIVATE int active_valid_call(const struct active_pool * pool, const int fd)
{
	int actid;  /* Active ID. */
	int portid; /* Port ID.   */

	/* Valid pool pointer. */
	if (pool == NULL)
		return (0);

	actid = ACTIVE_GET_LADDRESS_FD(pool->actives, fd);

	/* Active ID. */
	if (!WITHIN(actid, 0, pool->nactives))
		return (0);

	portid = ACTIVE_GET_LADDRESS_PORT(pool->actives, fd);

	/* Port ID. */
	return (WITHIN(portid, 0, ACTIVE_GET_NR_PORTS((&pool->actives[actid]))));
}

/*============================================================================*
 * active_search()                                                            *
 *============================================================================*/

/**
 * @name Helper Macros for active_search()
 */
/**@{*/

/**
 * @brief Asserts an input active.
 */
#define ACTIVE_SEARCH_IS_INPUT(_act, _type) \
	((_type == ACTIVE_TYPE_INPUT) && !resource_is_readable(&_act->resource))

/**
 * @brief Asserts an output active.
 */
#define ACTIVE_SEARCH_IS_OUTPUT(_act, _type) \
	 ((_type == ACTIVE_TYPE_OUTPUT) && !resource_is_writable(&_act->resource))
/**@}*/

/**
 * @brief Searches for an active HW active.
 *
 * Searches for a active in the table of activees.
 *
 * @param pool   Active resource pool.
 * @param local  Local node ID.
 * @param remote Remote node ID (It can be -1).
 * @param type   Communication type (INPUT or OUTPUT).
 *
 * @returns Upon successful completion, the ID of the active found is
 * returned. Upon failure, a negative error code is returned instead.
 */
PRIVATE int active_search(
	const struct active_pool * pool,
	int local,
	int remote,
	int type
)
{
	struct active * active; /* Active pointer. */

	KASSERT(pool != NULL);

	/* Search for a free synchronization point. */
	for (int i = 0; i < pool->nactives; i++)
	{
		active = &pool->actives[i];

		spinlock_lock(&active->lock);

			if (!resource_is_used(&active->resource))
			{
				spinlock_unlock(&active->lock);
				continue;
			}

			if (ACTIVE_SEARCH_IS_INPUT(active, type))
			{
				spinlock_unlock(&active->lock);
				continue;
			}

			else if (ACTIVE_SEARCH_IS_OUTPUT(active, type))
			{
				spinlock_unlock(&active->lock);
				continue;
			}

			/* Not the node we are looking for. */
			if ((active->local != local) || (active->remote != remote))
			{
				spinlock_unlock(&active->lock);
				continue;
			}

		spinlock_unlock(&active->lock);

		return (i);
	}

	return (-EBUSY);
}

/*============================================================================*
 * active_alloc()                                                             *
 *============================================================================*/

/**
 * @brief Allocate a active resource.
 *
 * @param pool   Active resource pool.
 * @param local  Local node ID.
 * @param remote Remote node ID (It can be -1).
 * @param portid Port ID.
 * @param type   Communication type (INPUT or OUTPUT).
 *
 * @returns Upon successful completion, the ID of the active is
 * returned. Upon failure, a negative error code is returned instead.
 */
PUBLIC int active_alloc(
	const struct active_pool * pool,
	int local,
	int remote,
	int portid,
	int type
)
{
	int ret;                     /* Return value.        */
	int actid;                   /* Hardware active ID. */
	struct port * port;          /* Port pointer.        */
	struct active * active;      /* Active pointer.      */
	struct port_pool * portpool; /* Port pool pointer.   */

	KASSERT(pool != NULL);
	ret = (-EINVAL);

	/* Search target hardware active. */
	if ((actid = active_search(pool, local, remote, type)) < 0)
		return (actid);

	active = &pool->actives[actid];
	portpool = &active->portpool;

	spinlock_lock(&active->lock);

		/* Choose a port of output active. */
		if (type == ACTIVE_TYPE_OUTPUT)
		{
			if ((portid = portpool_choose_port(portpool)) < 0)
			{
				ret = portid;
				goto error;
			}
		}

		/* Choose a port of output active. */
		else if (!WITHIN(portid, 0, ACTIVE_GET_NR_PORTS(active)))
			goto error;

		port = &portpool->ports[portid];

		/* Is the *port free? */
		if (!resource_is_used(&port->resource))
		{
			/* Initialize active. */
			port->flags = 0;
			resource_set_used(&port->resource);

			active->refcount++;
			portpool->used_ports++;

			ret = ACTIVE_LADDRESS_COMPOSE(actid, portid, ACTIVE_GET_NR_PORTS(active));
		}
		else
			ret = (-EBUSY);

error:
	spinlock_unlock(&active->lock);

	return (ret);
}

/*============================================================================*
 * active_release()                                                           *
 *============================================================================*/

/**
 * @brief Release a physical active port.
 *
 * @param pool   Active resource pool.
 * @param id     Compose address by active and port ID.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int active_release(const struct active_pool * pool, int id)
{
	int ret;                     /* Return value.        */
	int dest;                    /* Local address.       */
	struct port * port;          /* Port pointer.        */
	struct active * active;      /* Active pointer.      */
	struct port_pool * portpool; /* Port pool pointer.   */

	KASSERT(active_valid_call(pool, id));

	active = &pool->actives[ACTIVE_GET_LADDRESS_FD(pool->actives, id)];
	portpool = &active->portpool;
	port   = &portpool->ports[ACTIVE_GET_LADDRESS_PORT(active, id)];

	spinlock_lock(&active->lock);

		ret = (-EBUSY);

		if (resource_is_readable(&active->resource))
		{
			dest = ACTIVE_LADDRESS_COMPOSE(
				active->local,
				ACTIVE_GET_LADDRESS_PORT(active, id),
				ACTIVE_GET_NR_PORTS(active)
			);

			/* Check if exist pending messages for this port. */
			if (mbuffer_search(active->mbufferpool, dest, ACTIVE_ANY_SRC) >= 0)
				goto error;
		}

		ret = (-EINVAL);

		/* Is the port used? */
		if (resource_is_used(&port->resource))
		{
			/* Releases active. */
			resource_set_unused(&port->resource);
			active->refcount--;

			portpool->used_ports--;

			ret = 0;
		}

error:
	spinlock_unlock(&active->lock);

	return (ret);
}

/*============================================================================*
 * active_aread()                                                             *
 *============================================================================*/

/**
 * @brief Async reads from an active.
 *
 * @param pool   Active resource pool.
 * @param id     Compose address by active and port ID.
 * @param config Communication's configuration.
 * @param stats  Structure to store statstics.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC ssize_t active_aread(
	const struct active_pool * pool,
	int id,
	const struct active_config * config,
	struct pstats * stats
)
{
	ssize_t ret;                      /* Return value.                  */
	int remote;                       /* Remote node ID.                */
	int mbufferid;                    /* Mbuffer ID.                    */
	int is_local;                     /* Checks value for local comm.   */
	uint64_t t1;                      /* Clock value before aread call. */
	uint64_t t2;                      /* Clock value after aread call.  */
	struct port * port;               /* Port pointer.                  */
	struct mbuffer * buf;             /* Mbuffer pointer.               */
	struct active * active;           /* Active pointer.                */
	struct mbuffer_pool * bufferpool; /* Mbuffer pool pointer.          */

	KASSERT(active_valid_call(pool, id));

	active = &pool->actives[ACTIVE_GET_LADDRESS_FD(pool->actives, id)];
	port   = &active->portpool.ports[ACTIVE_GET_LADDRESS_PORT(active, id)];
	remote = ACTIVE_GET_LADDRESS_FD(active, config->remote_addr);

	/* Checks if it is a local receive. */
	is_local = node_is_local(remote);

	spinlock_lock(&active->lock);

		ret = (-EBADF);

		/* Bad active port. */
		if (!resource_is_used(&port->resource))
			goto error;

		if (is_local)
		{
			mbufferid = mbuffer_search(active->aux_bufferpool, config->local_addr, config->remote_addr);
			bufferpool = active->aux_bufferpool;
		}
		else
		{
			if ((mbufferid = mbuffer_search(active->mbufferpool, config->local_addr, config->remote_addr)) < 0)
			{
				mbufferid = mbuffer_search(active->aux_bufferpool, config->local_addr, config->remote_addr);
				bufferpool = active->aux_bufferpool;
			}
			else
				bufferpool = active->mbufferpool;
		}

		/* Is there a pending message for this vmailbox? */
		if (mbufferid >= 0)
		{

			buf = mbuffer_get(bufferpool, mbufferid);

			t1 = clock_read();
				active->do_copy(buf, config, ACTIVE_COPY_FROM_MBUFFER);
			t2 = clock_read();

			/* Update performance statistics. */
			stats->latency += (t2 - t1);
			stats->volume  += (config->size);

			KASSERT(mbuffer_release(bufferpool, mbufferid, MBUFFER_DISCARD_MESSAGE) == 0);

			spinlock_unlock(&active->lock);

			return (ACTIVE_COMM_RECEIVED);
		}

		ret = (-ENOMSG);

		/* Is it a local communication? */
		if (is_local)
			goto error;

		ret = (-EBUSY);

		/* Busy active. */
		if (resource_is_busy(&active->resource))
			goto error;

		/* Allow communication. */
		if ((ret = active->do_allow(active, remote)) < 0)
			goto error;

		active_set_allowed(active);
		active->remote = remote;

		/* Allocates a data buffer to receive data. */
		if ((mbufferid = mbuffer_alloc(active->mbufferpool)) < 0)
		{
			if ((mbufferid = mbuffer_alloc(active->aux_bufferpool)) < 0)
			{
				ret = mbufferid;
				goto error;
			}

			bufferpool = active->aux_bufferpool;
		}
		else
			bufferpool = active->mbufferpool;

		port->mbufferid   = mbufferid;
		port->mbufferpool = bufferpool;
		buf = mbuffer_get(bufferpool, mbufferid);

		t1 = clock_read();

			/* Setup asynchronous read. */
			if ((ret = active->do_aread(active->hwfd, (void *) &buf->message, active->size)) < 0)
				goto discard_message;

		t2 = clock_read();

		active->remote = -1;
		active_set_notallowed(active);

		/* Update performance statistics. */
		stats->latency += (t2 - t1);

		/* Sets the active as busy. */
		resource_set_busy(&active->resource);

	spinlock_unlock(&active->lock);

	return (ACTIVE_COMM_SUCCESS);

discard_message:
		KASSERT(mbuffer_release(bufferpool, mbufferid, MBUFFER_DISCARD_MESSAGE) == 0);
		port->mbufferid = -1;
		port->mbufferpool = NULL;

error:
	spinlock_unlock(&active->lock);

	return (ret);
}

/*============================================================================*
 * active_awrite()                                                            *
 *============================================================================*/

/**
 * @brief Async writes to an active.
 *
 * @param pool   Active resource pool.
 * @param id     Compose address by active and port ID.
 * @param config Communication's configuration.
 * @param stats  Structure to store statstics.
 *
 * @returns Upon successful completion, an status is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC ssize_t active_awrite(
	const struct active_pool * pool,
	int id,
	const struct active_config * config,
	struct pstats * stats
)
{
	ssize_t ret;                      /* Return value.                   */
	int forward;                      /* Check value for local comm.     */
	int port_nr;                      /* Port ID.                        */
	int mbufferid;                    /* Message buffer used to write.   */
	uint64_t t1;                      /* Clock value before awrite call. */
	uint64_t t2;                      /* Clock value after awrite call.  */
	struct port * port;               /* Port pointer.                   */
	struct mbuffer * buf;             /* Mbuffer pointer.                */
	struct active * active;           /* Active pointer.                 */
	struct mbuffer_pool * bufferpool; /* Mbuffer pool pointer.           */

	KASSERT(active_valid_call(pool, id));

	active  = &pool->actives[ACTIVE_GET_LADDRESS_FD(pool->actives, id)];
	port_nr = ACTIVE_GET_LADDRESS_PORT(active, id);
	port    = &active->portpool.ports[port_nr];

	/* Checks if the write is a forward or a send through NoC. */
	forward = node_is_local(active->remote);

	spinlock_lock(&active->lock);

		ret = (-EINVAL);

		/* Bad active port. */
		if (!resource_is_used(&port->resource))
			goto error;

		/* Checks if there is already a mbuffer allocated. */
		if ((mbufferid = port->mbufferid) < 0)
		{
			/* Allocates a message buffer to send the message. */
			if (forward)
			{
				mbufferid = mbuffer_alloc(active->aux_bufferpool);
				bufferpool = active->aux_bufferpool;
			}
			else
			{
				if ((mbufferid = mbuffer_alloc(active->mbufferpool)) < 0)
				{
					mbufferid = mbuffer_alloc(active->aux_bufferpool);
					bufferpool = active->aux_bufferpool;
				}
				else
					bufferpool = active->mbufferpool;
			}

			/* Successful allocated a message buffer? */
			if (mbufferid < 0)
			{
				ret = (mbufferid);
				goto error;
			}
			
			buf = mbuffer_get(bufferpool, mbufferid);

			active->do_header_config(buf, config);

			t1 = clock_read();
				active->do_copy(buf, config, ACTIVE_COPY_TO_MBUFFER);
			t2 = clock_read();

			/* Checks if the destination is the local node. */
			if (forward)
			{
				/* Forwards the message to the mbuffers table. */
				mbuffer_release(bufferpool, mbufferid, MBUFFER_KEEP_MESSAGE);

				/* Update performance statistics. */
				stats->latency += (t2 - t1);
				stats->volume  += (config->size);

				spinlock_unlock(&active->lock);

				return (ACTIVE_COMM_RECEIVED);
			}
			else
			{
				port->mbufferpool = bufferpool; 
				port->mbufferid   = mbufferid;
			}
		}
		else
			buf = mbuffer_get(port->mbufferpool, mbufferid);

		ret = (-EBUSY);

		/* Checks if the current port already requested an operation. */
		if (!port_is_requested(port))
		{
			/* Request an operation. */
			if (do_request_operation(active, port_nr) < 0)
				goto error;

			port_set_requested(port);
		}

		/* Check if it is the current comm's turn. */
		if (!do_request_verify(active, port_nr))
			goto error;

		/* Bad active. */
		if (resource_is_busy(&active->resource))
			goto error;

		t1 = clock_read();

			/* Setup asynchronous write. */
			if ((ret = active->do_awrite(active->hwfd, (void *) &buf->message, active->size)) < 0)
				goto error;

		t2 = clock_read();

		/* Update performance statistics. */
		stats->latency += (t2 - t1);

		/* Sets the active as busy. */
		resource_set_busy(&active->resource);

		ret = (ACTIVE_COMM_SUCCESS);

error:
	spinlock_unlock(&active->lock);

	return (ret);
}

/*============================================================================*
 * active_wait()                                                              *
 *============================================================================*/

/**
 * @brief Waits an operation on an active.
 *
 * @param pool   Active resource pool.
 * @param id     Compose address by active and port ID.
 * @param config Communication's configuration.
 * @param stats  Structure to store statstics.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
PUBLIC int active_wait(
	const struct active_pool * pool,
	int id,
	const struct active_config * config,
	struct pstats * stats
)
{
	int ret;                /* Return value.                   */
	int dest;               /* Msg destination address.        */
	int mbufferid;          /* Allocated mbufferid.            */
	int keep_rule;          /* Discard rule.                   */
	uint64_t t1;            /* Clock value before awrite call. */
	uint64_t t2;            /* Clock value after awrite call.  */
	int port_nr;            /* Port ID.                        */
	struct port * port;     /* Port pointer.                   */
	struct mbuffer * buf;   /* Mbuffer pointer.                */
	struct active * active; /* Active pointer.                 */

	KASSERT(active_valid_call(pool, id));

	active = &pool->actives[ACTIVE_GET_LADDRESS_FD(pool->actives, id)];

	port_nr = ACTIVE_GET_LADDRESS_PORT(active, id);
	port   = &active->portpool.ports[port_nr];

	spinlock_lock(&active->lock);

		ret = (-EBADF);

		/* Bad active. */
		if (!resource_is_used(&port->resource))
			goto error;

		/* Bad active. */
		if (!resource_is_busy(&active->resource))
			goto error;

		/* Invalid mbufferid. */
		if ((mbufferid = port->mbufferid) < 0)
			goto error;

		/* Not requested write. */
		if ((resource_is_writable(&active->resource)) && !(do_request_verify(active, port_nr)))
			goto error;

	spinlock_unlock(&active->lock);

	t1 = clock_read();

		/* Wait for asynchronous write to finish. */
		ret = active->do_wait(active->hwfd);

	t2 = clock_read();

	spinlock_lock(&active->lock);

		keep_rule = MBUFFER_DISCARD_MESSAGE;

		/* Was there no error? */
		if (ret == 0)
		{
			ret = (ACTIVE_COMM_SUCCESS);

			/* Read communication has extra operations. */
			if (resource_is_readable(&active->resource))
			{
				buf = mbuffer_get(port->mbufferpool, mbufferid);

				dest  = buf->message.header.dest;

				/* Checks if the message is addressed for the requesting port. */
				/* Consumes the message. */
				if (active->do_header_check(buf, config))
					active->do_copy(buf, config, ACTIVE_COPY_FROM_MBUFFER);

				/* Ignore the message. */
				else
				{
					/* Keep message? true (MBUFFER_KEEP_MESSAGE) : false (MBUFFER_DISCARD_MESSAGE) */
					keep_rule = resource_is_used(
						&active->portpool.ports[ACTIVE_GET_LADDRESS_PORT(active, dest)].resource
					);

					/* Returns sinalizing that a message was read, but not for local port. */
					ret = (ACTIVE_COMM_AGAIN);
				}
			}
			else if (resource_is_writable(&active->resource))
			{
				KASSERT(do_request_complete(active, port_nr) == 0);

				port_set_notrequested(port);
			}

			if (ret == ACTIVE_COMM_SUCCESS)
				stats->volume += (config->size);

			/* Update performance statistics. */
			stats->latency += (t2 - t1);
		}

		/* Releases mbuffer. */
		mbuffer_release(port->mbufferpool, mbufferid, keep_rule);
		port->mbufferid   = -1;
		port->mbufferpool = NULL;

		resource_set_notbusy(&active->resource);

error:
	spinlock_unlock(&active->lock);

	return (ret);
}

/*============================================================================*
 * _active_alloc()                                                            *
 *============================================================================*/

/**
 * @brief Allocate a active resource.
 *
 * @param pool Active resource pool.
 *
 * @returns Upon successful completion, active pointer is returned. Upon
 * failure, NULL pointer is returned.
 */
PRIVATE struct active * _active_alloc(const struct active_pool * pool)
{
	struct active * active;

	/* Search for a free synchronization point. */
	for (int i = 0; i < pool->nactives; i++)
	{
		active = &pool->actives[i];

		/* Found. */
		if (!resource_is_used(&active->resource))
		{
			active->resource = RESOURCE_INITIALIZER;
			resource_set_used(&active->resource);

			return (active);
		}
	}

	return (NULL);
}

/*============================================================================*
 * _active_free()                                                             *
 *============================================================================*/

/**
 * @brief Release a active resource.
 *
 * @param pool Active resource pool.
 * @param id   Active ID.
 */
PRIVATE void _active_free(const struct active_pool * pool, int id)
{
	resource_set_unused(&pool->actives[id].resource);
}

/*============================================================================*
 * _active_create()                                                           *
 *============================================================================*/

/**
 * @brief Creates a hardware active.
 *
 * @param pool  Active resource pool.
 * @param local Logic ID of the target local node.
 *
 * @returns Upon successful completion, the ID of the newly created
 * hardware active is returned. Upon failure, a negative error code
 * is returned instead.
 */
PUBLIC int _active_create(const struct active_pool * pool, int local)
{
	int hwfd;  /* File descriptor. */
	int actid;    /* active ID.      */
	struct active * active;

	KASSERT(pool != NULL);

	/* Search target hardware active. */
	if ((actid = active_search(pool, local, -1, ACTIVE_TYPE_INPUT)) >= 0)
		return (actid);

	/* Allocate resource. */
	if ((active = _active_alloc(pool)) == NULL)
		return (-EAGAIN);

	/* Create underlying input hardware active. */
	if ((hwfd = active->do_create(local)) < 0)
	{
		_active_free(pool, (active - pool->actives));
		return (hwfd);
	}

	/* Initialize hardware active. */
	active->flags    = 0;
	active->hwfd     = hwfd;
	active->local    = local;
	active->remote   = -1;
	active->refcount = 0;
	resource_set_rdonly(&active->resource);
	resource_set_notbusy(&active->resource);

	return (active - pool->actives);
}

/*============================================================================*
 * _active_open()                                                             *
 *============================================================================*/

/**
 * @brief Opens a hardware active.
 *
 * @param pool   Active resource pool.
 * @param local  Logic ID of the target local node.
 * @param remote Logic ID of the target remote node.
 *
 * @returns Upon successful completion, the ID of the newly opened
 * hardware active is returned. Upon failure, a negative error code
 * is returned instead.
 */
PUBLIC int _active_open(const struct active_pool * pool, int local, int remote)
{
	int hwfd;               /* Hardware ID.    */
	int actid;              /* Active ID.      */
	struct active * active; /* Active Pointer. */

	KASSERT(pool != NULL);

	/* Search target hardware active. */
	if ((actid = active_search(pool, local, remote, ACTIVE_TYPE_OUTPUT)) >= 0)
		return (actid);

	/* Allocate resource. */
	if ((active = _active_alloc(pool)) == NULL)
		return (-EAGAIN);

	hwfd = -EINVAL;

	if (node_is_local(local) && !node_is_local(remote))
	{
		/* Open underlying output hardware active. */
		if ((hwfd = active->do_open(local, remote)) < 0)
		{
			_active_free(pool, (active - pool->actives));
			return (hwfd);
		}
	}
	else if (!node_is_local(local))
		return (-EINVAL);

	/* Initialize hardware active. */
	active->flags    = 0;
	active->hwfd     = hwfd;
	active->refcount = 0;
	active->local    = local;
	active->remote   = remote;
	resource_set_wronly(&active->resource);
	resource_set_notbusy(&active->resource);

	return (active - pool->actives);
}
