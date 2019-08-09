/*
 * MIT License
 *
 * Copyright(c) 2011-2019 The Maintainers of Nanvix
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

#include <dev/net/net.h>
#include <nanvix/const.h>
#include <nanvix/klib.h>

#if __NANVIX_HAS_NETWORK

#include <arch/nic_driver_if.h>
#include <lwip/init.h>
#include <lwip/netif.h>

/**
 * @brinef Network interface
 */
PUBLIC struct netif netif;

/**
 * The network_setup() function initializes the network stack. It
 * initializes the underlying network interface and assigns an IP
 * address to the target.
 */
PUBLIC void network_setup(void)
{
	ip_addr_t ip;
	ip_addr_t netmask;
	ip_addr_t gateway;

	lwip_init();

	/* Setup interface. */
	ip4addr_aton(NETWORK_DEFAULT_IPADDR, &ip);
	ip4addr_aton(NETWORK_DEFAULT_NETMASK, &netmask);
	ip4addr_aton(NETWORK_DEFAULT_GATEWAY, &gateway);
	netif_add(&netif, &ip, &netmask, &gateway, NULL, nic_driver_if_init, netif_input);

	/* Bring the interface up. */
	netif_set_default(&netif);
	netif_set_up(&netif);

	network_test_driver();
}

#endif
