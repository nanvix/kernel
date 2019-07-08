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

PRIVATE void dev_net_rtl8139_handler(int num);
PRIVATE void dev_net_rtl8139_read_mac_addr();

PRIVATE struct pci_dev pci_rtl8139_device;
PRIVATE struct rtl8139_dev rtl8139_device;

PRIVATE uint8_t TSAD_array[4] = {0x20, 0x24, 0x28, 0x2C};
PRIVATE uint8_t TSD_array[4] = {0x10, 0x14, 0x18, 0x1C};

PRIVATE uint32_t current_packet_ptr_offset;

/**
 * Initialize the rtl8139 card driver : power on, interruption, ...
 */
PUBLIC void dev_net_rtl8139_init()
{
    current_packet_ptr_offset = 0;

    /* Find the network device using the PCI driver and the Vendor ID and Device ID of the card */
    pci_rtl8139_device = dev_pci_get_device(RTL8139_VENDOR_ID, RTL8139_DEVICE_ID, -1);
    
    /* Retrieve important information such as the io_base adress */
    uint32_t ret = dev_pci_read(pci_rtl8139_device, PCI_BAR0);
    rtl8139_device.io_base = ret & (~0x3);
    // rtl8139_device.bar_type = ret & 0x1;
    // rtl8139_device.mem_base = ret & (~0xf);

    /* Each packet will be send to a different transmit descriptor than the previous 
    one, starting at index 0 */
    rtl8139_device.tx_cur = 0;

    /* Enable PCI bus mastering: allow DMA */
    uint32_t pci_command_reg = dev_pci_read(pci_rtl8139_device, PCI_COMMAND);
    if (!(pci_command_reg & (1 << 2)))
    {
        pci_command_reg |= (1 << 2);
        dev_pci_write(pci_rtl8139_device, PCI_COMMAND, pci_command_reg);
    }

    /* Power on the device */
    i486_output8(rtl8139_device.io_base + CONFIG1, 0x0);

    /* Soft reset (clearing buffers) */
    i486_output8(rtl8139_device.io_base + COMMAND, 0x10);
    while ((i486_input8(rtl8139_device.io_base + COMMAND) & 0x10) != 0);

    /* Initialisation of the receive buffer */
    /* Using a tmp_buffer instead of defining the size directly inside the 
    structure so that the buffer is alocated in the stack.
    We need this buffer to be allocated in this stack because when we write to the
    card to indicate the adress, it has to be a physical adress. And, it seems 
    that virtual adress are equal to physical adress only with the stack, otherwise,
    a conversion would be needed. */
    uint8_t tmp_buffer[RX_BUF_ALLOC_SIZE];
    rtl8139_device.rx_buffer = tmp_buffer;
    kmemset(rtl8139_device.rx_buffer, 0x0, RX_BUF_ALLOC_SIZE);
    i486_output32(rtl8139_device.io_base + RX_BUFFER, (uint32_t) rtl8139_device.rx_buffer);
    // kprintf("%d %d",(paddr_t)rtl8139_device.rx_buffer, mmio_get((paddr_t)rtl8139_device.rx_buffer));
    
    /* Toggle receive and send interuptions on  */
    i486_output16(rtl8139_device.io_base + INTERRUPT_MASK, ROK | TOK);

    /* Accepting all kind of packets (Broadcast, Multicast, ...) and 
    setting up the rx_buffer so that it can overflow (easier to handle this way) */
    i486_output32(rtl8139_device.io_base + RX_CONFIG, 0xf | (1 << 7));

    /* Enabling Transmitter and Receiver */
    i486_output8(rtl8139_device.io_base + COMMAND, 0x0C);

    /* Register an interrupt that will be fired on packet reception and sending */
    uint32_t irq_num = dev_pci_read(pci_rtl8139_device, PCI_INTERRUPT_LINE);
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
    // First, copy the data to a physically contiguous chunk of memory
    // void *transfer_data = kmalloc(len);
    // kmemcpy(transfer_data, data, len);

    /* Write data (starting adress of contiguous data) and lenght to the current 
    descriptors. */
    i486_output32(rtl8139_device.io_base + TSAD_array[rtl8139_device.tx_cur], (uint32_t)data);
    i486_output32(rtl8139_device.io_base + TSD_array[rtl8139_device.tx_cur], len | 0x003f0000);
    
    /* Go to the next descriptor (if > 3 -> 0)*/
    rtl8139_device.tx_cur = (rtl8139_device.tx_cur + 1) & 0x3;
}

/**
 * @brief Function called by the handler when a packet is received. Should forward
 * the packet to the lwIP stack
 * WORK IN PROGRESS
 */
PRIVATE void dev_net_rtl8139_receive_packet()
{
    do 
    {
        uint16_t *t = (uint16_t *)(rtl8139_device.rx_buffer + current_packet_ptr_offset);
        
        // Skip packet header, get packet length
        uint16_t packet_length = *(t + 1);
        kprintf("%d", packet_length);

        // Skip, packet header and packet length, now t points to the packet data
        // t = t + 2;

        // Now, ethernet layer starts to handle the packet(be sure to make a copy of the packet, insteading of using the buffer)
        // and probabbly this should be done in a separate thread...    
        // void *packet = kmalloc(packet_length);
        // memcpy(packet, t, packet_length);
        // ethernet_handle_packet(packet, packet_length);

        current_packet_ptr_offset = ((current_packet_ptr_offset + packet_length + 4 + 3) & RX_READ_POINTER_MASK);

        i486_output16(rtl8139_device.io_base + CAPR, current_packet_ptr_offset - 0x10);
        // i486_output32(rtl8139_device.io_base + RX_BUFFER, (uint32_t) rtl8139_device.rx_buffer);

        if (current_packet_ptr_offset > RX_BUF_SIZE)
            current_packet_ptr_offset -= RX_BUF_SIZE;
    } while (!(i486_input8(rtl8139_device.io_base + COMMAND) & 1));
}

/**
 * @brief Interruption handler.
 * WORK IN PROGRESS
 */
PRIVATE void dev_net_rtl8139_handler(int num)
{
    UNUSED(num);
    
    /* Switch off interruptions during the time of the function */
    i486_output16(rtl8139_device.io_base + INTERRUPT_MASK, 0x0);

    uint16_t status = i486_input16(rtl8139_device.io_base + 0x3e);
    if (status & TOK)
    {
        kprintf("Packet sent\n");
    }
    if (status & ROK)
    {
        kprintf("Received packet\n");
        dev_net_rtl8139_receive_packet();
    }

    /* Switch interruptions back on and clear them */
    i486_output16(rtl8139_device.io_base + INTERRUPT_MASK, ROK | TOK);
    i486_output16(rtl8139_device.io_base + INTERRUPT_STATUS, ROK | TOK);
}

/**
 * @brief Retrieve the mac_addr of the device
 */
PRIVATE void dev_net_rtl8139_read_mac_addr()
{
    uint32_t mac_part1 = i486_input32(rtl8139_device.io_base + 0x00);
    uint16_t mac_part2 = i486_input16(rtl8139_device.io_base + 0x04);
    rtl8139_device.mac_addr[0] = mac_part1 >> 0;
    rtl8139_device.mac_addr[1] = mac_part1 >> 8;
    rtl8139_device.mac_addr[2] = mac_part1 >> 16;
    rtl8139_device.mac_addr[3] = mac_part1 >> 24;

    rtl8139_device.mac_addr[4] = mac_part2 >> 0;
    rtl8139_device.mac_addr[5] = mac_part2 >> 8;

    kprintf("MAC Address: %x:%x:%x:%x:%x:%x\n", rtl8139_device.mac_addr[0], rtl8139_device.mac_addr[1], rtl8139_device.mac_addr[2], rtl8139_device.mac_addr[3], rtl8139_device.mac_addr[4], rtl8139_device.mac_addr[5]);
}