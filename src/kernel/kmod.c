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
#define MODULE_NAME "kmod"

/**
 * @brief Maximum number of kernel modules.
 */
#define KMOD_MAX 8

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

/**
 * @brief Kernel module.
 */
struct kmod {
    paddr_t start;                  /**< Start address. */
    paddr_t end;                    /**< End address.   */
    char cmdline[KMOD_CMDLINE_MAX]; /**< Command line.  */
};

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
 * @details Returns the start address of a kernel module.
 */
int kmod_get_start(unsigned i, paddr_t *start)
{
    // Check if target kernel module is valid.
    if (i >= kmods_table.n) {
        kprintf(MODULE_NAME " ERROR: invalid kernel module");
        return (-1);
    }

    // Check if storage location is valid.
    if (start == NULL) {
        kprintf(MODULE_NAME " ERROR: invalid storage location");
        return (-1);
    }

    *start = kmods_table.mods[i].start;

    return (0);
}

/**
 * @details Returns the end address of a kernel module.
 */
int kmod_get_end(unsigned i, paddr_t *end)
{
    // Check if target kernel module is valid.
    if (i >= kmods_table.n) {
        kprintf(MODULE_NAME " ERROR: invalid kernel module");
        return (-1);
    }

    // Check if storage location is valid.
    if (end == NULL) {
        kprintf(MODULE_NAME " ERROR: invalid storage location");
        return (-1);
    }

    *end = kmods_table.mods[i].end;

    return (0);
}

/**
 * @details Returns the command line of a kernel module.
 */
int kmod_get_cmdline(unsigned i, char *cmdline)
{
    // Check if target kernel module is valid.
    if (i >= kmods_table.n) {
        kprintf(MODULE_NAME " ERROR: invalid kernel module");
        return (-1);
    }

    // Check if storage location is valid.
    if (cmdline == NULL) {
        kprintf(MODULE_NAME " ERROR: invalid storage location");
        return (-1);
    }

    __strncpy(cmdline, kmods_table.mods[i].cmdline, KMOD_CMDLINE_MAX);

    return (0);
}
