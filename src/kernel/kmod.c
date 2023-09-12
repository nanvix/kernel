/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/kmod.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/libcore.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Name of this module.
 */
#define MODULE_NAME "[kernel][kmod]"

/**
 * @brief Maximum number of kernel modules.
 */
#define KMOD_MAX 8

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Table of kernel modules.
 */
static struct {
    unsigned n;
    struct kmod mods[KMOD_MAX];
} kmods_table = {.n = 0};

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Registers a kernel module.
 */
int kmod_register(paddr_t start, paddr_t end, const char *cmdline)
{
    // Check if there is space for a new kernel module.
    if (kmods_table.n >= KMOD_MAX) {
        kprintf(MODULE_NAME " ERROR: no space for a new kernel module");
        return (-1);
    }

    kmods_table.mods[kmods_table.n].start = start;
    kmods_table.mods[kmods_table.n].end = end;
    __strncpy(kmods_table.mods[kmods_table.n].cmdline, cmdline, 64);

    kmods_table.n++;

    return (0);
}

/**
 * @details Returns the number of registered kernel modules.
 */
unsigned kmod_count(void)
{
    return (kmods_table.n);
}

/**
 * @details Retrieves information of a kernel module.
 */
int kmod_get(struct kmod *info, unsigned index)
{
    // Check if target kernel module is valid.
    if (index >= kmods_table.n) {
        kprintf(MODULE_NAME " ERROR: invalid kernel module");
        return (-1);
    }

    // Check if storage location is valid.
    if (info == NULL) {
        kprintf(MODULE_NAME " ERROR: invalid storage location");
        return (-1);
    }

    info->start = kmods_table.mods[index].start;
    info->end = kmods_table.mods[index].end;
    __strncpy(info->cmdline, kmods_table.mods[index].cmdline, KMOD_CMDLINE_MAX);

    return (0);
}

/**
 * @details Prints kernel modules table.
 */
void kmod_print(void)
{
    kprintf(MODULE_NAME " INFO: %d kernel modules registered", kmods_table.n);

    for (unsigned i = 0; i < kmods_table.n; i++) {
        kprintf(MODULE_NAME " INFO: kernel module %s (id=%d, start=%x, end=%x)",
                kmods_table.mods[i].cmdline,
                i,
                kmods_table.mods[i].start,
                kmods_table.mods[i].end);
    }
}
