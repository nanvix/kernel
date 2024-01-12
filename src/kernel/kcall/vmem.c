/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/kcall.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/mm.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Creates a virtual memory space.
 */
vmem_t kcall_vmcreate(void)
{
    // TODO: https://github.com/nanvix/microkernel/issues/367

    // Create underlying virtual address space
    const vmem_t vmem = vmem_create();

    return (vmem);
}

/**
 * @details Removes a virtual memory space.
 */
int kcall_vmremove(vmem_t vmem)
{
    // TODO: https://github.com/nanvix/microkernel/issues/367

    // Issue underlying operation.
    int ret = vmem_destroy(vmem);

    return (ret);
}

/**
 * @details Maps a page frame at a virtual address.
 */
int kcall_vmmap(vmem_t vmem, vaddr_t vaddr, frame_t frame)
{
    // TODO: https://github.com/nanvix/microkernel/issues/367

    // Check for invalid page frame.
    if (((frame << PAGE_SHIFT) < USER_BASE_PHYS) ||
        ((frame << PAGE_SHIFT) >= USER_END_PHYS)) {
        kprintf("ERROR: invalid page frame (frame=%x)", frame);
        return (-1);
    }

    // Issue underlying operation.
    int ret = vmem_map(vmem, vaddr, frame, PAGE_SIZE, false, false);

    return (ret);
}

/**
 * @details Unmaps a page frame from a virtual address.
 */
frame_t kcall_vmunmap(vmem_t vmem, vaddr_t vaddr)
{
    // Check for invalid virtual address.
    if ((vaddr < USER_BASE_VIRT) || (vaddr >= USER_END_VIRT)) {
        return (-1);
    }

    // Issue underlying operation.
    frame_t frame = vmem_unmap(vmem, vaddr);

    return (frame);
}

/**
 * @details Manipulates various parameters from a virtual memory space.
 */
int kcall_vmctrl(vmem_t vmem, unsigned request, vaddr_t vaddr, mode_t mode)
{
    int ret = -1;

    // TODO: https://github.com/nanvix/microkernel/issues/367

    // Parse request.
    switch (request) {
        case VMEM_CHMOD:
            ret = vmem_ctrl(vmem, vaddr, mode);
            break;

        default:
            kprintf("ERROR: invalid request (request=%x)", request);
            break;
    }

    return (ret);
}

/**
 * @details Gets information on a page.
 */
int kcall_vminfo(vmem_t vmem, vaddr_t vaddr, struct pageinfo *buf)
{
    // Check for invalid virtual address.
    if ((vaddr < USER_BASE_VIRT) || (vaddr >= USER_END_VIRT)) {
        return (-1);
    }

    // TODO: check if storage location is valid.

    // Issue underlying operation.
    int ret = vmem_info(vmem, vaddr, buf);

    return (ret);
}
