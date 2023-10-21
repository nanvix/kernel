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
    struct pde *pgdir; /** Underlying page directory*/
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
 * @brief Allocates a virtual memory space.
 *
 * @returns Upon successful completion, a pointer to the newly allocated virtual
 * memory space is returned. On failure, a null pointer is returned instead.
 */
static struct vmem *vmem_alloc(void)
{
    // Traverse all entries in the table of virtual memory spaces.
    for (int i = 0; i < VMEM_MAX; i++) {
        // Check if entry is free.
        if (vmem_table[i].pgdir == NULL) {
            // It is, thus return it.
            return (&vmem_table[i]);
        }
    }

    // No free entry was found.
    kprintf(MODULE_NAME " ERROR: no more virtual memory spaces are available");
    return (NULL);
}

/**
 * @brief Releases a virtual memory space.
 *
 * @param vmem Target virtual memory space.
 *
 * @returns Upon successful completion, zero is returned. Upon failure, a
 * negative number is returned instead.
 */
static int vmem_free(struct vmem *vmem)
{
    // Check if the target virtual memory is valid.
    if (vmem == NULL) {
        kprintf(MODULE_NAME " ERROR: invalid virtual memory space");
        return (-1);
    }

    // Check if the target virtual memory is busy.
    if (vmem->pgdir != NULL) {
        kprintf(MODULE_NAME " ERROR: virtual memory space is busy");
        return (-1);
    }

    // Release the target virtual memory space.
    vmem->pgdir = NULL;

    return (0);
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Returns the underlying page directory of a virtual memory space.
 */
const struct pde *vmem_pgdir_get(const struct vmem *vmem)
{
    return (vmem->pgdir);
}

/**
 * @details Creates a virtual memory space.
 */
struct vmem *vmem_create(const struct vmem *src_vmem)
{
    // Check if source virtual memory space is valid.
    if (src_vmem == NULL) {
        kprintf(MODULE_NAME " ERROR: invalid virtual memory space");
        goto error0;
    }

    // Allocate an entry in the table of virtual memory spaces.
    struct vmem *vmem = vmem_alloc();
    if (vmem == NULL) {
        goto error0;
    }

    // Allocate a page directory.
    struct pde *new_pgdir = kpage_get(true);
    if (new_pgdir == NULL) {
        goto error1;
    }

    // Link all pages from source page directory to new page directory.
    for (int i = 0; i < PGDIR_LENGTH; i++) {
        const struct pde *src_pgdir = src_vmem->pgdir;
        if (pde_is_present(&src_pgdir[i])) {
            __memcpy(&new_pgdir[i], &src_pgdir[i], sizeof(struct pde));
        }
    }

    // Initialize virtual memory space.
    vmem->pgdir = new_pgdir;

    return (vmem);

error1:
    vmem_free(vmem);
error0:
    return (vmem);
}

/**
 * @details Destroys a virtual memory space.
 */
int vmem_destroy(struct vmem *vmem)
{
    // Check if the target virtual memory is valid.
    if (vmem == NULL) {
        kprintf(MODULE_NAME " ERROR: invalid virtual memory space");
        return (-1);
    }

    // Check if the target virtual memory is busy.
    if (vmem->pgdir == NULL) {
        kprintf(MODULE_NAME " ERROR: virtual memory space is busy");
        return (-1);
    }

    // Release the target virtual memory space.
    vmem_free(vmem);

    // Release the page directory.
    kpage_put(vmem->pgdir);

    return (0);
}

/**
 * @details Attaches a virtual address range to a virtual memory space.
 */
int vmem_attach(struct vmem *vmem, vaddr_t addr, size_t size)
{
    // Check if target virtual memory space is valid.
    if (vmem == NULL) {
        kprintf(MODULE_NAME " ERROR: invalid virtual memory space");
        return (-1);
    }

    // Check if size is supported.
    if (size != PAGE_SIZE) {
        kpanic("unsupport range size");
    }

    KASSERT(upage_alloc(vmem->pgdir, addr, true, false) == 0);

    return (0);
}

/**
 * @details Maps a virtual address range into a virtual memory space.
 */
int vmem_map(struct vmem *vmem, vaddr_t vaddr, paddr_t paddr, size_t size,
             bool w, bool x)
{
    // Check if target virtual memory space is valid.
    if (vmem == NULL) {
        kprintf(MODULE_NAME " ERROR: invalid virtual memory space");
        return (-1);
    }

    // Check if size is supported.
    if (size != PAGE_SIZE) {
        kpanic("unsupport range size");
    }

    return (upage_map(vmem->pgdir, vaddr, paddr >> PAGE_SHIFT, w, x));
}

/**
 * @details Prints a virtual memory space.
 */
int vmem_print(const struct vmem *vmem)
{
    // Check if target virtual memory space is valid.
    if (vmem == NULL) {
        kprintf(MODULE_NAME " ERROR: invalid virtual memory space");
        return (-1);
    }

    // Print all present entries.
    for (int i = 0; i < PGDIR_LENGTH; i++) {
        if (pde_is_present(&vmem->pgdir[i])) {
            kprintf(MODULE_NAME " INFO: pde=%d, frame=%x",
                    i,
                    pde_frame_get(&vmem->pgdir[i]));
        }
    }

    return (0);
}

/**
 * @details Initializes the virtual memory manager.
 */
const struct vmem *vmem_init(const struct pde *root_pgdir)
{
    static bool initialized = false;

    // Check if the virtual memory manager was already initialized.
    if (initialized) {
        kpanic("[mm] virtual memory manager was already initialized");
    }

    kprintf(MODULE_NAME "initializing the virtual memory manager...");

    for (int i = 1; i < VMEM_MAX; i++) {
        vmem_table[i].pgdir = NULL;
    }

    vmem_table[0].pgdir = (struct pde *)root_pgdir;

    kprintf(MODULE_NAME "%d virtual memory spaces available", VMEM_MAX - 1);

    initialized = true;

    return (&vmem_table[0]);
}
