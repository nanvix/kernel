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

#ifndef NIC_DRIVER_IF_H_
#define NIC_DRIVER_IF_H_

	#include <nanvix/kernel/config.h>

#if __NANVIX_HAS_NETWORK

	/* Define those to better describe your network interface. */
	#define IFNAME0 'n'
	#define IFNAME1 'i'

	#include <lwip/err.h>
	#include <lwip/netif.h>

	/**
	 * @brief Forwards a packet to LwIP.
	 */
	extern void nic_driver_if_input(struct netif *netif);

	/**
	 * @brief Initializes forwarding mode in LwIP.
	 */
	extern err_t nic_driver_if_init(struct netif *netif);

#endif /* __NANVIX_HAS_NETWORK */

#endif /* NIC_DRIVER_IF_H_ */
