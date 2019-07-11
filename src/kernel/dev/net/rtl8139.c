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

#include <dev/pci.h>
#include <dev/net/rtl8139.h>
#include <nanvix/hal/hal.h>
#include <nanvix/klib.h>

/**
 * A simple driver for the rtl8139 network interface card.
 * The dev_net_rtl8139_init function should be called for the driver to work.
 * Packets are received and (work in progress) forwarded to the lwIP stack.
 * Raw packets can be sent using the dev_net_rtl8139_send_packet function.
 */

/* PRIVATE functions */
PRIVATE void dev_net_rtl8139_handler(int num);
PRIVATE void dev_net_rtl8139_read_mac_addr(void);
PRIVATE void dev_net_rtl8139_reset_rx(void);

PRIVATE const uint8_t TSD_array[4] = {0x10, 0x14, 0x18, 0x1C};
PRIVATE const uint8_t TSAD_array[4] = {0x20, 0x24, 0x28, 0x2C};

/* PRIVATE global variables */
/* Those variables are global for an easier implementation, but, in the case of
using multiple rtl8139 cards, an "argument passing" implementation is required */
PRIVATE struct pci_dev pci_rtl8139_device;
PRIVATE struct rtl8139_dev rtl8139_device;

/*   RTL RX Header          802.3 Ethernet Frame          32 bit Align
	* <---------------><------------------------------------><---------->
	*  -----------------------------------------------------------------
	* | STATUS | SIZE | DMAC | SMAC | LEN/TYPE | DATA | FCS | ALIGNMENT |
	*  -----------------------------------------------------------------
	*     2       2       6      6       2       /~/     4      [0;3]
	* simple structure used for clarity */
struct packet_header {
	uint16_t status;
	uint16_t size;
};

/**
 * Initialize the rtl8139 card driver : power on, interruption, ...
 */
PUBLIC void dev_net_rtl8139_init(void)
{
	uint32_t pci_command_reg = 0;
	uint32_t irq_num = 0;

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
	output8(rtl8139_device.io_base + CONFIG1, 0x0);

	/* Soft reset (clearing buffers) */
	output8(rtl8139_device.io_base + COMMAND, 0x10);
	while ((input8(rtl8139_device.io_base + COMMAND) & 0x10) != 0);

	/* Initialisation of the receive buffer */
	kmemset(rtl8139_device.rx_buffer, 0x0, RX_BUF_ALLOC_SIZE);
	/* - KBASE_VIRT to translate from virtual to physical adress */
	output32(rtl8139_device.io_base + RX_BUFFER,
				(uint32_t) rtl8139_device.rx_buffer - KBASE_VIRT);

	/* Toggle receive and send interuptions on  */
	output16(rtl8139_device.io_base + INTERRUPT_MASK, ROK | TOK);

	/* Accepting all kind of packets (Broadcast, Multicast, ...) and
	setting up the rx_buffer so that it can overflow (WRAP 1)
	(easier to handle this way) */
	output32(rtl8139_device.io_base + RX_CONFIG, 0xf | (1 << 7));

	/* Enabling Transmitter and Receiver */
	output8(rtl8139_device.io_base + COMMAND, 0x0C);

	/* Make sure to disable Loopback mode */
	output32(rtl8139_device.io_base + TX_CONFIG, 0);

	/* Register an interrupt that will be fired on packet reception and sending */
	irq_num = dev_pci_read(pci_rtl8139_device, PCI_INTERRUPT_LINE);
	interrupt_register(irq_num, dev_net_rtl8139_handler);

	dev_net_rtl8139_read_mac_addr();
}

/**
 * @brief Send a packet containing data data of lenght len. This function should
 * be called by the lwIP stack
 * WORK IN PROGRESS
 */
PUBLIC void dev_net_rtl8139_send_packet(void *data, uint32_t len)
{
    /* Write data (starting adress of contiguous data) and lenght to the current
    descriptors. */
    output32(rtl8139_device.io_base + TSAD_array[rtl8139_device.tx_cur], (uint32_t)data);
    output32(rtl8139_device.io_base + TSD_array[rtl8139_device.tx_cur], len | 0x003f0000);

    /* Go to the next descriptor (if > 3 -> 0)*/
    rtl8139_device.tx_cur = (rtl8139_device.tx_cur + 1) & 0x3;
}

/**
 * @brief return a pointer to the rtl8139 device (used for testing)
 */
PUBLIC struct rtl8139_dev* dev_net_rtl8139_get_device(void)
{
	return &rtl8139_device;
}

/**
 * @brief Determine if the given status correspond to a valid or invalid packet
 *
 * @return true if the status correspond to a valid packet, false otherwise
 */
PUBLIC bool dev_net_rtl8139_packet_status_valid(uint16_t status)
{
	bool valid = true;
	if(!status || status & (1 << 5) || status & (1 << 2) || status & (1 << 1)) {
		valid = false;
	}
	return valid;
}

/**
 * @brief Function called by the handler when a packet is received. Should forward
 * the packet to the lwIP stack
 * WORK IN PROGRESS
 */
PRIVATE void dev_net_rtl8139_receive_packet(void)
{
	/* do ... while the receive buffer is not empty  */
	do
	{
		/* Find the beginning of the packet */
		struct packet_header* rcv_packet_header = (struct packet_header *)
		(rtl8139_device.rx_buffer + rtl8139_device.rx_cur);

		/* Check if the packet is valid or not */
		if(!dev_net_rtl8139_packet_status_valid(rcv_packet_header->status))
		{
			kprintf("Invalid packet, dropping it, and resetting Rx");
			dev_net_rtl8139_reset_rx();
			return;
		}

		/* Copy the packet in memory and forward the packet to lwIP */
		/* Work in progress */

		/* Update the offset */
		rtl8139_device.rx_cur = (rtl8139_device.rx_cur + rcv_packet_header->size + 4 + 3)
		& RX_READ_POINTER_MASK;
        output16(rtl8139_device.io_base + CAPR, rtl8139_device.rx_cur - 0x10);

		/* Is the offset is at the end of the ring buffer,
		go back to the beginning of the buffer */
		if (rtl8139_device.rx_cur > RX_BUF_SIZE)
			rtl8139_device.rx_cur -= (RX_BUF_SIZE - 16);

    } while (!(input8(rtl8139_device.io_base + COMMAND) & 1));
}

/**
 * @brief Interruption handler.
 * WORK IN PROGRESS
 */
PRIVATE void dev_net_rtl8139_handler(int num)
{
    UNUSED(num);

    /* Switch off interruptions during the time of the function */
    output16(rtl8139_device.io_base + INTERRUPT_MASK, 0x0);

    uint16_t status = input16(rtl8139_device.io_base + 0x3e);
    if (status & TOK)
    {
        /* Packet sent */
    }
    if (status & ROK)
    {
        /* Packet received */
        dev_net_rtl8139_receive_packet();
    }

    /* Switch interruptions back on and clear them */
    output16(rtl8139_device.io_base + INTERRUPT_MASK, ROK | TOK);
    output16(rtl8139_device.io_base + INTERRUPT_STATUS, ROK | TOK);
}

/**
 * @brief Reset the Receive buffer
 */
PRIVATE void dev_net_rtl8139_reset_rx(void)
{
	uint8_t tmp;
	tmp = input8(rtl8139_device.io_base + COMMAND);

	/* Receive disable */
	output8(rtl8139_device.io_base + COMMAND, tmp & (0 << 2));
	/* Receive enable */
	output8(rtl8139_device.io_base + COMMAND, tmp);

	/* Accepting all kind of packets (Broadcast, Multicast, ...) and
	setting up the rx_buffer so that it can overflow (WRAP 1)
	(easier to handle this way) */
	output32(rtl8139_device.io_base + RX_CONFIG, 0xf | (1 << 7));

	/* Back to the beginning of the ring buffer */
	rtl8139_device.rx_cur = 0;
}

/**
 * @brief Retrieve the mac_addr of the device
 */
PRIVATE void dev_net_rtl8139_read_mac_addr(void)
{
    uint32_t mac_part1 = input32(rtl8139_device.io_base + 0x00);
    uint16_t mac_part2 = input16(rtl8139_device.io_base + 0x04);
    rtl8139_device.mac_addr[0] = mac_part1 >> 0;
    rtl8139_device.mac_addr[1] = mac_part1 >> 8;
    rtl8139_device.mac_addr[2] = mac_part1 >> 16;
    rtl8139_device.mac_addr[3] = mac_part1 >> 24;

    rtl8139_device.mac_addr[4] = mac_part2 >> 0;
    rtl8139_device.mac_addr[5] = mac_part2 >> 8;

    kprintf("MAC Address: %x:%x:%x:%x:%x:%x\n",
	rtl8139_device.mac_addr[0], rtl8139_device.mac_addr[1],
	rtl8139_device.mac_addr[2], rtl8139_device.mac_addr[3],
	rtl8139_device.mac_addr[4], rtl8139_device.mac_addr[5]);
}
