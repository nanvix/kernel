/**
 * @file
 * Ethernet Interface between lwIP and the nic_driver interface
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

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

/**
 * Glue layer between the nic_driver interface and lwip
 * (inspired from the ethernetif file given as an example by lwip)
 */
#if defined(__qemu_x86__) || defined(__qemu_openrisc__)

#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "lwip/etharp.h"
#include "netif/ppp/pppoe.h"

#include "dev/net/net.h"
#include "nanvix/hal/hal.h"

#include "arch/nic_driver_if.h"

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 */
struct nic_driver_if
{
	struct eth_addr *ethaddr;
};

/**
 * Called from nic_driver_if().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this nic_driver_if
 */
PRIVATE void low_level_init(struct netif *netif)
{
	/* set MAC hardware address length */
	netif->hwaddr_len = ETHARP_HWADDR_LEN;

	/* maximum transfer unit */
	netif->mtu = 1500;

	/* device capabilities */
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

#if LWIP_IPV6 && LWIP_IPV6_MLD
	/*
	 * For hardware/netifs that implement MAC filtering.
	 * All-nodes link-local is handled by default, so we must let the hardware know
	 * to allow multicast packets in.
	 * Should set mld_mac_filter previously. */
	if (netif->mld_mac_filter != NULL)
	{
		ip6_addr_t ip6_allnodes_ll;
		ip6_addr_set_allnodes_linklocal(&ip6_allnodes_ll);
		netif->mld_mac_filter(netif, &ip6_allnodes_ll, NETIF_ADD_MAC_FILTER);
	}
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */

	/* initialize interface. */
	network_init(netif);

	/* set MAC hardware address */
	network_get_mac_adress(netif->hwaddr);
}

/**
 * This function do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this nic_driver_if
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

PRIVATE err_t low_level_output(struct netif *netif, struct pbuf *p)
{
	UNUSED(netif);
	struct pbuf *q;

	/* Init buffer */
	u8_t *data = mem_calloc(p->tot_len, 1);
	u32_t data_index = 0;

#if ETH_PAD_SIZE
	pbuf_remove_header(p, ETH_PAD_SIZE); /* drop the padding word */
#endif

	for (q = p; q != NULL; q = q->next)
	{
		/* Fill buffer */
		for(int i = 0; i < q->len; i++)
		{
			data[data_index++] = ((u8_t *)q->payload)[i];
		}
	}

	/* Send packet (buffer + len) */
	struct packet packet;
	packet.len = p->tot_len;
	packet.data = data;
	network_send_packet(packet);

	MIB2_STATS_NETIF_ADD(netif, ifoutoctets, p->tot_len);
	if (((u8_t *)p->payload)[0] & 1)
	{
		/* broadcast or multicast packet*/
		MIB2_STATS_NETIF_INC(netif, ifoutnucastpkts);
	}
	else
	{
		/* unicast packet */
		MIB2_STATS_NETIF_INC(netif, ifoutucastpkts);
	}
	/* increase ifoutdiscards or ifouterrors on error */

#if ETH_PAD_SIZE
	pbuf_add_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

	LINK_STATS_INC(link.xmit);

	return ERR_OK;
}

/**
 * Alocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this nic_driver_if
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
PRIVATE struct pbuf* low_level_input(struct netif *netif)
{
	UNUSED(netif);
	struct pbuf *p, *q;

	struct packet packet;
	uint8_t data[1600];
	packet.data = data;
	KASSERT(network_get_new_packet(&packet));
	u32_t packet_index = 0;

#if ETH_PAD_SIZE
	len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

	/* We allocate a pbuf chain of pbufs from the pool. */
	p = pbuf_alloc(PBUF_RAW, packet.len, PBUF_POOL);

	if (p != NULL)
	{

#if ETH_PAD_SIZE
		pbuf_remove_header(p, ETH_PAD_SIZE); /* drop the padding word */
#endif

		/* We iterate over the pbuf chain until we have read the entire
		 * packet into the pbuf. */
		for (q = p; q != NULL; q = q->next)
		{
			/* Read enough bytes to fill this pbuf in the chain. The
			 * available data in the pbuf is given by the q->len
			 * variable.
			 */
			for(int i = 0; i < q -> len; i++)
			{
				((u8_t *)q->payload)[i] = packet.data[packet_index++];
			}
		}

		MIB2_STATS_NETIF_ADD(netif, ifinoctets, p->tot_len);
		if (((u8_t *)p->payload)[0] & 1)
		{
			/* broadcast or multicast packet*/
			MIB2_STATS_NETIF_INC(netif, ifinnucastpkts);
		}
		else
		{
			/* unicast packet*/
			MIB2_STATS_NETIF_INC(netif, ifinucastpkts);
		}
#if ETH_PAD_SIZE
		pbuf_add_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

		LINK_STATS_INC(link.recv);
	}
	else
	{
		/* drop packet(); */
		LINK_STATS_INC(link.memerr);
		LINK_STATS_INC(link.drop);
		MIB2_STATS_NETIF_INC(netif, ifindiscards);
	}

	return p;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this nic_driver_if
 */
void nic_driver_if_input(struct netif *netif)
{
	struct pbuf *p;

	/* move received packet into a new pbuf */
	p = low_level_input(netif);
	/* if no packet could be read, silently ignore this */
	if (p != NULL)
	{
		/* pass all packets to ethernet_input, which decides what packets it supports */
		if (netif->input(p, netif) != ERR_OK)
		{
			LWIP_DEBUGF(NETIF_DEBUG, ("nic_driver_if: IP input error\n"));
			pbuf_free(p);
			p = NULL;
		}
	}
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this nic_driver_if
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t nic_driver_if_init(struct netif *netif)
{
	struct nic_driver_if *nic_driver_if;

	LWIP_ASSERT("netif != NULL", (netif != NULL));

	nic_driver_if = mem_malloc(sizeof(struct nic_driver_if));
	if (nic_driver_if == NULL)
	{
		LWIP_DEBUGF(NETIF_DEBUG, ("nic_driver_if: out of memory\n"));
		return ERR_MEM;
	}

#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

	/*
	 * Initialize the snmp variables and counters inside the struct netif.
	 * The last argument should be replaced with your link speed, in units
	 * of bits per second.
	 */
	MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

	netif->state = nic_driver_if;
	netif->name[0] = IFNAME0;
	netif->name[1] = IFNAME1;
	netif->num = 0;
	netif->next = NULL;
	/* We directly use etharp_output() here to save a function call.
	 * You can instead declare your own function an call etharp_output()
	 * from it if you have to do some checks before sending (e.g. if link
	 * is available...) */
#if LWIP_IPV4
	netif->output = etharp_output;
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
	netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
	netif->linkoutput = low_level_output;

	nic_driver_if->ethaddr = (struct eth_addr *) & (netif->hwaddr[0]);

	/* initialize the hardware */
	low_level_init(netif);

	return ERR_OK;
}

#else
extern int make_iso_compilers_happy;
#endif /* defined(__qemu_x86__) || defined(__qemu_openrisc__) */
