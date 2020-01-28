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

#ifndef DEV_NET_NET_H_
#define DEV_NET_NET_H_

	#include <nanvix/kernel/config.h>
	#include <nanvix/const.h>
	#include <posix/stdint.h>

#if __NANVIX_HAS_NETWORK

	#include <lwip/netif.h>

	/**
	 * @name Default Network Configuration
	 */
	/**@{*/
	#define NETWORK_DEFAULT_IPADDR  "192.168.66.67" /**< IP Address   */
	#define NETWORK_DEFAULT_NETMASK "255.255.255.0" /**< Network Mask */
	#define NETWORK_DEFAULT_GATEWAY "192.168.66.66" /**< Gateway      */
	/**@}*/

	/**
	 * @brief Network packet.
	 */
	struct packet
	{
		uint16_t len;  /**< Size */
		uint8_t *data; /**< Data */
	};

	/**
	 * @brief Tests the network driver.
	 */
	EXTERN void network_test_driver(void);

	/**
	 * @brief Initializes the network interface card.
	 */
	EXTERN void network_init(struct netif* netif);

	/**
	 * @brief Sends a network packet.
	 */
	EXTERN void network_send_packet(struct packet packet);

	/**
	 * @brief Pools for a new packet.
	 *
	 * @return One if a new packets was received, and zero otherwise.
	 */
	EXTERN int network_get_new_packet(struct packet* packet);

	/**
	 * @brief Asserts if a new packet was received.
	 *
	 * @return One if a new packets was received, and zero otherwise.
	 */
	EXTERN int network_is_new_packet(void);

	/**
	 * @brief Asserts if loopback mode is supported.
	 */
	EXTERN int network_loopback_supported(void);

	/**
	 * @brief Enables loopback mode.
	 */
	EXTERN void network_loopback_enable(void);

	/**
	 * @brief Disables loopback mode.
	 */
	EXTERN void network_loopback_disable(void);

	/**
	 * @brief Gets the MAC address of the network interface card.
	 */
	EXTERN void network_get_mac_adress(uint8_t mac[6]);

#endif /* __NANVIX_HAS_NETWORK */

	/**
	 * @brief Initializes the network driver.
	 */
	EXTERN void network_setup(void);

#endif /* DEV_NET_NET_H_ */
