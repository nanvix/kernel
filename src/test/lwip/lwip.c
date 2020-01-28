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

#include <dev/net/net.h>
#include <arch/nic_driver_if.h>
#include <lwip/timeouts.h>
#include <lwip/udp.h>
#include <lwip/tcp.h>
#include <lwip/etharp.h>
#include <stdint.h>

struct netif* netif;

/*============================================================================*
 * Utility Functions                                                		  *
 *============================================================================*/

/**
 * @brief Fills up a packet buffer.
 *
 * @param p    Target packet buffer.
 * @param data Data.
 *
 * @author Atoine Saget
 */
static void fill_pbuf(struct pbuf* p, uint8_t* data)
{
	struct pbuf *q;
	uint32_t packet_index = 0;

	KASSERT(p != NULL);
	KASSERT(data != NULL);

	#if ETH_PAD_SIZE
	pbuf_remove_header(p, ETH_PAD_SIZE); /* drop the padding word */
	#endif

	/*
	 * We iterate over the pbuf chain until
	 * we have read the entire packet into
	 * the pbuf.
	 */
	for (q = p; q != NULL; q = q->next)
	{
		for (int i = 0; i < q -> len; i++)
			((u8_t *)q->payload)[i] = data[packet_index++];
	}
}

/*============================================================================*
 * Automated UDP Unit Tests	                                                  *
 *============================================================================*/

static char* udp_data = "this is a udp packet";
static uint32_t udp_data_length = 20;
static uint32_t udp_recv_counter;

/**
 * @brief Processes a received packet.
 *
 * @author Atoine Saget
 */
static void udp_echo_recv(
	void *arg,
	struct udp_pcb *pcb,
	struct pbuf *p,
	const ip_addr_t *addr,
	u16_t port
)
{
	UNUSED(arg);
	UNUSED(addr);
	UNUSED(port);
	UNUSED(pcb);

	KASSERT(p != NULL);

	/* Check packet integrity. */
	for (int i = 0; i < p->len; i++)
		KASSERT(udp_data[i] == ((uint8_t *)p->payload)[i]);
	udp_recv_counter++;

	pbuf_free(p);
}

/**
 * @brief Sends and receives UDP packets in loopback mode.
 *
 * @author Atoine Saget
 */
static void test_lwip_udp_send_receive(void)
{
	udp_recv_counter = 0;

	/* Creating the udp pcb */
	struct udp_pcb *udp_pcb;
	udp_pcb = udp_new();
	udp_bind(udp_pcb, IP_ADDR_ANY, 1235);
	udp_recv(udp_pcb, udp_echo_recv, NULL);

	/* Initialize a packet */
	struct pbuf *p;
	p = pbuf_alloc(PBUF_RAW, udp_data_length, PBUF_POOL);
	fill_pbuf(p, (uint8_t *)udp_data);

	uint32_t udp_send_counter = 0;
	/* Loop until we send 20 packets. If we loop forever here it mean that either
	we don't send any packet or we don't receive any */
	while (udp_recv_counter < 20)
	{
		if (udp_send_counter < 20)
		{
			udp_send_counter++;
			udp_sendto(udp_pcb, p, &netif->ip_addr, 1235);
		}

		if (network_is_new_packet())
			nic_driver_if_input(netif);
		sys_check_timeouts();
	}
	KASSERT(udp_recv_counter == 20);

	pbuf_free(p);
	udp_disconnect(udp_pcb);
}

/*============================================================================*
 * Automated TCP Unit Tests	                                                  *
 *============================================================================*/

static const char *tcp_data = "this is a tcp packet";
static uint32_t tcp_data_length = 20;
static bool tcp_can_send;
static uint32_t tcp_ack_counter;
static uint32_t tcp_recv_counter;
static uint32_t tcp_sent_counter;
static struct tcp_pcb *sender_pcb;
static struct tcp_pcb *receiver_pcb;

/**
 * @brief Processes a received packet.
 *
 * @author Atoine Saget
 */
static err_t receiver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(err);

	KASSERT(p != NULL);

	/* Check packet integrity. */
	for (int i = 0; i < p->len; i++)
		KASSERT(tcp_data[i] == ((uint8_t *)p->payload)[i]);
	tcp_recv_counter++;

	tcp_recved(tpcb, p->tot_len);
	pbuf_free(p);

	return (ERR_OK);
}

/**
 * @brief Accepts a TCP connection.
 *
 * @author Atoine Saget
 */
static err_t receive_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(err);

	tcp_setprio(newpcb, TCP_PRIO_MIN);

	/* Set the receive function */
	tcp_recv(newpcb, receiver_recv);

	return (ERR_OK);
}

/**
 * @brief Processes an ACK packet.
 *
 * @author Atoine Saget
 */
static err_t sender_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(tpcb);
	LWIP_UNUSED_ARG(len);

	tcp_ack_counter++;
	tcp_can_send = true;

	return (ERR_OK);
}

/**
 * @brief Establishes a TCP connection.
 *
 * @author Atoine Saget
 */
static err_t tcp_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(tpcb);
	LWIP_UNUSED_ARG(err);

	tcp_sent(sender_pcb, sender_sent);
	tcp_can_send = true;

	return (ERR_OK);
}

/**
 * @brief Sends and receives TCP packets in loopback mode.
 *
 * @author Atoine Saget
 */
static void test_lwip_tcp_send_receive(void)
{
	tcp_can_send = false;
	tcp_ack_counter = 0;
	tcp_recv_counter = 0;
	tcp_sent_counter = 0;

	/* Initialize the TCP server PCB. */
	receiver_pcb = tcp_new();
	KASSERT(receiver_pcb != NULL);
	KASSERT(tcp_bind(receiver_pcb, &netif->ip_addr, 1236) == ERR_OK);
	receiver_pcb = tcp_listen(receiver_pcb);
	tcp_accept(receiver_pcb, receive_accept);

	/* Initialize the TCP client PCB. */
	sender_pcb = tcp_new();
	KASSERT(sender_pcb != NULL);
	KASSERT(tcp_bind(sender_pcb, &netif->ip_addr, 1237) == ERR_OK);
	tcp_connect(sender_pcb, &netif->ip_addr, 1236, tcp_connected);

	/*
	 * Loop until the sender received 20 ack from the receiver.
	 * If we loop forever here it mean that either we don't send
	 * any packet or we don't receive any.
	 */
	while (tcp_ack_counter < 10)
	{
		/*
		 * Only send a new packet when
		 * the last one have been acknowledged.
		 */
		if (tcp_can_send && tcp_sent_counter < 10)
		{
			tcp_can_send = false;
			tcp_write(sender_pcb, tcp_data, tcp_data_length, 0);
			tcp_output(sender_pcb);
			tcp_sent_counter++;
		}

		if (network_is_new_packet())
			nic_driver_if_input(netif);

		sys_check_timeouts();
	}

	KASSERT(tcp_sent_counter == 10 &&
			tcp_recv_counter == 10 &&
			tcp_ack_counter == 10);

	tcp_abort(sender_pcb);
	tcp_close(receiver_pcb);
}

/**
 * @brief Network unit tests.
 */
static struct
{
	void (*test_fn)(); /**< Test function.     */
	const char *type;  /**< Name of test type. */
	const char *name;  /**< Test Name.         */
} network_tests[] = {
	{ test_lwip_tcp_send_receive, "lwip", "send/receive tcp packets" },
	{ test_lwip_udp_send_receive, "lwip", "send/receive udp packets" },
	{ NULL,                        NULL,   NULL                      }
};

/*============================================================================*
 * Automated Test Driver                                                      *
 *============================================================================*/

/**
 * @brief Runs automated tests in LwIP.
 *
 * @author Atoine Saget
 */
PUBLIC void lwip_test(struct netif* _netif)
{
	netif = _netif;

	/* Test not applicable. */
	if (!network_loopback_supported())
		return;

	network_loopback_enable();

	/*
	 * Adding our own IP as a static ARP entry
	 * so that LwIP don't send any ARP request.
	 */
	etharp_add_static_entry(&netif->ip_addr, (struct eth_addr *)(&netif->hwaddr));

	for (int i = 0; network_tests[i].test_fn != NULL; i++)
	{
		network_tests[i].test_fn();
		kprintf("[test][%s] %s [passed]", network_tests[i].type, network_tests[i].name);
	}

	/* Restore previous TCP/IP stack's state. */
	etharp_remove_static_entry(&netif->ip_addr);
	network_loopback_disable();

	netif = NULL;
}
