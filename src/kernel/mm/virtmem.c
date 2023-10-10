/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include "mod.h"
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/mm.h>

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

/**
 * @brief Virtual memory.
 */
struct virtmem {
    struct pde *pgdir; /** Underlying page directory*/
};

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Initializes a virtual memory.
 */
int virtmem_init(struct virtmem *virtmem, const struct pde *pgdir)
{
    // Check if page directory is valid.
    if (pgdir == NULL) {
        kprintf("[mm] ERROR: invalid page directory");
        return (-1);
    }

    // Check if storage location is valid.
    if (virtmem == NULL) {
        kprintf("[mm] ERROR: invalid storage location");
        return (-1);
    }

    virtmem->pgdir = (struct pde *)pgdir;

    return (0);
}

/**
 * @details Creates a virtual Memory.
 */
int virtmem_create(struct virtmem *virtmem, const struct pde *src_pgdir)
{
    // Check if page directory is valid.
    if (src_pgdir == NULL) {
        kprintf("[mm] ERROR: invalid page directory");
        return (-1);
    }

    // Check if storage location is valid.
    if (virtmem == NULL) {
        kprintf("[mm] ERROR: invalid storage location");
        return (-1);
    }

    struct pde *new_pgdir = kpage_get(true);

    // Check if we succeeded to allocate a new page directory.
    if (new_pgdir == NULL) {
        kprintf("[mm] ERROR: failed to allocate a new page directory");
        return (-1);
    }

    // Link all pages from source page directory to new page directory.
    for (int i = 0; i < PGDIR_LENGTH; i++) {
        if (pde_is_present(&src_pgdir[i])) {
            __memcpy(&new_pgdir[i], &src_pgdir[i], sizeof(struct pde));
        }
    }

    virtmem->pgdir = new_pgdir;

    return (0);
}

/**
 * @brief Attaches a stack to a virtual memory.
 */
int virtmem_attach_stack(struct virtmem *virtmem, vaddr_t addr)
{
    // Check if storage location is valid.
    if (virtmem == NULL) {
        kprintf("[mm] ERROR: invalid storage location");
        return (-1);
    }

    KASSERT(upage_alloc(virtmem->pgdir, addr, true, false) == 0);

    return (0);
}

/**
 * @details Prints a virtual memory.
 */
void virtmem_print(const struct virtmem *virtmem)
{
    // Print all present entries.
    for (int i = 0; i < PGDIR_LENGTH; i++) {
        if (pde_is_present(&virtmem->pgdir[i])) {
            kprintf(
                "INFO: pde=%d, frame=%x", i, pde_frame_get(&virtmem->pgdir[i]));
        }
    }
}
