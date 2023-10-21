/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_MM_VMEM_H_
#define NANVIX_KERNEL_MM_VMEM_H_

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>

/*============================================================================*
 * Opaque Structures                                                          *
 *============================================================================*/

/**
 * @brief Virtual Memory Space
 */
struct vmem;

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Returns the underlying page directory of a virtual memory space.
 *
 * @deprecated
 */
extern const struct pde *vmem_pgdir_get(const struct vmem *vmem);

/**
 * @brief Creates a virtual memory space.
 *
 * @param src_vmem Source virtual memory space.
 *
 * @returns Upon successful completion, a pointer to the newly allocated virtual
 * memory space is returned. On failure, a null pointer is returned instead.
 */
extern struct vmem *vmem_create(const struct vmem *src_vmem);

/**
 * @brief Destroys a virtual memory space.
 *
 * @param vmem Target virtual memory space.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int vmem_destroy(struct vmem *vmem);

/**
 * @brief Attaches a virtual address range to a virtual memory space.
 *
 * @param vmem Target virtual memory space.
 * @param addr Address where the stack should be attached.
 * @param size Size of virtual address range.
 *
 * @return Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int vmem_attach(struct vmem *vmem, vaddr_t addr, size_t size);

/**
 * @brief Maps a virtual address range into a virtual memory space.
 *
 * @param vmem Target virtual memory space.
 * @param vaddr Target virtual address.
 * @param paddr Target physical address.
 * @param size Size of virtual address range.
 * @param w Write permission.
 * @param x Execute permission.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int vmem_map(struct vmem *vmem, vaddr_t vaddr, paddr_t paddr,
                    size_t size, bool w, bool x);

/**
 * @brief Prints a virtual memory space.
 *
 * @param vmem Target virtual memory space.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
extern int vmem_print(const struct vmem *vmem);

/**
 * @brief Initializes the virtual memory manager.
 *
 * @param root_pgdir Root page directory.
 *
 * @returns A pointer to the root virtual memory space is returned. If this
 * function does not succeed, the kernel panics.
 */
extern const struct vmem *vmem_init(const struct pde *root_pgdir);

/*============================================================================*/

#endif /* NANVIX_KERNEL_MM_VMEM_H_ */
