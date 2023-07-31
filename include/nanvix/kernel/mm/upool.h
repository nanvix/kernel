/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_MM_UPOOL_H_
#define NANVIX_KERNEL_MM_UPOOL_H_

/**
 * @addtogroup kernel-mm-upage Page Allocator
 * @ingroup kernel-mm
 *
 * @brief User Page Allocator
 */
/**@{*/

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Number of pages for user use
 */
#define NUM_UPAGES NUM_UFRAMES

/*============================================================================*
 * Variables                                                                  *
 *============================================================================*/

/**
 * @brief Invalidates HW references to a user page.
 *
 * @param vaddr Target virtual address.
 */
extern int upage_inval(vaddr_t vaddr);

/**
 * @brief Maps a page frame into a page.
 *
 * @param pgdir Target page directory.
 * @param vaddr Target virtual address.
 * @param frame Target page frame.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 *
 * @see upage_unmap().
 */
extern int upage_map(struct pde *pgdir, vaddr_t vaddr, frame_t frame);

/**
 * @brief Unmaps a page frame.
 *
 * @param pgdir Target page directory.
 * @param vaddr Target virtual address.
 *
 * @returns Upon successful completion, the number of the frame
 * that was previously mapped to the target page is returned. Upon
 * failure, @p FRAME_NULL is returned instead.
 *
 * @see upage_map().
 */
extern frame_t upage_unmap(struct pde *pgdir, vaddr_t vaddr);

/**
 * @brief Allocates a user page.
 *
 * @param pgdir Target page directory.
 * @param vaddr Target virtual address.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 *
 * @see upage_free().
 */
extern int upage_alloc(struct pde *pgdir, vaddr_t vaddr);

/**
 * @brief Releases a user page.
 *
 * @param pgdir Target page directory.
 * @param vaddr Target virtual address.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 *
 * @see upage_alloc().
 */
extern int upage_free(struct pde *pgdir, vaddr_t vaddr);

/**
 * @brief Links two pages.
 *
 * @param pgdir Target page directory.
 * @param vaddr1 Virtual address of source page.
 * @param vaddr2 Virtual address of target page.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern int upage_link(struct pde *pgdir, vaddr_t vaddr1, vaddr_t vaddr2);

/**
 * @brief Runs unit tests on the user page allocator.
 */
extern void upool_test_driver(void);

/**
 * @brief Initializes the user page allocator.
 */
extern void upool_init(void);

/**
 * @brief Root page directory.
 */
extern struct pde root_pgdir[];

/*============================================================================*/

/**@}*/

#endif /* NANVIX_KERNEL_MM_UPOOL_H_ */
