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

#if __NANVIX_HAS_NETWORK

/**
 * @name Destructive Tests for Networking
 */
/**@{*/
#define TEST_NETWORK_RECEIVE 0 /**< Receive Packtets Only */
#define TEST_NETWORK_SEND    0 /**< Send Packets Only     */
/**@}*/

/**
 * Assert that packet a and b have the same lenght and content
 */
PRIVATE void assert_packet_equals(struct packet a, struct packet b)
{
	int min;
	KASSERT(a.len == b.len);

	min = (a.len < b.len) ? a.len : b.len;

	for (int i = 0; i < min; i++)
		KASSERT(a.data[i] == b.data[i]);
}

/**
 * Change the src and dest mac adresses of data to mac :
 * Replace the 12 first bytes of data with the 6 bytes of mac (2 times).
 */
PRIVATE void add_src_dest_mac_to_packet(uint8_t *data, uint8_t mac[6])
{
	for (int i = 0; i < 6; i++)
	{
		data[i] = mac[i];
		data[i + 6] = mac[i];
	}
}

/*============================================================================*
 * API Unit Tests                                                             *
 *============================================================================*/

/**
 * @brief Small data frame.
 */
PRIVATE uint8_t df_small[72] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x08, 0x00, 0x45, 0x00,	0x00, 0x80, 0x00, 0x00, 0x40, 0x00, 0x40, 0x01,
	0x33, 0xeb, 0xc0, 0xa8, 0x42, 0x42, 0xc0, 0xa8,	0x42, 0xff, 0x08, 0x00,
	0x8f, 0x32, 0x60, 0x78, 0x00, 0x03, 0xc5, 0x0e, 0x26, 0x5d, 0x00, 0x00,
	0x00, 0x00, 0xab, 0x58, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11,
	0x12, 0x13, 0x14, 0x15,	0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
};

/**
 * @brief Large data frame.
 */
PRIVATE uint8_t df_large[142] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
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

/**
 * @brief API Test: send and receive multiple packets of same size.
 */
PRIVATE void test_api_network_loopback_receive_single_size(void)
{
	uint8_t mac[6];
	struct packet send_packet;
	struct packet recv_packet;

	/* Test not applicable. */
	if (!network_loopback_supported())
		return;

	network_get_mac_adress(mac);
	add_src_dest_mac_to_packet(df_small, mac);
	send_packet.len = 72;
	send_packet.data = df_small;

	network_loopback_enable();

	for (int i = 0; i < 10; i++)
	{
		network_send_packet(send_packet);
		KASSERT(network_get_new_packet(&recv_packet));
		assert_packet_equals(recv_packet, send_packet);
	}

	network_loopback_disable();
}

/**
 * @brief Stress Test: ring buffer overflow.
 */
PRIVATE void test_stress_network_loopback_overflow(void)
{
	uint8_t mac[6];
	struct packet send_packet;
	struct packet recv_packet;

	/* Test not applicable. */
	if (!network_loopback_supported())
		return;

	network_get_mac_adress(mac);
	add_src_dest_mac_to_packet(df_large, mac);
	send_packet.len = 142;
	send_packet.data = df_large;

	network_loopback_enable();

	for (int i = 0; i < 1000; i ++)
	{
		network_send_packet(send_packet);
		KASSERT(network_get_new_packet(&recv_packet));
		assert_packet_equals(recv_packet, send_packet);
	}

	network_loopback_disable();
}

/**
 * @brief API Test: send and receive multiple packets of different sizes.
 */
PRIVATE void test_api_network_loopback_receive_multiple_sizes(void)
{
	uint8_t mac[6];
	struct packet send_packet;
	struct packet recv_packet;

	/* Test not applicable. */
	if (!network_loopback_supported())
		return;

	network_get_mac_adress(mac);
	add_src_dest_mac_to_packet(df_large, mac);
	send_packet.data = df_large;

	network_loopback_enable();

	for (int i = 64; i < 142; i++)
	{
		send_packet.len = i;
		network_send_packet(send_packet);
		KASSERT(network_get_new_packet(&recv_packet));
		assert_packet_equals(recv_packet, send_packet);
	}

	network_loopback_disable();
}

#if TEST_NETWORK_SEND

/**
 * @brief API Test: sends packets in an infinite loop.
 */
PRIVATE void test_api_network_send(void)
{
	uint8_t mac[6];
	struct packet send_packet;
	struct packet recv_packet;

	network_get_mac_adress(mac);
	add_src_dest_mac_to_packet(df_small, mac);
	send_packet.len = 72;
	send_packet.data = df_small;

	while (1)
		network_send_packet(send_packet);
}

#endif

#if TEST_NETWORK_RECEIVE

/**
 * @brief API Test: receives packets in an infinite loop.
 */
PRIVATE void test_api_network_receive(void)
{
	uint8_t mac[6];
	struct packet send_packet;
	struct packet recv_packet;

	network_get_mac_adress(mac);
	add_src_dest_mac_to_packet(df_small, mac);
	send_packet.len = 72;
	send_packet.data = df_small;

	while (1)
		KASSERT(network_get_new_packet(&recv_packet));
}

#endif

/**
 * @brief Network unit tests.
 */
PRIVATE struct
{
	void (*test_fn)(); /**< Test function.     */
	const char *type;  /**< Name of test type. */
	const char *name;  /**< Test Name.         */
} network_tests[] = {
	{ test_api_network_loopback_receive_single_size,    "api",    "receiving packets single size"    },
	{ test_api_network_loopback_receive_multiple_sizes, "api",    "receiving packets multiple sizes" },
	{ test_stress_network_loopback_overflow,            "stress", "receiving packets overflow"       },
#if TEST_NETWORK_SEND
	{ test_api_network_send,                            "api",    "send packets"                     },
#endif
#if TEST_NETWORK_RECEIVE
	{ test_api_network_receive,                         "api",    "receive packets"                  },
#endif
	{ NULL,                                              NULL,     NULL                              }
};

/**
 * @brief Kernel-level network test driver.
 */
PUBLIC void network_test_driver(void)
{
	for (int i = 0; network_tests[i].test_fn != NULL; i++)
	{
		network_tests[i].test_fn();
		kprintf("[test][%s] %s [passed]", network_tests[i].type, network_tests[i].name);
	}
}

#endif
