/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/kmod.h>
#include <nanvix/kernel/mm.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Gets information about a kernel module.
 */
int kcall_kmod_get(struct kmod *kmod, unsigned index)
{
    struct kmod _kmod = {0};

    // Check if storage location is valid.
    if (kmod == NULL) {
        return (-1);
    }

    // Check if storage location lies in user space.
    if (!mm_check_area(VADDR(kmod), sizeof(struct kmod), UMEM_AREA)) {
        return (-1);
    }

    // Retrieve information on kernel module.
    if (kmod_get(&_kmod, index) < 0) {
        return (-1);
    }

    // TODO: perform a safe copy to user space.
    __memcpy(kmod, &_kmod, sizeof(struct kmod));

    return (0);
}
