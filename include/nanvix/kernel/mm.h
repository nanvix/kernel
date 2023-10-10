/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_MM_H_
#define NANVIX_KERNEL_MM_H_

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/mm/frame.h>
#include <nanvix/kernel/mm/kpool.h>
#include <nanvix/kernel/mm/memory.h>
#include <nanvix/kernel/mm/upool.h>
#include <nanvix/kernel/mm/virtmem.h>

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

/**
 * @brief Asserts a user virtual address.
 *
 * The mm_is_uaddr() function asserts whether or not the virtual
 * address @p vaddr lies in user space.
 *
 * @returns If @p vaddr lies in user space, non zero is returned.
 * Otherwise, zero is returned instead.
 */
static inline int mm_is_uaddr(vaddr_t vaddr)
{
    return ((vaddr >= USER_BASE_VIRT) &&
            (vaddr < (USER_BASE_VIRT + UMEM_SIZE)));
}

/**
 * @brief Asserts a kernel virtual address.
 *
 * The mm_is_kaddr() function asserts whether or not the virtual
 * address @p vaddr lies in kernel space.
 *
 * @returns If @p vaddr lies in kernel space, non zero is
 * returned. Otherwise, zero is returned instead.
 */
static inline int mm_is_kaddr(vaddr_t vaddr)
{
    return ((vaddr < (KERNEL_BASE_VIRT + KMEM_SIZE)) ||
            ((vaddr >= KPOOL_BASE_VIRT) &&
             (vaddr < (KPOOL_BASE_VIRT + KPOOL_SIZE))));
}

/**
 * @brief Checks access permissions to a memory area.
 *
 * @param addr Address to be checked.
 * @param size Size of memory area.
 * @param area User memory area.
 *
 * @returns Non-zero if access is authorized, and zero otherwise.
 */
static inline int mm_check_area(vaddr_t vaddr, uint64_t size, int area)
{

    return ((area == UMEM_AREA)
                ? mm_is_uaddr(vaddr) && mm_is_uaddr(vaddr + size)
                : mm_is_kaddr(vaddr) && mm_is_kaddr(vaddr + size));
}

/**
 * @brief Initializes the Memory Management (MM) system.
 *
 * @returns The root page directory.
 */
extern const void *mm_init(void);

/*============================================================================*/

#endif /* NANVIX_KERNEL_MM_H_ */
