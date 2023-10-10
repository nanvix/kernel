/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <elf.h>
#include <nanvix/cc.h>
#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/kargs.h>
#include <nanvix/kernel/kmod.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/mm.h>
#include <nanvix/kernel/pm.h>
#include <nanvix/libcore.h>
#include <stdint.h>
#include <stdnoreturn.h>

// TODO: place these on a header file.
extern vaddr_t elf32_load(const struct elf32_fhdr *elf);
extern noreturn void handle_syscall(void);

/*============================================================================*
 * Extern Functions                                                           *
 *============================================================================*/

/**
 * @brief Runs unit tests on the Page Frame Allocator.
 */
extern void test_frame(void);

/**
 * @brief Runs unit tests on the Kernel Page Allocator.
 */
extern void test_kpool(void);

/**
 * @brief Runs unit tests on the User Page Allocator.
 */
extern void test_upool(void);

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

/**
 * @brief Spawns init server.
 */
static void spawn_init(void)
{
    // Check if init was loaded.
    if (kmod_count() == 0) {
        // It was not, thus panic because the whole system depends on it.
        kpanic("ERROR: missing init server");
    }

    struct kmod kmod = {0};

    // Assert shouldn't fail because we request details of a valid module.
    KASSERT(kmod_get(&kmod, 0) == 0);

    kprintf("INFO: loading module %s", kmod.cmdline);

    const vaddr_t start = kmod.start;

    // Check if the module was loaded successfully.
    if (start == 0) {
        kpanic("ERROR: failed to load module %s", kmod.cmdline);
    }

    // Spawn init server.
    thread_create((void *(*)(void *))start, NULL);
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
 * @param args Kernel arguments.
 *
 * @returns This function does not return.
 */
noreturn void kmain(struct kargs *args)
{
    // Clear BSS before doing anything else, to ensure a
    // deterministic starting state for uninitialized data structures.
    clear_bss();

    stdout_init();
    klib_init(stdout_write, disable_interrupts);

    if (kargs_parse(args) != 0) {
        kpanic("failed to parse kernel arguments");
    }

    hal_init();
    const void *root_pgdir = mm_init();
    pm_init(root_pgdir);

    test_frame();
    test_kpool();
    test_upool();

    // Spawn the init server. Note that although we create a new thread, we will
    // not switch to it, because interrupts are disabled. This will save us from
    // a race condition in the system call dispatcher module.
    spawn_init();

    // Start handling system calls. Interrupts will be enabled as soon as we
    // block waiting for a kernel call to be issued.
    handle_syscall();

    UNREACHABLE();
}
