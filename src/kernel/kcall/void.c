/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/lib.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Void kernel call that takes no arguments.
 */
int kcall_void0(void)
{
    return (0);
}

/**
 * @details Void kernel call that takes one argument.
 */
int kcall_void1(int arg0)
{
    KASSERT(arg0 == 1);

    return (arg0);
}

/**
 * @details Void kernel call that takes two arguments.
 */
int kcall_void2(int arg0, int arg1)
{
    KASSERT(arg0 == 1);
    KASSERT(arg1 == 2);

    return (arg0 + arg1);
}

/**
 * @details Void kernel call that takes three arguments.
 */
int kcall_void3(int arg0, int arg1, int arg2)
{
    KASSERT(arg0 == 1);
    KASSERT(arg1 == 2);
    KASSERT(arg2 == 3);

    return (arg0 + arg1 + arg2);
}

/**
 * @details Void kernel call that takes four arguments.
 */
int kcall_void4(int arg0, int arg1, int arg2, int arg3)
{
    KASSERT(arg0 == 1);
    KASSERT(arg1 == 2);
    KASSERT(arg2 == 3);
    KASSERT(arg3 == 4);

    return (arg0 + arg1 + arg2 + arg3);
}

/**
 * @details Void kernel call that takes five arguments.
 */
int kcall_void5(int arg0, int arg1, int arg2, int arg3, int arg4)
{
    KASSERT(arg0 == 1);
    KASSERT(arg1 == 2);
    KASSERT(arg2 == 3);
    KASSERT(arg3 == 4);
    KASSERT(arg4 == 5);

    return (arg0 + arg1 + arg2 + arg3 + arg4);
}
