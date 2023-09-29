/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/
#include <nanvix/kernel/hal.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @brief Server entry point.
 *
 * @param args Arguments (unused).
 *
 * @return This function does not return.
 */
int _do_start(void *args)
{
    ((void)args);

    while (1) {
        __kcall5(0, 1, 2, 3, 4, 5);
    }

    return 0;
}
