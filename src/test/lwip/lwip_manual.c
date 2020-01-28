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

/**
 * @brief Launch manual tests?
 */
#ifndef __TEST_LWIP_MANUAL
#define __TEST_LWIP_MANUAL 0
#endif

/*============================================================================*
 * Utils Functions                                                            *
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

/**
 * Print the payload of the given pbuf.
 *
 * @author Atoine Saget
 */
static void print_payload(struct pbuf* p)
{
	KASSERT(p != NULL);
	char msg[0x600 + 1];

	kmemcpy(msg, (const void *)p->payload, p->len);
	msg[p->len] = '\0';

	kprintf("%s", msg);
}

/*============================================================================*
 * UDP Manual Tests                                                           *
 *============================================================================*/

static const char *udp_data = "this is a udp packet";
static const uint32_t udp_data_length = 20;
static uint32_t udp_recv_counter;

/**
 * @brief Processes a received UDP packet.
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
	print_payload(p);
	udp_recv_counter++;

	pbuf_free(p);
}

/**
 * @brief Sends UDP packets.
 *
 * @author Atoine Saget
 */
PUBLIC void test_lwip_udp_send_receive_manual(struct netif* netif)
{
#if !(__TEST_LWIP_MANUAL)
	return;
#endif

	etharp_add_static_entry(&netif->ip_addr, (struct eth_addr *)(&netif->hwaddr));

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

	for (int i = 0; i < 5; i ++)
		udp_sendto(udp_pcb, p, &netif->ip_addr, 1235);
	while (udp_recv_counter < 5)
	{
		if (network_is_new_packet())
			nic_driver_if_input(netif);
		sys_check_timeouts();
	}

	pbuf_free(p);
	udp_disconnect(udp_pcb);
	etharp_remove_static_entry(&netif->ip_addr);
	kprintf("[test][lwip] udp send / receive packets manual [passed]");
}

/*============================================================================*
 * TCP Manual Tests                                                           *
 *============================================================================*/

static struct tcp_pcb *receiver_pcb;
static uint32_t tcp_recv_counter;

/**
 * @brief Processes a received TCP packet.
 *
 * @author Atoine Saget
 */
static err_t receiver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(err);

	if (p != NULL)
	{
		print_payload(p);
		tcp_recv_counter++;
	}

	tcp_recved(tpcb, p->tot_len);
	if (p != NULL) pbuf_free(p);
	return ERR_OK;
}

/**
 * @brief Acepts a TCP connection.
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
	return ERR_OK;
}

/**
 * @brief Sends TCP packets.
 *
 * @author Atoine Saget
 */
PUBLIC void test_lwip_tcp_send_receive_manual(struct netif* netif)
{
#if !(__TEST_LWIP_MANUAL)
	return;
#endif

	tcp_recv_counter = 0;

	/* Init the tcp server pcb */
	receiver_pcb = tcp_new();
	KASSERT(receiver_pcb != NULL);
	KASSERT(tcp_bind(receiver_pcb, &netif->ip_addr, 1236) == ERR_OK);
	receiver_pcb = tcp_listen(receiver_pcb);
	tcp_accept(receiver_pcb, receive_accept);

	/* Loop until we receive 5 tcp packet */
	while (tcp_recv_counter < 5)
	{
		if (network_is_new_packet())
		{
			nic_driver_if_input(netif);
		}
		sys_check_timeouts();
	}

	tcp_close(receiver_pcb);
	kprintf("[test][lwip] tcp receive packets manual [passed]");
}

