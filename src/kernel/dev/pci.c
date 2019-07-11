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

/*
 * PCI Init, filling size for each field in config space
 * */
#include <pci.h>

uint32_t pci_size_map[100];
union pci_dev dev_zero = {0};

/*
 * Given a pci device(32-bit vars containing info about bus, device number, and function number), a field(what u want to read from the config space)
 * Read it for me !
 * */
uint32_t pci_read(union pci_dev dev, uint32_t field)
{
    // Only most significant 6 bits of the field
    dev.field_num = (field & 0xFC) >> 2;
    dev.enable = 1;
    outportl(PCI_CONFIG_ADDRESS, dev.bits);

    // What size is this field supposed to be ?
    uint32_t size = pci_size_map[field];
    if (size == 1)
    {
        // Get the first byte only, since it's in little endian, it's actually the 3rd byte
        uint8_t t = inportb(PCI_CONFIG_DATA + (field & 3));
        return t;
    }
    else if (size == 2)
    {
        uint16_t t = inports(PCI_CONFIG_DATA + (field & 2));
        return t;
    }
    else if (size == 4)
    {
        // Read entire 4 bytes
        uint32_t t = inportl(PCI_CONFIG_DATA);
        return t;
    }
    return 0xffff;
}

/*
 * Write pci field
 * */
void pci_write(union pci_dev dev, uint32_t field, uint32_t value)
{
    dev.field_num = (field & 0xFC) >> 2;
    dev.enable = 1;
    // Tell where we want to write
    outportl(PCI_CONFIG_ADDRESS, dev.bits);
    // Value to write
    outportl(PCI_CONFIG_DATA, value);
}

/*
 * Get device type (i.e, is it a bridge, ide controller ? mouse controller? etc)
 * */
uint32_t get_device_type(union pci_dev dev)
{
    uint32_t t = pci_read(dev, PCI_CLASS) << 8;
    return t | pci_read(dev, PCI_SUBCLASS);
}

/*
 * Get secondary bus from a PCI bridge device
 * */
uint32_t get_secondary_bus(union pci_dev dev)
{
    return pci_read(dev, PCI_SECONDARY_BUS);
}

/*
 * Is current device an end point ? PCI_HEADER_TYPE 0 is end point
 * */
uint32_t pci_reach_end(union pci_dev dev)
{
    uint32_t t = pci_read(dev, PCI_HEADER_TYPE);
    return !t;
}

/*
 * The following three functions are basically doing recursion, enumerating each and every device connected to pci
 * We start with the primary bus 0, which has 8 function, each of the function is actually a bus
 * Then, each bus can have 8 devices connected to it, each device can have 8 functions
 * When we gets to enumerate the function, check if the vendor id and device id match, if it does, we've found our device !
 **/

/*
 * Scan function
 * */
union pci_dev pci_scan_function(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device, uint32_t function, int device_type)
{
    union pci_dev dev = {0};
    dev.bus_num = bus;
    dev.device_num = device;
    dev.function_num = function;

    // If it's a PCI Bridge device, get the bus it's connected to and keep searching
    if (get_device_type(dev) == PCI_TYPE_BRIDGE)
    {
        pci_scan_bus(vendor_id, device_id, get_secondary_bus(dev), device_type);
    }

    // If type matches, we've found the device, just return it
    if (device_type == -1 || device_type == get_device_type(dev))
    {
        uint32_t devid = pci_read(dev, PCI_DEVICE_ID);
        uint32_t vendid = pci_read(dev, PCI_VENDOR_ID);
        if (devid == device_id && vendor_id == vendid)
            return dev;
    }
    return dev_zero;
}

/*
 * Scan device
 * */
union pci_dev pci_scan_device(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device, int device_type)
{
    union pci_dev dev = {0};
    dev.bus_num = bus;
    dev.device_num = device;

    if (pci_read(dev, PCI_VENDOR_ID) == PCI_NONE)
        return dev_zero;

    union pci_dev t = pci_scan_function(vendor_id, device_id, bus, device, 0, device_type);
    if (t.bits)
        return t;

    if (pci_reach_end(dev))
        return dev_zero;

    for (int function = 1; function < FUNCTION_PER_DEVICE; function++)
    {
        if (pci_read(dev, PCI_VENDOR_ID) != PCI_NONE)
        {
            t = pci_scan_function(vendor_id, device_id, bus, device, function, device_type);
            if (t.bits)
                return t;
        }
    }
    return dev_zero;
}
/*
 * Scan bus
 * */
union pci_dev pci_scan_bus(uint16_t vendor_id, uint16_t device_id, uint32_t bus, int device_type)
{
    for (int device = 0; device < DEVICE_PER_BUS; device++)
    {
        union pci_dev t = pci_scan_device(vendor_id, device_id, bus, device, device_type);
        if (t.bits)
            return t;
    }
    return dev_zero;
}

/*
 * Device driver use this function to get its device object(given unique vendor id and device id)
 * */
union pci_dev pci_get_device(uint16_t vendor_id, uint16_t device_id, int device_type)
{

    union pci_dev t = pci_scan_bus(vendor_id, device_id, 0, device_type);
    if (t.bits)
        return t;

    // Handle multiple pci host controllers

    if (pci_reach_end(dev_zero)) // ?
    {
        printf("PCI Get device failed...\n");
    }

    for (int bus = 1; bus < NUMBER_OF_BUSES; bus++)
    {
        /*         union pci_dev dev = {0};
        dev.function_num = function;

        if (pci_read(dev, PCI_VENDOR_ID) == PCI_NONE)
            break; */
        t = pci_scan_bus(vendor_id, device_id, bus, device_type);
        if (t.bits)
            return t;
    }
    return dev_zero;
}

void pci_init()
{
    // Init size map
    pci_size_map[PCI_VENDOR_ID] = 2;
    pci_size_map[PCI_DEVICE_ID] = 2;
    pci_size_map[PCI_COMMAND] = 2;
    pci_size_map[PCI_STATUS] = 2;
    pci_size_map[PCI_SUBCLASS] = 1;
    pci_size_map[PCI_CLASS] = 1;
    pci_size_map[PCI_CACHE_LINE_SIZE] = 1;
    pci_size_map[PCI_LATENCY_TIMER] = 1;
    pci_size_map[PCI_HEADER_TYPE] = 1;
    pci_size_map[PCI_BIST] = 1;
    pci_size_map[PCI_BAR0] = 4;
    pci_size_map[PCI_BAR1] = 4;
    pci_size_map[PCI_BAR2] = 4;
    pci_size_map[PCI_BAR3] = 4;
    pci_size_map[PCI_BAR4] = 4;
    pci_size_map[PCI_BAR5] = 4;
    pci_size_map[PCI_INTERRUPT_LINE] = 1;
    pci_size_map[PCI_SECONDARY_BUS] = 1;
}