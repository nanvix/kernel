/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/lib.h>
#include <stdnoreturn.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Shutdowns the system.
 */
noreturn void kcall_shutdown(void)
{
    // Magic string.
    // Our CI will look for this as the last print statement.
    kpanic("Hello World!");
}
