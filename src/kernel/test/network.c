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

/**
 * @cond release_test
 */

#include <nanvix/const.h>
#include <dev/net/rtl8139.h>
#include <nanvix/klib.h>
#include <errno.h>

/*============================================================================*
 * Network Unit Tests                                                 		  *
 *============================================================================*/

/**
 * @brief Testing the sending and receiving of multiple simple packets
 */
PRIVATE void test_receive_packet_simple()
{
	uint8_t packet[43] =
	{
	0x52, 0x55, 0x00, 0xd1, 0x55, 0x01, 0x52, 0x55,
	0x00, 0xd1, 0x55, 0x01, 0x08, 0x00, 0x45, 0x00,
	0x00, 0x1d, 0x00, 0x00, 0x40, 0x00, 0x40, 0x01,
	0x34, 0x4e, 0xc0, 0xa8, 0x42, 0x42, 0xc0, 0xa8,
	0x42, 0xff, 0x08, 0x00, 0xf1, 0x19, 0x06, 0xe4,
	0x00, 0x02, 0x00
	};

	struct rtl8139_dev* rtl8139_device = dev_net_rtl8139_get_device();

	for(int i = 0; i < 10; i ++) {
		uint32_t old_ptr_offset = rtl8139_device->rx_cur;
		dev_net_rtl8139_send_packet(packet, 43);

		uint16_t rcv_packet_status = (uint16_t) *(rtl8139_device->rx_buffer +  old_ptr_offset);
		/* If the received packet is valid */
		if (dev_net_rtl8139_packet_status_valid(rcv_packet_status)) {
			uint8_t* rcv_packet_data = (uint8_t *) (rtl8139_device->rx_buffer +  old_ptr_offset + 4);
			/* Compare it with the sent packet */
			for(int j = 0; j < 43; j ++) {
				KASSERT(packet[j] == rcv_packet_data[j]);
			}
		}
	}
}

/**
 * @brief Testing the sending and receiving of many packets to test
 * the buffer ring wrap
 */
PRIVATE void test_receive_packet_rx_overflow()
{
	uint8_t packet[142] =
	{
	0x52, 0x55, 0x00, 0xd1, 0x55, 0x01, 0x52, 0x55, 0x00, 0xd1, 0x55, 0x01,
	0x08, 0x00, 0x45, 0x00,	0x00, 0x80, 0x00, 0x00, 0x40, 0x00, 0x40, 0x01,
	0x33, 0xeb, 0xc0, 0xa8, 0x42, 0x42, 0xc0, 0xa8,	0x42, 0xff, 0x08, 0x00,
	0x8f, 0x32, 0x60, 0x78, 0x00, 0x03, 0xc5, 0x0e, 0x26, 0x5d, 0x00, 0x00,
	0x00, 0x00, 0xab, 0x58, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11,
	0x12, 0x13, 0x14, 0x15,	0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
	0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,	0x26, 0x27, 0x28, 0x29,
	0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
	0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41,
	0x42, 0x43, 0x44, 0x45,	0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d,
	0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55,	0x56, 0x57, 0x58, 0x59,
	0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63
	};
	struct rtl8139_dev* rtl8139_device = dev_net_rtl8139_get_device();

	for(int i = 0; i < 100; i ++) {
		uint32_t old_ptr_offset = rtl8139_device->rx_cur;

		dev_net_rtl8139_send_packet(packet, 142);

		uint16_t rcv_packet_status = (uint16_t) *(rtl8139_device->rx_buffer +  old_ptr_offset);
		if (dev_net_rtl8139_packet_status_valid(rcv_packet_status)) {
			uint8_t* rcv_packet_data = (uint8_t *) (rtl8139_device->rx_buffer +  old_ptr_offset + 4);
			for(int j = 0; j < 142; j ++) {
				KASSERT(packet[j] == rcv_packet_data[j]);
			}
		}
	}
}


/**
 * @brief Testing the sending and receiving of packets with size variation to
 * make sure that the receive buffer pointer is updated correctly
 */
PRIVATE void test_receive_packet_multiple_sizes()
{
	uint8_t packet[142] =
	{
	0x52, 0x55, 0x00, 0xd1, 0x55, 0x01, 0x52, 0x55, 0x00, 0xd1, 0x55, 0x01,
	0x08, 0x00, 0x45, 0x00,	0x00, 0x80, 0x00, 0x00, 0x40, 0x00, 0x40, 0x01,
	0x33, 0xeb, 0xc0, 0xa8, 0x42, 0x42, 0xc0, 0xa8,	0x42, 0xff, 0x08, 0x00,
	0x8f, 0x32, 0x60, 0x78, 0x00, 0x03, 0xc5, 0x0e, 0x26, 0x5d, 0x00, 0x00,
	0x00, 0x00, 0xab, 0x58, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11,
	0x12, 0x13, 0x14, 0x15,	0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
	0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,	0x26, 0x27, 0x28, 0x29,
	0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
	0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41,
	0x42, 0x43, 0x44, 0x45,	0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d,
	0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55,	0x56, 0x57, 0x58, 0x59,
	0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63
	};
	struct rtl8139_dev* rtl8139_device = dev_net_rtl8139_get_device();

	for(int i = 43; i < 142; i ++) {
		uint32_t old_ptr_offset = rtl8139_device->rx_cur;

		dev_net_rtl8139_send_packet(packet, i);

		uint16_t rcv_packet_status = (uint16_t) *(rtl8139_device->rx_buffer +  old_ptr_offset);
		if (dev_net_rtl8139_packet_status_valid(rcv_packet_status)) {
			uint8_t* rcv_packet_data = (uint8_t *) (rtl8139_device->rx_buffer +  old_ptr_offset + 4);
			for(int j = 0; j < i; j ++) {
				KASSERT(packet[j] == rcv_packet_data[j]);
			}
		}
	}
}

/**
 * @brief Network unit tests.
 *
 * Unit tests for rtl8139 driver
 */
PRIVATE struct
{
	void (*test_fn)(); /**< Test function.     */
	const char *type;  /**< Name of test type. */
	const char *name;  /**< Test Name.         */
} network_tests[] = {
	{ test_receive_packet_simple,         "network",    "reveiving packets simple"        },
	{ test_receive_packet_rx_overflow,    "network",    "reveiving packets overflow"      },
	{ test_receive_packet_multiple_sizes, "network",    "reveiving packets multiple sizes"},
	{ NULL,                                    NULL,    NULL                              }
};

/**
 * @brief Runs netwotk unit tests.
 *
 * The network_test_driver() function runs send/receive packets tests for the
 * rtl8139 network interface card.
 *
 */
PUBLIC void network_test_driver()
{
	struct rtl8139_dev* rtl8139_device = dev_net_rtl8139_get_device();
    /* Only receive packets send by the card itself */
	output32(rtl8139_device->io_base + RX_CONFIG, 0x2 | (1 << 7));
    /* Enable loopback mode */
	output32(rtl8139_device->io_base + TX_CONFIG, (0x3 << 17));

	for (int i = 0; network_tests[i].test_fn != NULL; i++)
	{
		network_tests[i].test_fn();
		kprintf("[test][%s] %s [passed]", network_tests[i].type, network_tests[i].name);
	}

    /* Receive all packets */
	output32(rtl8139_device->io_base + RX_CONFIG, 0xf | (1 << 7));
    /* Disable loopback mode */
	output32(rtl8139_device->io_base + TX_CONFIG, 0);
}
