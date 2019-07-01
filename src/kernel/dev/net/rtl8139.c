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

struct pci_dev pci_rtl8139_device;
struct rtl8139_dev rtl8139_device;

// Four TXAD register, you must use a different one to send packet each time(for example, use the first one, second... fourth and back to the first)
uint8_t TSAD_array[4] = {0x20, 0x24, 0x28, 0x2C};
uint8_t TSD_array[4] = {0x10, 0x14, 0x18, 0x1C};

void net_rtl8139_send_packet(void *data, uint32_t len)
{
    // First, copy the data to a physically contiguous chunk of memory
    // void *transfer_data = kmalloc(len);
    // kmemcpy(transfer_data, data, len);

    // Second, fill in physical address of data, and length
    i486_output32(rtl8139_device.io_base + TSAD_array[rtl8139_device.tx_cur], (uint32_t)data);
    i486_output32(rtl8139_device.io_base + TSD_array[rtl8139_device.tx_cur++], len);
    if (rtl8139_device.tx_cur > 3)
        rtl8139_device.tx_cur = 0;
}

void net_rtl8139_handler(int num)
{
    kprintf("RTL8139 interript was fired !!!! %d \n", num);
    uint16_t status = i486_input16(rtl8139_device.io_base + 0x3e);

    if (status & TOK)
    {
        kprintf("Packet sent\n");
    }
    if (status & ROK)
    {
        kprintf("Received packet\n");
        // receive_packet();
    }

    i486_output16(rtl8139_device.io_base + 0x3E, 0x5);
}

void read_mac_addr()
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

/*
 * Initialize the rtl8139 card driver
 * */
void net_rtl8139_init()
{
    // First get the network device using PCI
    pci_rtl8139_device = dev_pci_get_device(RTL8139_VENDOR_ID, RTL8139_DEVICE_ID, -1);
    uint32_t ret = dev_pci_read(pci_rtl8139_device, PCI_BAR0);
    rtl8139_device.bar_type = ret & 0x1;
    // Get io base or mem base by extracting the high 28/30 bits
    rtl8139_device.io_base = ret & (~0x3);
    rtl8139_device.mem_base = ret & (~0xf);
    kprintf("rtl8139 use %s access (base: %x)\n", (rtl8139_device.bar_type == 0) ? "mem based" : "port based", (rtl8139_device.bar_type != 0) ? rtl8139_device.io_base : rtl8139_device.mem_base);

    // Set current TSAD
    rtl8139_device.tx_cur = 0;

    // Enable PCI Bus Mastering
    uint32_t pci_command_reg = dev_pci_read(pci_rtl8139_device, PCI_COMMAND);
    if (!(pci_command_reg & (1 << 2)))
    {
        pci_command_reg |= (1 << 2);
        dev_pci_write(pci_rtl8139_device, PCI_COMMAND, pci_command_reg);
    }

    // Send 0x00 to the CONFIG_1 register (0x52) to set the LWAKE + LWPTN to active high. this should essentially *power on* the device.
    i486_output8(rtl8139_device.io_base + 0x52, 0x0);

    // Soft reset
    i486_output8(rtl8139_device.io_base + 0x37, 0x10);
    while ((i486_input8(rtl8139_device.io_base + 0x37) & 0x10) != 0)
    {
        // Do nothibg here...
    }

    // Allocate receive buffer
    // rtl8139_device.rx_buffer = malloc(8192 + 16 + 1500);
    kmemset(rtl8139_device.rx_buffer, 0x0, 8192 + 16 + 1500);
    // i486_output32(rtl8139_device.io_base + 0x30, (uint32_t)virtual2phys(kpage_dir, rtl8139_device.rx_buffer));
    i486_output32(rtl8139_device.io_base + 0x30, (uint32_t) rtl8139_device.rx_buffer);

    // Sets the TOK and ROK bits high
    i486_output16(rtl8139_device.io_base + 0x3C, 0x0005);

    // (1 << 7) is the WRAP bit, 0xf is AB+AM+APM+AAP
    i486_output32(rtl8139_device.io_base + 0x44, 0xf | (1 << 7));

    // Sets the RE and TE bits high
    i486_output8(rtl8139_device.io_base + 0x37, 0x0C);

    // Register and enable network interrupts
    uint32_t irq_num = dev_pci_read(pci_rtl8139_device, PCI_INTERRUPT_LINE);
    int tmp =  12;
    tmp = interrupt_register(irq_num, net_rtl8139_handler);
    kprintf("%d %d\n", tmp, irq_num);    
    kprintf("Registered irq interrupt for rtl8139, irq num = %d\n", irq_num);

    read_mac_addr();
}
