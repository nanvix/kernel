/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>

/*============================================================================*
 * Public Variables                                                           *
 *============================================================================*/

/**
 * @details Interrupt handlers.
 */
interrupt_handler_t interrupt_handlers[INTERRUPTS_NUM] = {NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL};

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Forges an interrupt stack.
 */
void *interrupt_forge_stack(void *user_stack, void *kernel_stack,
                            void (*user_func)(void), void (*kernel_func)(void))
{
    // Check for invalid user stack.
    if (user_stack == NULL) {
        kprintf("[hal] ERROR: invalid user stack");
        return (NULL);
    }

    // Check for invalid kernel stack.
    if (kernel_stack == NULL) {
        kprintf("[hal] ERROR: invalid kernel stack");
        return (NULL);
    }

    // Check for invalid user function.
    if (user_func == NULL) {
        kprintf("[hal] ERROR: invalid user function");
        return (NULL);
    }

    // Check for invalid kernel function.
    if (kernel_func == NULL) {
        kprintf("[hal] ERROR: invalid kernel function");
        return (NULL);
    }

    __memset(kernel_stack, 0, PAGE_SIZE);

    // Create a fake interrupt stack.
    word_t *kstackp = (word_t *)((word_t)kernel_stack + PAGE_SIZE);
    *--kstackp = gdt_user_ds();       /* user ds     */
    *--kstackp = (word_t)user_stack;  /* user esp    */
    *--kstackp = EFLAGS_IF;           /* eflags      */
    *--kstackp = gdt_user_cs();       /* cs          */
    *--kstackp = (word_t)user_func;   /* user eip    */
    *--kstackp = (word_t)kernel_func; /* kernel eip  */

    return (kstackp);
}
