/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_MMAP_H_
#define NANVIX_KERNEL_HAL_MMAP_H_

/**
 * @addtogroup kernel-hal-mmap Memory Map
 * @ingroup kernel-mmap
 *
 * @brief Memory Map Module
 */
/**@{*/

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal/arch.h>

/*============================================================================*
 * Enumerations                                                               *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Type of a memory map entry.
 */
enum mmap_entry_type {
    MMAP_ENTRY_AVAILABLE = 1, /** Available memory.      */
    MMAP_ENTRY_RESERVED,      /** Reserved memory.       */
    MMAP_ENTRY_BADRAM,        /** Malfunctioning memory. */
};

#endif /* !_ASM_FILE_ */

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief An entry in the memory map.
 */
struct mmap_entry {
    paddr_t base;              /** Base address.    */
    size_t size;               /** Size (in bytes). */
    enum mmap_entry_type type; /** Memory type.     */
};

#endif /* !_ASM_FILE_ */

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Registers a memory region that is available for use.
 *
 * @param base Base address of the memory region.
 * @param size Size of the memory region (in bytes).
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int mmap_register_available(paddr_t base, size_t size);

/**
 * @brief Registers a memory region that is reserved for use.
 *
 * @param base Base address of the memory region.
 * @param size Size of the memory region (in bytes).
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int mmap_register_reserved(paddr_t base, size_t size);

/**
 * @brief Registers a memory region that is malfunctioning.
 *
 * @param base Base address of the memory region.
 * @param size Size of the memory region (in bytes).
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int mmap_register_erroneous(paddr_t base, size_t size);

/**
 * @brief Returns the number of entries in the memory map.
 *
 * @return The number of entries in the memory map.
 */
extern unsigned mmap_count(void);

/**
 * @brief Gets a memory map entry.
 *
 * @param entry Storage location for the target entry.
 * @param index Index the target entry in the memory map.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int mmap_get(struct mmap_entry *entry, unsigned index);

/**
 * @brief Prints the contents of the memory map.
 */
extern void mmap_print(void);

/**
 * @brief Initializes physical memory.
 */
extern void mem_init(void);

#endif /* !_ASM_FILE_ */

/*============================================================================*/

/**@}*/

#endif /* NANVIX_KERNEL_HAL_MMAP_H_ */
