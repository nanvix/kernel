/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_MM_KPOOL_H_
#define NANVIX_KERNEL_MM_KPOOL_H_

/**
 * @addtogroup kernel-mm-kpool Kernel Pool
 * @ingroup kernel-mm
 *
 * @brief Kernel Page Pool
 *
 * The Kernel Page Pool is a subsystem of the Memory Management (MM)
 * system that maintains a pool of pages for kernel use. Overall, this
 * subsystem exposes an interface for allocating and releasing kernel
 * pages, thereby providing the bare-bones for dynamic memory
 * allocation in kernel land.
 *
 * Page frames for the Kernel Page Pool are reserved at system
 * startup. These frames are contiguously located in the physical
 * memory area, but the actual location and the size of this area  is
 * platform dependent, thus being safely accessed through the Hardware
 * Abstraction Layer (HAL).
 */
/**@{*/

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/mm/memory.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Number of pages for kernel use.
 */
#define NUM_KPAGES (KPOOL_SIZE / PAGE_SIZE)

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Asserts a kernel page address.
 *
 * The kpool_is_kpage() function asserts whether or not the
 * virtual address @p vaddr refers to a kernel page.
 *
 * @returns If @p vaddr refers to a kernel page, non zero is
 * returned. Otherwise, zero is returned instead.
 *
 * @author Pedro Henrique Penna
 */
static inline int kpool_is_kpage(vaddr_t vaddr)
{
    return ((vaddr >= KPOOL_BASE_VIRT) &&
            (vaddr < KPOOL_BASE_VIRT + KPOOL_SIZE));
}

/**
 * @brief Translates a kernel page ID into a virtual address.
 *
 * The kpool_id_to_addr() function converts the kernel page ID @p
 * id into a virtual address.
 *
 * @param id ID of target kernel page.
 *
 * @returns The virtual address of the target kernel page.
 *
 * @note This function expects that @p is valid.
 *
 * @author Pedro Henrique Penna
 */
static inline vaddr_t kpool_id_to_addr(unsigned id)
{
    return (KPOOL_BASE_VIRT + (id << PAGE_SHIFT));
}

/**
 * @brief Translates a frame number into a virtual address.
 *
 * The kpool_frame_to_addr() function converts the frame number @p
 * frame into a virtual address.
 *
 * @param frame Target frame.
 *
 * @returns The virtual address of the target frame.
 *
 * @note This function expects that @p frame is valid.
 *
 * @author Pedro Henrique Penna
 */
static inline vaddr_t kpool_frame_to_addr(frame_t frame)
{
    return (kpool_id_to_addr(frame - (KPOOL_BASE_PHYS >> PAGE_SHIFT)));
}

/**
 * @brief Translates a virtual address into a kernel page ID.
 *
 * The kpool_addr_to_id() function converts the virtual address @p
 * vaddr into a kernel page ID.
 *
 * @param vaddr Target virtual address.
 *
 * @returns The kernel page ID of the target virtual address.
 *
 * @note This function expects that @p vaddr is valid.
 *
 * @author Pedro Henrique Penna
 */
static inline int kpool_addr_to_id(vaddr_t vaddr)
{
    return ((vaddr - KPOOL_BASE_VIRT) >> PAGE_SHIFT);
}

/**
 * @brief Translates a kernel page into a frame number.
 *
 * The kpool_addr_to_frame() function converts the virtual address
 * @p vaddr into a frame number.
 *
 * @param vaddr Target virtual address.
 *
 * @returns The frame number of a kernel page.
 *
 * @note This function expects that @p vaddr is valid.
 *
 * @author Pedro Henrique Penna
 */
static inline frame_t kpool_addr_to_frame(vaddr_t vaddr)
{
    return (kpool_addr_to_id(vaddr) + (KPOOL_BASE_PHYS >> PAGE_SHIFT));
}

/**
 * @brief Allocates a kernel page.
 *
 * @param clean Should the page be cleaned?
 *
 * @returns Upon successful completion, a pointer to a kernel page
 * is returned. Upon failure, a @p NULL pointer is returned
 * instead.
 */
extern void *kpage_get(int clean);

/**
 * @brief Releases kernel page.
 *
 * @param kpg Kernel page to be released.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern int kpage_put(void *kpg);

/**
 * @brief Initializes the kernel page pool.
 */
extern void kpool_init(void);

/*============================================================================*/

/**@}*/

#endif /* NANVIX_KERNEL_MM_KPOOL_H_ */
