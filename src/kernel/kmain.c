/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#include <grub/mboot.h>
#include <nanvix/cc.h>
#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/mm.h>
#include <nanvix/kernel/pm.h>
#include <nanvix/libcore.h>
#include <stdint.h>
#include <stdnoreturn.h>

/**
 * @brief Kernels stack.
 *
 * @note This is defined in assembly code.
 */
extern byte_t kstack[PAGE_SIZE];

extern noreturn void handle_syscall(void);

void *init(void *args)
{
    UNUSED(args);

    while (1) {
        __kcall5(0, 1, 2, 3, 4, 5);
    }

    return (NULL);
}

/**
 * @brief Kernel main function.
 *
 * @param mboot_info Multiboot information.
 *
 * @returns This function does not return.
 */
noreturn void kmain(struct mboot_info *mboot_info)
{
    UNUSED(mboot_info);

    stdout_init();
    klib_init(stdout_write, disable_interrupts);

    cpu_init();

    exceptions_init();
    interrupts_init();
    memory_init();

    mm_init();
    pm_init();

    thread_create(init, NULL);

    handle_syscall();

    UNREACHABLE();
}
