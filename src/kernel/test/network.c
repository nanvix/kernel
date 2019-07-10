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
 * Network Unit Tests                                                 *
 *============================================================================*/

/**
 * @brief API Test: Page Frame Address Translation
 */
PRIVATE void test_receive_packet()
{
	uint8_t packet[43] =  
	{
	0x52, 0x55, 0x00, 0xd1, 0x55, 0x01, 0x52, 0x55, 0x00, 0xd1, 0x55, 0x01, 0x08, 0x00, 0x45, 0x00,
   	0x00, 0x1d, 0x00, 0x00, 0x40, 0x00, 0x40, 0x01, 0x34, 0x4e, 0xc0, 0xa8, 0x42, 0x42, 0xc0, 0xa8,
	0x42, 0xff, 0x08, 0x00, 0xf1, 0x19, 0x06, 0xe4, 0x00, 0x02, 0x00
	};
	
	struct rtl8139_dev* rtl8139_device = dev_net_rtl8139_get_device();

	for(int i = 0; i < 10; i ++) {
		uint32_t old_ptr_offset = dev_net_rtl8139_get_packet_ptr();
		
		dev_net_rtl8139_send_packet(packet, 43);

		uint8_t* rcv_packet = (uint8_t *) (rtl8139_device->rx_buffer +  old_ptr_offset + 4);
		for(int j = 0; j < 43; j ++) {
			KASSERT(packet[j] == rcv_packet[j]);
		}
	}
}

/**
 * @brief Page Frame unit tests.
 *
 * Unit tests for the Page Frame Allocator subsystem.
 */
PRIVATE struct
{
	void (*test_fn)(); /**< Test function.     */
	const char *type;      /**< Name of test type. */
	const char *name;      /**< Test Name.         */
} network_tests[] = {
	{ test_receive_packet,           "network",    "reveiving packets"         },
	{ NULL,                                 NULL,     NULL                                },
};

/**
 * @brief Runs unit tests on the Page Frame Allocator.
 *
 * The frame_test_driver() function runs API, fault injection and
 * stress tests on the Page Frame Allocator.
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