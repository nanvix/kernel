/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include "mod.h"
#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/log.h>
#include <stddef.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Maximum number of entries in the memory map.
 */
#define MMAP_MAX_ENTRIES 16

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Memory map.
 */
static struct {
    unsigned num_entries;                        /** Number of entries. */
    struct mmap_entry entries[MMAP_MAX_ENTRIES]; /** Entries            */
} mmap;

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Returns the corresponding string to a memory entry type.
 *
 * @param type Memory entry type.
 *
 * @returns The corresponding string to a memory entry type.
 */
static const char *mmap_entry_type_str(enum mmap_entry_type type)
{
    switch (type) {
        case MMAP_ENTRY_AVAILABLE:
            return ("available");
        case MMAP_ENTRY_BADRAM:
            return ("malfunctioning");
        default:
            return ("reserved");
    }
}

/**
 * @brief Registers a memory region.
 *
 * @param base Base address of the memory region.
 * @param size Size of the memory region (in bytes).
 * @param type Type of the memory region.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
static int mmap_register(paddr_t base, size_t size, enum mmap_entry_type type)
{
    // Check for invalid size.
    if (size == 0) {
        error("invalid size (size=%d)", size);
        return (-1);
    }

    // Check if too many regions are already registered.
    if (mmap.num_entries >= MMAP_MAX_ENTRIES) {
        error("too many regions registered");
        return (-1);
    }

    // Check if new region would overlap with one that is already registered.
    for (unsigned i = 0; i < mmap.num_entries; i++) {
        if ((base >= mmap.entries[i].base) &&
            (base < (mmap.entries[i].base + mmap.entries[i].size))) {
            error("new region would overlap with another "
                  "one (base=%x, size=%d)",
                  base,
                  size);
            return (-1);
        }

        if (((base + size) > mmap.entries[i].base) &&
            ((base + size) <= (mmap.entries[i].base + mmap.entries[i].size))) {
            error("new region would overlap with "
                  "another one (base=%x, size=%d)",
                  base,
                  size);
            return (-1);
        }
    }

    info("registering region (base=%x, size=%d, type=%s)",
         base,
         size,
         mmap_entry_type_str(type));

    // Register memory region.
    mmap.entries[mmap.num_entries].base = (paddr_t)base;
    mmap.entries[mmap.num_entries].size = size;
    mmap.entries[mmap.num_entries].type = type;
    mmap.num_entries++;

    return (0);
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @brief Registers a memory region that is available for use.
 *
 * @param base Base address of the memory region.
 * @param size Size of the memory region (in bytes).
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
int mmap_register_available(paddr_t base, size_t size)
{
    return (mmap_register(base, size, MMAP_ENTRY_AVAILABLE));
}

/**
 * @brief Registers a memory region that is reserved for use.
 *
 * @param base Base address of the memory region.
 * @param size Size of the memory region (in bytes).
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
int mmap_register_reserved(paddr_t base, size_t size)
{
    return (mmap_register(base, size, MMAP_ENTRY_RESERVED));
}

/**
 * @brief Registers a memory region that is malfunctioning.
 *
 * @param base Base address of the memory region.
 * @param size Size of the memory region (in bytes).
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
int mmap_register_erroneous(paddr_t base, size_t size)
{
    return (mmap_register(base, size, MMAP_ENTRY_BADRAM));
}

/**
 * @brief Returns the number of entries in the memory map.
 *
 * @return The number of entries in the memory map.
 */
unsigned mmap_count(void)
{
    return (mmap.num_entries);
}

/**
 * @brief Gets a memory map entry.
 *
 * @param entry Storage location for the target entry.
 * @param index Index the target entry in the memory map.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
int mmap_get(struct mmap_entry *entry, unsigned index)
{
    // Check for invalid storage location.
    if (entry == NULL) {
        kprintf(MODULE_NAME "ERROR: invalid storage location");
        return (-1);
    }

    // Check for invalid index.
    if (index >= mmap.num_entries) {
        kprintf(MODULE_NAME "ERROR: invalid index (index=%d)", index);
        return (-1);
    }

    // Get memory map entry.
    entry->base = mmap.entries[index].base;
    entry->size = mmap.entries[index].size;
    entry->type = mmap.entries[index].type;

    return (0);
}

/**
 * @brief Prints the contents of the memory map.
 */
void mmap_print(void)
{
    kprintf("Memory Map:");

    for (unsigned i = 0; i < mmap.num_entries; i++) {
        kprintf(MODULE_NAME " INFO: base=%x, size=%d, type=%s",
                mmap.entries[i].base,
                mmap.entries[i].size,
                mmap_entry_type_str(mmap.entries[i].type));
    }
}
