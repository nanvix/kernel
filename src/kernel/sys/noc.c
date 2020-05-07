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
#include <nanvix/kernel/noc.h>
#include <posix/errno.h>

/*============================================================================*
 * kernel_node_get_num()                                                      *
 *============================================================================*/

/**
 * @brief The kernel_node_get_num() function gets the NoC Node Number attached
 * with the core.
 *
 * @return Positive number with successfully get the node number, negative
 * otherwise.
 *
 * @author João Vicente Souto
 */
PUBLIC int kernel_node_get_num(void)
{
#if (PROCESSOR_HAS_NOC)
    return (processor_node_get_num());
#else
	return (0);
#endif /* PROCESSOR_HAS_NOC */
}

/*============================================================================*
 * kernel_cluster_get_num()                                                   *
 *============================================================================*/

/**
 * @brief Returns the logical ID of the underlying cluster.
 *
 * @return Upon successful completion the logical ID of the underlying
 * cluster is returned. Upon failure, a negative error code is returned
 * instead..
 *
 * @author Pedro Henrique Penna
 */
PUBLIC int kernel_cluster_get_num(void)
{
#if (PROCESSOR_IS_MULTICLUSTER)
    return (cluster_get_num());
#else
	return (-ENOSYS);
#endif /* PROCESSOR_IS_MULTICLUSTER */
}

/*============================================================================*
 * kernel_comm_get_port()                                                     *
 *============================================================================*/

/**
 * @brief Returns the virtual communicator logic port.
 *
 * @param id   Virtual communicator id.
 * @param type Type of the communicator (MAILBOX ? PORTAL)
 *
 * @return Upon successful completion the logical port ID of the underlying
 * communicator is returned. Upon failure, a negative error code is returned
 * instead.
 *
 * @author João Fellipe Uller
 */
PUBLIC int kernel_comm_get_port(int id, int type)
{
#if (__TARGET_HAS_MAILBOX)
	if (type == COMM_TYPE_MAILBOX)
		return (do_vmailbox_get_port(id));
#endif /* TARGET_HAS_MAILBOX */

#if (__TARGET_HAS_PORTAL)
	if (type == COMM_TYPE_PORTAL)
		return (do_vportal_get_port(id));
#endif /* TARGET_HAS_PORTAL */

#if (!__TARGET_HAS_PORTAL && !__TARGET_HAS_MAILBOX)
	UNUSED(id);
	UNUSED(type);
#endif /* (!__TARGET_HAS_PORTAL && !__TARGET_HAS_MAILBOX) */

	return (-ENOSYS);
}
