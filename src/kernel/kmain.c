/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/cc.h>
#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/mm.h>
#include <nanvix/kernel/pm.h>
#include <nanvix/libcore.h>
#include <stdint.h>
#include <stdnoreturn.h>

/**
 * @brief Prints Multiboot information.
 *
 * @param addr Address of the Multiboot information.
 * @param magic Magic number.
 */
extern void mboot_print(unsigned magic, unsigned long addr);

extern noreturn void handle_syscall(void);

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Kernels stack.
 *
 * @note This is defined in assembly code.
 */
extern byte_t kstack[PAGE_SIZE];

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

static void *init(void *args)
{
    UNUSED(args);

    while (1) {
        __kcall5(0, 1, 2, 3, 4, 5);
    }

    return (NULL);
}

/**
 * @brief Clears the BSS section.
 */
static void clear_bss(void)
{
    __memset(&__BSS_START, 0, &__BSS_END - &__BSS_START);
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @brief Kernel main function.
 *
 * @param magic Magic number.
 * @param addr  Address of the Multiboot information.
 *
 * @returns This function does not return.
 */
noreturn void kmain(unsigned long magic, unsigned long addr)
{
    // Clear BSS before doing anything else, to ensure a
    // deterministic starting state for uninitialized data structures.
    clear_bss();

    stdout_init();
    klib_init(stdout_write, disable_interrupts);

    mboot_print(magic, addr);

    hal_init();
    mm_init();
    pm_init();

    thread_create(init, NULL);

    handle_syscall();

    UNREACHABLE();
}
