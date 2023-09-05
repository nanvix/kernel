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
extern int kmod_register(vaddr_t start, vaddr_t end, const char *cmdline);

/**
 * @brief Returns the number of registered kernel modules.
 *
 * @returns The number of registered kernel modules.
 */
extern unsigned kmod_count(void);

/**
 * @brief Returns the start address of a kernel module
 *
 * @param i Index of the target kernel module.
 * @param start Storage locations for the start address of the target module.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int kmod_get_start(unsigned i, vaddr_t *start);

/**
 * @brief Returns the end address of a kernel module
 *
 * @param i Index of the target kernel module.
 * @param end Storage locations for the end address of the target module.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int kmod_get_end(unsigned i, vaddr_t *end);

/**
 * @brief Returns the command line of a kernel module
 *
 * @param i Index of the target kernel module.
 * @param cmdline Storage locations for the command line of the target module.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int kmod_get_cmdline(unsigned i, char *cmdline);

#endif /* NANVIX_KERNEL_KMOD_H_ */
