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
#include <nanvix/types.h>
#include <stdbool.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Maximum number of virtual memory spaces.
 *
 * @todo Move this to a <config.h> header file.
 */
#define VMEM_MAX 16

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

/**
 * @details Virtual memory space.
 */
struct vmem {
    bool used;         /** Used?                     */
    struct pde *pgdir; /** Underlying page directory */
};

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Table of virtual memory spaces.
 */
static struct vmem vmem_table[VMEM_MAX];

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Checks if a virtual memory space is valid.
 *
 * @param vmem Target virtual memory space.
 */
static bool vmem_is_valid(vmem_t vmem)
{
    return ((vmem >= 0) && (vmem < VMEM_MAX) && (vmem_table[vmem].used));
}

/**
 * @brief Allocates a virtual memory space.
 *
 * @returns Upon successful completion, a pointer to the newly allocated virtual
 * memory space is returned. On failure, a null pointer is returned instead.
 */
static vmem_t vmem_alloc(void)
{
    // Traverse all entries in the table of virtual memory spaces.
    for (int i = 0; i < VMEM_MAX; i++) {
        // Check if entry is free.
        if (!vmem_table[i].used) {
            // It is, thus allocate and return it.
            vmem_table[i].used = true;
            return (i);
        }
    }

    // No free entry was found.
    kprintf(MODULE_NAME " ERROR: no more virtual memory spaces are available");
    return (VMEM_NULL);
}

/**
 * @brief Releases a virtual memory space.
 *
 * @param vmem Target virtual memory space.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
static int vmem_free(vmem_t vmem)
{
    // Check for valid virtual memory space.
    if (!vmem_is_valid(vmem)) {
        kprintf(MODULE_NAME " ERROR: invalid virtual memory space");
        return (-1);
    }

    // Release the target virtual memory space.
    vmem_table[vmem].used = false;

    return (0);
}

/**
 * @brief Returns a pointer to the root virtual memory space.
 *
 * @returns A pointer to the root virtual memory space is returned.
 */
static vmem_t vmem_get_root(void)
{
    return (0);
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Returns the underlying page directory of a virtual memory space.
 */
const struct pde *vmem_pgdir_get(vmem_t vmem)
{
    // Check for valid virtual memory space.
    if (!vmem_is_valid(vmem)) {
        kprintf(MODULE_NAME " ERROR: invalid virtual memory space");
        return (NULL);
    }

    return (vmem_table[vmem].pgdir);
}

/**
 * @details Creates a virtual memory space.
 */
vmem_t vmem_create(void)
{
    vmem_t src_vmem = vmem_get_root();

    // Allocate an entry in the table of virtual memory spaces.
    vmem_t vmem = vmem_alloc();
    if (vmem == VMEM_NULL) {
        goto error0;
    }

    // Allocate a page directory.
    struct pde *new_pgdir = kpage_get(true);
    if (new_pgdir == NULL) {
        goto error1;
    }

    // Link all pages from source page directory to new page directory.
    for (int i = 0; i < PGDIR_LENGTH; i++) {
        const struct pde *src_pgdir = vmem_table[src_vmem].pgdir;
        if (pde_is_present(&src_pgdir[i])) {
            __memcpy(&new_pgdir[i], &src_pgdir[i], sizeof(struct pde));
        }
    }

    // Initialize virtual memory space.
    vmem_table[vmem].pgdir = new_pgdir;

    return (vmem);

error1:
    vmem_free(vmem);
error0:
    return (VMEM_NULL);
}

/**
 * @details Destroys a virtual memory space.
 */
int vmem_destroy(vmem_t vmem)
{
    int ret = 0;

    // Check for valid virtual memory space.
    if (!vmem_is_valid(vmem)) {
        kprintf(MODULE_NAME " ERROR: invalid virtual memory space");
        return (-1);
    }

    // Check for root virtual memory space.
    if (vmem == vmem_get_root()) {
        kprintf(MODULE_NAME " ERROR: cannot destroy root virtual memory space");
        return (-1);
    }

    // Check if the target virtual memory is busy.
    for (unsigned i = pde_idx_get(USER_BASE_VIRT);
         i < pde_idx_get(USER_END_VIRT);
         i++) {
        if (pde_is_present(&vmem_table[vmem].pgdir[i])) {
            kprintf(MODULE_NAME " ERROR: virtual memory space is busy");
            return (-1);
        }
    }

    // Release the page directory.
    kpage_put(vmem_table[vmem].pgdir);
    vmem_table[vmem].pgdir = NULL;

    // Release the target virtual memory space.
    if ((ret = vmem_free(vmem)) != 0) {
        return (ret);
    }

    return (ret);
}

/**
 * @details Changes access permissions of a user page.
 */
int vmem_ctrl(vmem_t vmem, vaddr_t vaddr, mode_t mode)
{
    // Check for valid virtual memory space.
    if (!vmem_is_valid(vmem)) {
        kprintf(MODULE_NAME " ERROR: invalid virtual memory space");
        return (-1);
    }

    return (upage_ctrl(vmem_table[vmem].pgdir, vaddr, mode));
}

/**
 * @brief Gets information on a user page.
 */
int vmem_info(vmem_t vmem, vaddr_t vaddr, struct pageinfo *buf)
{
    // Check for valid virtual memory space.
    if (!vmem_is_valid(vmem)) {
        kprintf(MODULE_NAME " ERROR: invalid virtual memory space");
        return (-1);
    }

    return (upage_info(vmem_table[vmem].pgdir, vaddr, buf));
}

/**
 * @details Attaches a virtual address range to a virtual memory space.
 */
int vmem_attach(vmem_t vmem, vaddr_t addr, size_t size)
{
    // Check for valid virtual memory space.
    if (!vmem_is_valid(vmem)) {
        kprintf(MODULE_NAME " ERROR: invalid virtual memory space");
        return (-1);
    }

    // Check if size is supported.
    if (size != PAGE_SIZE) {
        kpanic("unsupport range size");
    }

    KASSERT(upage_alloc(vmem_table[vmem].pgdir, addr, true, false) == 0);

    return (0);
}

/**
 * @details Maps a virtual address range into a virtual memory space.
 */
int vmem_map(vmem_t vmem, vaddr_t vaddr, frame_t frame, size_t size, bool w,
             bool x)
{
    // Check for valid virtual memory space.
    if (!vmem_is_valid(vmem)) {
        kprintf(MODULE_NAME " ERROR: invalid virtual memory space");
        return (-1);
    }

    // Check if size is supported.
    if (size != PAGE_SIZE) {
        kpanic("unsupport range size");
    }

    return (upage_map(vmem_table[vmem].pgdir, vaddr, frame, w, x));
}

/**
 * @details Unmaps a page frame from a virtual address.
 */
frame_t vmem_unmap(vmem_t vmem, vaddr_t vaddr)
{
    // Check for valid virtual memory space.
    if (!vmem_is_valid(vmem)) {
        kprintf(MODULE_NAME " ERROR: invalid virtual memory space");
        return (-1);
    }

    return (upage_unmap(vmem_table[vmem].pgdir, vaddr));
}

/**
 * @details Prints a virtual memory space.
 */
int vmem_print(vmem_t vmem)
{
    // Check for valid virtual memory space.
    if (!vmem_is_valid(vmem)) {
        kprintf(MODULE_NAME " ERROR: invalid virtual memory space");
        return (-1);
    }

    // Print all present entries.
    for (int i = 0; i < PGDIR_LENGTH; i++) {
        if (pde_is_present(&vmem_table[vmem].pgdir[i])) {
            kprintf(MODULE_NAME " INFO: pde=%d, frame=%x",
                    i,
                    pde_frame_get(&vmem_table[vmem].pgdir[i]));
        }
    }

    return (0);
}

/**
 * @details Initializes the virtual memory manager.
 */
vmem_t vmem_init(const struct pde *root_pgdir)
{
    static bool initialized = false;

    // Check if the virtual memory manager was already initialized.
    if (initialized) {
        kpanic("[mm] virtual memory manager was already initialized");
    }

    kprintf(MODULE_NAME "initializing the virtual memory manager...");

    for (int i = 1; i < VMEM_MAX; i++) {
        vmem_table[i].used = false;
        vmem_table[i].pgdir = NULL;
    }

    vmem_table[0].used = true;
    vmem_table[0].pgdir = (struct pde *)root_pgdir;

    kprintf(MODULE_NAME "%d virtual memory spaces available", VMEM_MAX - 1);

    initialized = true;

    return (0);
}
