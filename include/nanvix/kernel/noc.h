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

#ifndef NANVIX_NOC_H_
#define NANVIX_NOC_H_

	#include <nanvix/hal.h>
	#include <nanvix/kernel/portal.h>
	#include <nanvix/kernel/sync.h>
	#include <nanvix/kernel/mailbox.h>

	/**
	 * @name Comm_type constants.
	 */
	/**@{*/
	#define COMM_TYPE_MAILBOX (0)
	#define COMM_TYPE_PORTAL  (1)
	#define COMM_TYPE_SYNC    (2)
	/**@}*/

	/**
	 * @name Sanity checks
	 */
	/**@{*/
	#if (KMAILBOX_IOCTL_GET_VOLUME != KPORTAL_IOCTL_GET_VOLUME)
	#error "The identifiers of the IOCTL commands must be the same!"
	#endif
	#if (KMAILBOX_IOCTL_GET_LATENCY != KPORTAL_IOCTL_GET_LATENCY)
	#error "The identifiers of the IOCTL commands must be the same!"
	#endif
	#if (KMAILBOX_IOCTL_GET_NCREATES != KPORTAL_IOCTL_GET_NCREATES)
	#error "The identifiers of the IOCTL commands must be the same!"
	#endif
	#if (KMAILBOX_IOCTL_GET_NUNLINKS != KPORTAL_IOCTL_GET_NUNLINKS)
	#error "The identifiers of the IOCTL commands must be the same!"
	#endif
	#if (KMAILBOX_IOCTL_GET_NOPENS != KPORTAL_IOCTL_GET_NOPENS)
	#error "The identifiers of the IOCTL commands must be the same!"
	#endif
	#if (KMAILBOX_IOCTL_GET_NCLOSES != KPORTAL_IOCTL_GET_NCLOSES)
	#error "The identifiers of the IOCTL commands must be the same!"
	#endif
	#if (KMAILBOX_IOCTL_GET_NREADS != KPORTAL_IOCTL_GET_NREADS)
	#error "The identifiers of the IOCTL commands must be the same!"
	#endif
	#if (KMAILBOX_IOCTL_GET_NWRITES != KPORTAL_IOCTL_GET_NWRITES)
	#error "The identifiers of the IOCTL commands must be the same!"
	#endif
	#if (KMAILBOX_IOCTL_GET_LATENCY != KSYNC_IOCTL_GET_LATENCY)
	#error "The identifiers of the IOCTL commands must be the same!"
	#endif
	#if (KMAILBOX_IOCTL_GET_NCREATES != KSYNC_IOCTL_GET_NCREATES)
	#error "The identifiers of the IOCTL commands must be the same!"
	#endif
	#if (KMAILBOX_IOCTL_GET_NUNLINKS != KSYNC_IOCTL_GET_NUNLINKS)
	#error "The identifiers of the IOCTL commands must be the same!"
	#endif
	#if (KMAILBOX_IOCTL_GET_NOPENS != KSYNC_IOCTL_GET_NOPENS)
	#error "The identifiers of the IOCTL commands must be the same!"
	#endif
	#if (KMAILBOX_IOCTL_GET_NCLOSES != KSYNC_IOCTL_GET_NCLOSES)
	#error "The identifiers of the IOCTL commands must be the same!"
	#endif
	/**@}*/

	/**
	 * @name NoC I/O Control types.
	 */
	/**@{*/
	#define COMM_IOCTL_GET_VOLUME    (1) /**< Gets communication volume.  */
	#define COMM_IOCTL_GET_LATENCY   (2) /**< Gets communication latency. */
	#define COMM_IOCTL_GET_NCREATES  (3) /**< Gets number of creates.     */
	#define COMM_IOCTL_GET_NUNLINKS  (4) /**< Gets number of unlinks.     */
	#define COMM_IOCTL_GET_NOPENS    (5) /**< Gets number of opens.       */
	#define COMM_IOCTL_GET_NCLOSES   (6) /**< Gets number of closes.      */
	#define COMM_IOCTL_GET_NREADS    (7) /**< Gets number of reads.       */
	#define COMM_IOCTL_GET_NWRITES   (8) /**< Gets number of writes.      */
	#define COMM_IOCTL_GET_NWAITS    (9) /**< Gets number of waits.       */
	#define COMM_IOCTL_GET_NSIGNALS (10) /**< Gets number of signals.     */
	/**@}*/

#ifdef __NANVIX_MICROKERNEL

	/**
	 * @brief Initializes the NoC system.
	 */
	EXTERN void noc_init(void);

#endif

#endif /** NANVIX_NOC_H_ */
