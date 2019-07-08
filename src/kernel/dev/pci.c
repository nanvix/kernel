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
 * A simple PCI driver that can read, write to PCI devices.
 * Device informations can be retrieved using the dev_pci_get_device function and 
 * the VendorID and DeviceID of the deivce.
 * 
 * The pci_dev structure contain device informations, it correspond to the 
 * CONFIG_ADRESS 32-bit register. The translation of the practical pci_dev structure
 * to the real, contiguous, 32-bit register is done using the 
 * dev_pci_bits_from_fields function.
 */

#include <dev/pci.h>
#include <nanvix/hal/hal.h>

/* PRIVATE functions */
PRIVATE uint32_t dev_pci_get_device_type(struct pci_dev dev);
PRIVATE uint32_t dev_pci_get_secondary_bus(struct pci_dev dev);
PRIVATE uint32_t dev_pci_reach_end(struct pci_dev dev);

PRIVATE struct pci_dev dev_pci_scan_function(uint16_t vendor_id, uint16_t device_id, 
        uint32_t bus, uint32_t device, uint32_t function, uint32_t device_type);
PRIVATE struct pci_dev dev_pci_scan_device(uint16_t vendor_id, uint16_t device_id, 
        uint32_t bus, uint32_t device, uint32_t device_type);
PRIVATE struct pci_dev dev_pci_scan_bus(uint16_t vendor_id, uint16_t device_id, 
        uint32_t bus, uint32_t device_type);

PRIVATE uint32_t dev_pci_bits_from_fields(struct pci_dev dev);
PRIVATE uint32_t pci_size_map(uint32_t field);

PRIVATE struct pci_dev dev_zero = {0};

/**
 * @brief Given a pci_dev and a chosen field, read and return its value
 * 
 * @param dev, the pci_dev you want to read a value from
 * @param register_offset, the register you want to read 
 * 
 * @return the read register if success, 0xffff if fail 
 */
PUBLIC uint32_t dev_pci_read(struct pci_dev dev, uint32_t register_offset)
{
    uint32_t size;
    /* Add the chosen register to the device informations */
    dev.register_offset = register_offset;
    dev.enable = 1;

    /* Request the information */
    output32(PCI_CONFIG_ADDRESS, dev_pci_bits_from_fields(dev));

    size = pci_size_map(register_offset);
    if (size == 1)
    {
        /* Read the first byte (3rd because of little endian) */
        return i486_input8(PCI_CONFIG_DATA + (register_offset & 3));
    }
    else if (size == 2)
    {
        return i486_input16(PCI_CONFIG_DATA + (register_offset & 2));
    }
    else if (size == 4)
    {
        return i486_input32(PCI_CONFIG_DATA);
    }
    return 0xffff;
}

/**
 * @brief Write the value value to the register register_offset of the device dev.
 */
PUBLIC void dev_pci_write(struct pci_dev dev, uint32_t register_offset, uint32_t value)
{
    /* Add the chosen register to the device informations */
    dev.register_offset = register_offset;
    dev.enable = 1;

    /* Indicate wich register of wich device we want to write to */
    output32(PCI_CONFIG_ADDRESS, dev_pci_bits_from_fields(dev));
    /* Write the value */
    output32(PCI_CONFIG_DATA, value);
}

/**
 * @brief Given a vendor_id and device_id find and return the corresponding device.
 * A device_type can also be given, if the given device_type is -1 then only the 
 * device_id and vendor_id are used for finding the device.
 * 
 * @return a pci_dev struct containing all needed information if the device has
 * been found, an empty pci_dev struct otherwise.
 */
PUBLIC struct pci_dev dev_pci_get_device(uint16_t vendor_id, uint16_t device_id, uint32_t device_type)
{
    /* Scan every bus, looking for the device */
    for (int bus = 0; bus < NUMBER_OF_BUSES; bus++)
    {
        struct pci_dev t = dev_pci_scan_bus(vendor_id, device_id, bus, device_type);
        /* If the device has been found, return it */
        if (dev_pci_bits_from_fields(t))
            return t;
    }

    /* If no device were found, return an empty one */
    return dev_zero;
}

/**
 * @brief scan a bus, looking for a device with matching vendor_id and device_id.
 * Function called by dev_pci_get_device
 * 
 * @return a pci_dev struct containing all needed information if the device has
 * been found. An empty pci_dev struct otherwise.
 */
PRIVATE struct pci_dev dev_pci_scan_bus(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device_type)
{
    /* Scan every device on the bus, looking for the device */
    for (int device = 0; device < DEVICE_PER_BUS; device++)
    {
        struct pci_dev t = dev_pci_scan_device(vendor_id, device_id, bus, device, device_type);
        /* If the device has been found, return it */
        if (dev_pci_bits_from_fields(t))
            return t;
    }

    /* If no device were found, return an empty one */
    return dev_zero;
}

/*
 * @brief scan a device, looking for a device with matching vendor_id and device_id.
 * Function called by dev_pci_scan_bus
 * 
 * @return a pci_dev struct containing all needed information if the device has
 * been found. An empty pci_dev struct otherwise.
 */
PRIVATE struct pci_dev dev_pci_scan_device(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device, uint32_t device_type)
{
    /* Scan every function on the device, looking for the device */
    for (int function = 0; function < FUNCTION_PER_DEVICE; function++)
    {
        struct pci_dev t = dev_pci_scan_function(vendor_id, device_id, bus, device, function, device_type);
        /* If the device has been found, return it */
        if (dev_pci_bits_from_fields(t))
            return t;
    }

    /* If no device were found, return an empty one */
    return dev_zero;
}

/*
 * @brief scan a function, looking for a device with matching vendor_id and device_id.
 * Function called by dev_pci_scan_device
 * 
 * @return a pci_dev struct containing all needed information if the device has
 * been found. An empty pci_dev struct otherwise.
 */
PRIVATE struct pci_dev dev_pci_scan_function(uint16_t vendor_id, uint16_t device_id, uint32_t bus, uint32_t device, uint32_t function, uint32_t device_type)
{
    /* Create the pci_dev corresponding to the current bus, device and function */
    struct pci_dev dev = {0};
    dev.bus_num = bus;
    dev.device_num = device;
    dev.function_num = function;

    /* If it's a PCI Bridge device, get the bus it's connected to and keep searching */
    if (dev_pci_get_device_type(dev) == PCI_TYPE_BRIDGE)
    {
        dev_pci_scan_bus(vendor_id, device_id, dev_pci_read(dev, PCI_SECONDARY_BUS), device_type);
    }

    /* Check if device_type match */
    if (device_type == (uint32_t)-1 || device_type == dev_pci_get_device_type(dev))
    {
        uint32_t devid = dev_pci_read(dev, PCI_DEVICE_ID);
        uint32_t vendid = dev_pci_read(dev, PCI_VENDOR_ID);
        /* Check if DeviceID and VendorID match */
        if (devid == device_id && vendor_id == vendid)
            return dev; /* The device has been found */
    }

    /* The device is not the one we're looking for, return an empty one */
    return dev_zero;
}

/**
 * @brief return the type of the device. All device types are listed here :
 * https://wiki.osdev.org/PCI#Class_Codes
 */
PRIVATE uint32_t dev_pci_get_device_type(struct pci_dev dev)
{
    uint32_t t = dev_pci_read(dev, PCI_CLASS) << 8;
    return t | dev_pci_read(dev, PCI_SUBCLASS);
}

/**
 * @brief Convert all pci_dev fields into one 32 bits uint32_t
 */
PRIVATE uint32_t dev_pci_bits_from_fields(struct pci_dev dev) {
    uint32_t bits = 0;

    bits |= (dev.enable) << 31;
    bits |= (dev.reserved) << 24;
    bits |= (dev.bus_num) << 16;
    bits |= (dev.device_num) << 11;
    bits |= (dev.function_num) << 8;
    bits |= (dev.register_offset & 0xFC); /* 2 first bits at zero */
    
    return bits;
}

/**
 * @brief Given a field, return its size. (Previously achieved using an associative table, better with a switch for less memory usage)
 */
PRIVATE uint32_t pci_size_map(uint32_t field) {
    switch (field)
    {
        case PCI_SUBCLASS:
        case PCI_CLASS:
        case PCI_CACHE_LINE_SIZE:
        case PCI_LATENCY_TIMER:
        case PCI_HEADER_TYPE:
        case PCI_BIST:
        case PCI_INTERRUPT_LINE:
        case PCI_SECONDARY_BUS:
            return 1;
        case PCI_VENDOR_ID:
        case PCI_DEVICE_ID:
        case PCI_COMMAND:
        case PCI_STATUS:
            return 2;    
        case PCI_BAR0:
        case PCI_BAR1:
        case PCI_BAR2:
        case PCI_BAR3:
        case PCI_BAR4:
        case PCI_BAR5:
            return 4;
        default:
            return 0;
    }
}