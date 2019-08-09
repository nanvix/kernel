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

#ifndef DEV_NET_RTL8139_H_
#define DEV_NET_RTL8139_H_

	#ifndef __NEED_RTL8139
		#error "do not include this file"
	#endif

	#include <dev/net/net.h>
	#include <stdint.h>

	#define RTL8139_VENDOR_ID 0x10EC
	#define RTL8139_DEVICE_ID 0x8139

	/* Minimum possible size for the rtl8139 cards */
	#define RTL8139_RX_BUF_SIZE 8192 + 16

	/* Adding 1500 bytes when allocating to prevent overflows */
	#define RTL8139_RX_BUF_ALLOC_SIZE RTL8139_RX_BUF_SIZE + 1500

	#define RTL8139_RX_READ_POINTER_MASK (~3)
	#define RTL8139_ROK (1 << 0)
	#define RTL8139_TOK (1 << 2)

	/**
	 * @name Register Offsets
	 */
	/**@{*/
	#define RTL8139_RX_BUFFER        0x30
	#define RTL8139_COMMAND          0x37
	#define RTL8139_CAPR             0x38
	#define RTL8139_TX_CONFIG        0x40
	#define RTL8139_RX_CONFIG        0x44
	#define RTL8139_CONFIG1          0x52
	#define RTL8129_INTERRUPT_MASK   0x3c
	#define RTL8129_INTERRUPT_STATUS 0x3e
	/**@}*/

	/**
	 * @brief Packet array length.
	 */
	#define PACKET_ARRAY_LENGTH 16

	/**
	 * @brief RTL 8139 device information.
	 */
	struct rtl8139_dev
	{
		uint16_t io_base;
		uint8_t mac_addr[6];
		uint8_t rx_buffer[RTL8139_RX_BUF_ALLOC_SIZE];
		uint8_t tx_cur;
		uint32_t rx_cur;
		bool lwip_forwarding;
	};

	/**
	 * @brief Packet header.
	 */
	struct rtl8139_packet_header
	{
		uint16_t status;
		uint16_t size;
	};

	/**
	 * @brief Packet array.
	 */
	struct rtl8139_packet_array
	{
		struct packet packets[PACKET_ARRAY_LENGTH];
		uint8_t read_pos;
		uint8_t write_pos;
	};

#endif /* !DEV_NET_RTL8139_H_ */
