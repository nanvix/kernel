/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_MM_VIRTMEM_H_
#define NANVIX_KERNEL_MM_VIRTMEM_H_

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>

/*============================================================================*
 * Opaque Structures                                                          *
 *============================================================================*/

struct virtmem;

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Creates a virtual memory.
 *
 * @param virtmem target virtual memory.
 * @param src_pgdir Source page directory.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int virtmem_create(struct virtmem *virtmem, const struct pde *src_pgdir);

/**
 * @brief Initializes a virtual memory.
 *
 * @param virtmem Target virtual memory.
 * @param pgdir Underlying page directory.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int virtmem_init(struct virtmem *virtmem, const struct pde *src_pgdir);

/**
 * @brief Attaches a stack to a virtual memory.
 *
 * @param virtmem Target virtual memory
 * @param addr Address where the stack should be attached.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int virtmem_attach_stack(struct virtmem *virtmem, vaddr_t addr);

/**
 * @brief Prints a virtual memory.
 *
 * @param virtmem Target virtual memory
 */
extern void virtmem_print(const struct virtmem *virtmem);

/*============================================================================*/

#endif /* NANVIX_KERNEL_MM_VIRTMEM_H_ */
