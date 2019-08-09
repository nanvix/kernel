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

/* Must come first. */
#define __NEED_RTL8139

/**
 * A simple driver for the rtl8139 network interface card.
 * The rtl8139_init function should be called for the driver to work.
 * Raw packets can be sent using the rtl8139_send_packet function.
 */

#include <nanvix/hal/hal.h>
#include <dev/pci.h>
#include <dev/net/net.h>
#include <dev/net/rtl8139.h>
#include <lwip/netif.h>
#include <arch/nic_driver_if.h>
#include <stdint.h>

PRIVATE const uint8_t TSD_array[4] = {0x10, 0x14, 0x18, 0x1C};
PRIVATE const uint8_t TSAD_array[4] = {0x20, 0x24, 0x28, 0x2C};

PRIVATE struct pci_dev pci_rtl8139_device;
PRIVATE struct rtl8139_dev rtl8139_device;
PRIVATE struct netif *mynetif;
PRIVATE struct rtl8139_packet_array recv_packet_array;

/**
 * Enable forwarding packet to lwip when receiving one
 *
 * @author Atoine Saget
 */
PRIVATE void rtl8139_enable_lwip_forwarding(void)
{
	rtl8139_device.lwip_forwarding = 1;
}

/**
 * Disable forwarding packet to lwip when receiving one
 *
 * @author Atoine Saget
 */
PRIVATE void rtl8139_disable_lwip_forwarding(void)
{
	rtl8139_device.lwip_forwarding = 0;
}

/**
 * @brief Retrieve the mac_addr of the device
 *
 * @author Atoine Saget
 */
PRIVATE void rtl8139_read_mac_addr(void)
{
    uint32_t mac_part1 = input32(rtl8139_device.io_base + 0x00);
    uint16_t mac_part2 = input16(rtl8139_device.io_base + 0x04);
    rtl8139_device.mac_addr[0] = mac_part1 >> 0;
    rtl8139_device.mac_addr[1] = mac_part1 >> 8;
    rtl8139_device.mac_addr[2] = mac_part1 >> 16;
    rtl8139_device.mac_addr[3] = mac_part1 >> 24;

    rtl8139_device.mac_addr[4] = mac_part2 >> 0;
    rtl8139_device.mac_addr[5] = mac_part2 >> 8;
}

/**
 * @brief Determine if the given status correspond to a valid or invalid packet
 *
 * @return true if the status correspond to a valid packet, zero otherwise
 *
 * @author Atoine Saget
 */
PRIVATE int rtl8139_packet_status_valid(uint16_t status)
{
	int valid = 1;
	if(!status || status & (1 << 5) || status & (1 << 2) || status & (1 << 1))
		valid = 0;
	return valid;
}

/**
 * @brief simple way of converting virtual to physical adresses
 *
 * @author Atoine Saget
 */
PRIVATE uint32_t virtual_to_physical(uint32_t addr)
{
	return ((addr > KBASE_VIRT) ? (addr - KBASE_VIRT) : addr);
}

/**
 * @brief Send a packet containing data data of lenght len. This function should
 * be called by the lwIP stack
 *
 * @author Atoine Saget
 */
PRIVATE void rtl8139_send_packet(struct packet packet)
{
	if(packet.len > 1500)
	{
		kprintf("The rtl8139 driver does not support long packet.");
		KASSERT(0);
	}
    /* Write data (starting adress of contiguous data) and lenght to the current
    descriptors. */
    output32(rtl8139_device.io_base + TSAD_array[rtl8139_device.tx_cur],
			virtual_to_physical((uint32_t)packet.data));
    output32(rtl8139_device.io_base + TSD_array[rtl8139_device.tx_cur],
			packet.len | 0x003f0000);

    /* Go to the next descriptor (if > 3 -> 0)*/
    rtl8139_device.tx_cur = (rtl8139_device.tx_cur + 1) & 0x3;
}

/**
 * @brief Reset the Receive buffer
 *
 * @author Atoine Saget
 */
PRIVATE void rtl8139_reset_rx(void)
{
	uint8_t tmp;
	tmp = input8(rtl8139_device.io_base + RTL8139_COMMAND);

	/* Receive disable */
	output8(rtl8139_device.io_base + RTL8139_COMMAND, tmp & (0 << 2));

	for(int i = 0; i < PACKET_ARRAY_LENGTH; i++)
	{
		recv_packet_array.packets[i].data = NULL;
	}
	recv_packet_array.read_pos = 0;
	recv_packet_array.write_pos = 0;

	/* Receive enable */
	output8(rtl8139_device.io_base + RTL8139_COMMAND, tmp);

	/* Back to the beginning of the ring buffer */
	rtl8139_device.rx_cur = 0;
}

/**
 * @brief Add packet to packet_array if the array isn't full
 *
 * @author Atoine Saget
 */
PRIVATE void write_packet_array(struct rtl8139_packet_array* packet_array, struct packet packet)
{
	/* If the packet array is full, drop the packet */
	if((packet_array->write_pos + 1) % PACKET_ARRAY_LENGTH == packet_array->read_pos)
	{
		kprintf("Warning : packet array full, dropping packet (increase PACKET_ARRAY_LENGTH)");
		return;
	}

	/* If we are about to override the receive buffer, tell the user.
	In this case, the user should treat packets faster */
	if(packet_array->packets[packet_array->read_pos].data != NULL &&
		packet.data <= packet_array->packets[packet_array->read_pos].data &&
		packet.data + packet.len <= packet_array->packets[packet_array->read_pos].data)
		kprintf("Warning : packet buffer full, override old packets (you should treat packets faster)");

	/* Add the packet to the array */
	packet_array->packets[packet_array->write_pos] = packet;
	/* Increment the write_pos, if at the end, go back to the beginning */
	packet_array->write_pos = (packet_array->write_pos + 1) &(PACKET_ARRAY_LENGTH - 1);
}

/**
 * @brief Function called by the handler when a packet is received. Should forward
 * the packet to the lwIP stack
 * WORK IN PROGRESS
 *
 * @author Atoine Saget
 */
PRIVATE void rtl8139_receive_packet(void)
{
	/* do ... while the receive buffer is not empty  */
	do
	{
		/* Find the beginning of the packet */
		struct rtl8139_packet_header* rcv_packet_header = (struct rtl8139_packet_header *)
		(rtl8139_device.rx_buffer + rtl8139_device.rx_cur);

		/* Check if the packet is valid or not */
		if(!rtl8139_packet_status_valid(rcv_packet_header->status))
		{
			kprintf("Invalid packet, dropping it, and resetting Rx");
			rtl8139_reset_rx();
			return;
		}

		/* Add the received packet to the packets array */
		struct packet packet;
		packet.len = rcv_packet_header->size - 4;
		packet.data = (uint8_t *)(rtl8139_device.rx_buffer + rtl8139_device.rx_cur + 4);
		write_packet_array(&recv_packet_array, packet);

		/* Forward it to lwip if lwip_forwarding is enabled */
		if(rtl8139_device.lwip_forwarding)
			nic_driver_if_input(mynetif);

		/* Update the offset */
		rtl8139_device.rx_cur = (rtl8139_device.rx_cur + rcv_packet_header->size + 4 + 3)
		& RTL8139_RX_READ_POINTER_MASK;
        output16(rtl8139_device.io_base + RTL8139_CAPR, rtl8139_device.rx_cur - 0x10);

		/* Is the offset is at the end of the ring buffer,
		go back to the beginning of the buffer */
		if (rtl8139_device.rx_cur >= RTL8139_RX_BUF_SIZE - 16)
			rtl8139_device.rx_cur -= (RTL8139_RX_BUF_SIZE - 16);

    } while (!(input8(rtl8139_device.io_base + RTL8139_COMMAND) & 1));
}

/**
 * @brief Read a packet from packet_array in array.
 *
 * @return true if a new packet as been fecthed, zero otherwise
 *
 * @author Atoine Saget
 */
PRIVATE int read_packet_array(struct rtl8139_packet_array* packet_array, struct packet* packet)
{
	/*There is nothing to read */
	if(packet_array->read_pos == packet_array->write_pos)
		return (0);

	/* Copy the content of the packet */
	*packet = packet_array->packets[packet_array->read_pos];
	/* Reset the value in the packet array */
	packet_array->packets[packet_array->read_pos].data = NULL;
	/* Increment the read_pos, if at the end, go back to the beginning */
	packet_array->read_pos = (packet_array->read_pos + 1) & (PACKET_ARRAY_LENGTH - 1);

	return (1);
}

/**
 * @brief Interruption handler.
 *
 * @author Atoine Saget
 */
PRIVATE void rtl8139_handler(int num)
{
    UNUSED(num);

    /* Switch off interruptions during the time of the function */
    output16(rtl8139_device.io_base + RTL8129_INTERRUPT_MASK, 0x0);

    uint16_t status = input16(rtl8139_device.io_base + 0x3e);
    if (status & RTL8139_TOK)
    {
        /* Packet sent */
    }
    if (status & RTL8139_ROK)
    {
        /* Packet received */
        rtl8139_receive_packet();
    }

    /* Switch interruptions back on and clear them */
    output16(rtl8139_device.io_base + RTL8129_INTERRUPT_MASK, RTL8139_ROK | RTL8139_TOK);
    output16(rtl8139_device.io_base + RTL8129_INTERRUPT_STATUS, RTL8139_ROK | RTL8139_TOK);
}

/**
 * Initialize the rtl8139 card driver : power on, interruption, ...
 *
 * @author Atoine Saget
 */
PRIVATE void rtl8139_init(struct netif *netif)
{
	mynetif = netif;
	uint32_t pci_command_reg = 0;
	uint32_t irq_num = 0;

	/* Initialize the array */
	recv_packet_array.read_pos = 0;
	recv_packet_array.write_pos = 0;
	for(int i = 0; i < PACKET_ARRAY_LENGTH; i++)
	{
		recv_packet_array.packets[i].data = NULL;
	}

	/* Check if PACKET_ARRAY_LENGTH is a power of 2 to use this property later */
	KASSERT(PACKET_ARRAY_LENGTH != 0 && (PACKET_ARRAY_LENGTH & (PACKET_ARRAY_LENGTH - 1)) == 0);

	rtl8139_device.lwip_forwarding = 0;
	/* Find the network device using the PCI driver and the Vendor ID and
	Device ID of the card */
	pci_rtl8139_device = dev_pci_get_device(RTL8139_VENDOR_ID, RTL8139_DEVICE_ID, -1);

	/* Retrieve important information such as the io_base adress */
	rtl8139_device.io_base = dev_pci_read(pci_rtl8139_device, PCI_BAR0) & (~0x3);

	/* Each packet will be send to a different transmit descriptor than the previous
	one, starting at index 0 */
	rtl8139_device.tx_cur = 0;

	/* Each received packet is at a specific offset in the receive ring buffer,
	starting at offset 0 */
	rtl8139_device.rx_cur = 0;

	/* Enable PCI bus mastering: allow DMA */
	pci_command_reg = dev_pci_read(pci_rtl8139_device, PCI_COMMAND);
	if (!(pci_command_reg & (1 << 2)))
	{
		pci_command_reg |= (1 << 2);
		dev_pci_write(pci_rtl8139_device, PCI_COMMAND, pci_command_reg);
	}

	/* Power on the device */
	output8(rtl8139_device.io_base + RTL8139_CONFIG1, 0x0);

	/* Soft reset (clearing buffers) */
	output8(rtl8139_device.io_base + RTL8139_COMMAND, 0x10);
	while ((input8(rtl8139_device.io_base + RTL8139_COMMAND) & 0x10) != 0);

	/* Initialisation of the receive buffer */
	kmemset(rtl8139_device.rx_buffer, 0x0, RTL8139_RX_BUF_ALLOC_SIZE);
	/* - KBASE_VIRT to translate from virtual to physical adress */
	output32(rtl8139_device.io_base + RTL8139_RX_BUFFER,
			virtual_to_physical((uint32_t)rtl8139_device.rx_buffer));

	/* Toggle receive and send interuptions on  */
	output16(rtl8139_device.io_base + RTL8129_INTERRUPT_MASK, RTL8139_ROK | RTL8139_TOK);

	/* Accepting all kind of packets (Broadcast, Multicast, ...) and
	setting up the rx_buffer so that it can overflow (WRAP 1)
	(easier to handle this way) */
	output32(rtl8139_device.io_base + RTL8139_RX_CONFIG, 0xf | (1 << 7));

	/* Enabling Transmitter and Receiver */
	output8(rtl8139_device.io_base + RTL8139_COMMAND, 0x0C);

	/* Make sure to disable Loopback mode */
	output32(rtl8139_device.io_base + RTL8139_TX_CONFIG, 0);

	/* Register an interrupt that will be fired on packet reception and sending */
	irq_num = dev_pci_read(pci_rtl8139_device, PCI_INTERRUPT_LINE);
	interrupt_register(irq_num, rtl8139_handler);

	rtl8139_read_mac_addr();
}

/*============================================================================*
 * Exported Interface                                                         *
 *============================================================================*/

/**
 * @todo Provide a long description to this function.
 */
PUBLIC void network_init(struct netif* netif)
{
	rtl8139_init(netif);
	network_loopback_disable();
	rtl8139_disable_lwip_forwarding();
}

/**
 * @todo Provide a long description to this function.
 */
PUBLIC void network_send_packet(struct packet packet)
{
	rtl8139_send_packet(packet);
}

/**
 * @todo Provide a long description to this function.
 */
PUBLIC int network_get_new_packet(struct packet* packet)
{
	return read_packet_array(&recv_packet_array, packet);
}

/**
 * @todo Provide a long description to this function.
 */
PUBLIC int network_is_new_packet(void)
{
	return (recv_packet_array.read_pos != recv_packet_array.write_pos);
}

/**
 * @todo Provide a long description to this function.
 */
PUBLIC int network_loopback_supported(void)
{
	return (1);
}

/**
 * @todo Provide a long description to this function.
 */
PUBLIC void network_loopback_enable(void)
{
	/* Only receive packets send by the card itself. */
	output32(rtl8139_device.io_base + RTL8139_RX_CONFIG, (1 << 1) | (1 << 7));

    /* Enable loopback mode. */
	output32(rtl8139_device.io_base + RTL8139_TX_CONFIG, (0x3 << 17));
	rtl8139_reset_rx();
}

/**
 * @todo Provide a long description to this function.
 */
PUBLIC void network_loopback_disable(void)
{
	/* Receive all packets */
	output32(rtl8139_device.io_base + RTL8139_RX_CONFIG, 0xf | (1 << 7));

    /* Disable loopback mode */
	output32(rtl8139_device.io_base + RTL8139_TX_CONFIG, 0);
	rtl8139_reset_rx();
}

/**
 * @todo Provide a long description to this function.
 */
PUBLIC void network_get_mac_adress(uint8_t mac[6])
{
	for(int i = 0; i < 6; i++)
	{
		mac[i] = rtl8139_device.mac_addr[i];
	}
}
