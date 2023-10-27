/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
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
    // TODO: Check for permissions.

    // Create underlying virtual address space
    const vmem_t vmem = vmem_create();

    return (vmem);
}

/**
 * @details Removes a virtual memory space.
 */
int kcall_vmremove(vmem_t vmem)
{
    // TODO: Check for permissions.

    // Issue underlying operation.
    int ret = vmem_destroy(vmem);

    return (ret);
}

/**
 * @details Maps a page frame at a virtual address.
 */
int kcall_vmmap(vmem_t vmem, vaddr_t vaddr, frame_t frame)
{
    // TODO: Check for permissions.

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

    // TODO: implement.
    return (frame);
}

/**
 * @details Manipulates various parameters from a virtual memory space.
 */
int kcall_vmctrl(vmem_t vmem, unsigned request, unsigned arg0, unsigned arg1)
{
    // Check for invalid virtual memory space.
    if (vmem == VMEM_NULL) {
        return (-1);
    }

    // TODO: Check for permissions.

    // Parse request.
    switch (request) {
        case VMEM_CHMOD:
            vmem_ctrl(vmem, arg0, arg1 & VMEM_WRITE, arg1 & VMEM_EXEC);
            break;

        default:
            break;
    }

    // TODO: implement.
    return (-1);
}
