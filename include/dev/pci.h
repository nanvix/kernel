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
 * it seems that I should add some doxygen stuff here, but I don't know what
 */

#ifndef PCI_H_
#define PCI_H_

#include <stdint.h>

union pci_dev {
    uint32_t bits;
    
    uint32_t always_zero : 2;
    uint32_t field_num : 6;
    uint32_t function_num : 3;
    uint32_t device_num : 5;
    uint32_t bus_num : 8;
    uint32_t reserved : 7;
    uint32_t enable : 1;
};

// Ports
#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

// Config Address Register

// Offset
#define PCI_VENDOR_ID 0x00
#define PCI_DEVICE_ID 0x02
#define PCI_COMMAND 0x04
#define PCI_STATUS 0x06
#define PCI_REVISION_ID 0x08
#define PCI_PROG_IF 0x09
#define PCI_SUBCLASS 0x0a
#define PCI_CLASS 0x0b
#define PCI_CACHE_LINE_SIZE 0x0c
#define PCI_LATENCY_TIMER 0x0d
#define PCI_HEADER_TYPE 0x0e
#define PCI_BIST 0x0f
#define PCI_BAR0 0x10
#define PCI_BAR1 0x14
#define PCI_BAR2 0x18
#define PCI_BAR3 0x1C
#define PCI_BAR4 0x20
#define PCI_BAR5 0x24
#define PCI_INTERRUPT_LINE 0x3C
#define PCI_SECONDARY_BUS 0x09

// Device type
#define PCI_HEADER_TYPE_DEVICE 0
#define PCI_HEADER_TYPE_BRIDGE 1
#define PCI_HEADER_TYPE_CARDBUS 2
#define PCI_TYPE_BRIDGE 0x0604
#define PCI_TYPE_SATA 0x0106
#define PCI_NONE 0xFFFF

#define NUMBER_OF_BUSES 256
#define DEVICE_PER_BUS 32
#define FUNCTION_PER_DEVICE 8 // 32

uint32_t dev_pci_read(union pci_dev dev, uint32_t field);
void dev_pci_write(union pci_dev dev, uint32_t field, uint32_t value);

uint32_t dev_pci_get_device_type(union pci_dev dev);
uint32_t dev_pci_get_secondary_bus(union pci_dev dev);
uint32_t dev_pci_reach_end(union pci_dev dev);

union pci_dev dev_pci_scan_function(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device, uint32_t function, uint32_t device_type);
union pci_dev dev_pci_scan_device(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device, uint32_t device_type);
union pci_dev dev_pci_scan_bus(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device_type);
union pci_dev dev_pci_get_device(uint16_t vendor_id, uint16_t device_id, uint32_t device_type);

void pci_init();

#endif /* PCI_H_ */