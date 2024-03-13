/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/log.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Forges the user stack of a user-created thread.
 */
void *uthread_forge_stack(void *ustack, void *arg, void *(*func)())
{
    // Check for invalid user stack.
    if (ustack == NULL) {
        log(ERROR, "invalid user stack");
        return (NULL);
    }

    // Sets up thread_caller() arguments.
    word_t *usp = (word_t *)((word_t)ustack + PAGE_SIZE);
    *--usp = (word_t)arg;  /* Thread Argument.       */
    *--usp = (word_t)func; /* Thread function.       */
    usp--;                 /* Alignment (saved eip). */

    return (usp);
}
