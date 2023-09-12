/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_KMOD_H_
#define NANVIX_KERNEL_KMOD_H_

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Maximum length for a kernel module command line.
 */
#define KMOD_CMDLINE_MAX 64

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

/**
 * @brief Kernel module.
 */
struct kmod {
    paddr_t start;                  /**< Start address. */
    paddr_t end;                    /**< End address.   */
    char cmdline[KMOD_CMDLINE_MAX]; /**< Command line.  */
};

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @brief Registers a kernel module.
 *
 * @param start   Start address of the module.
 * @param end     End address of the module.
 * @param cmdline Command line arguments for the module.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int kmod_register(paddr_t start, paddr_t end, const char *cmdline);

/**
 * @brief Returns the number of registered kernel modules.
 *
 * @returns The number of registered kernel modules.
 */
extern unsigned kmod_count(void);

/**
 * @brief Retrieves information of a kernel module.
 *
 * @param info  Storage location for the target kernel module information.
 * @param index Target kernel module.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative error code is returned instead.
 */
extern int kmod_get(struct kmod *info, unsigned index);

/**
 * @brief Prints kernel modules table.extern void kmod_print(void);
 */
extern void kmod_print(void);

#endif /* NANVIX_KERNEL_KMOD_H_ */
