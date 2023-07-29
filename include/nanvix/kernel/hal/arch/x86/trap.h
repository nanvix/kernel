/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_HAL_ARCH_X86_TRAP_H_
#define NANVIX_HAL_ARCH_X86_TRAP_H_

#include <nanvix/kernel/hal.h>

#ifndef _ASM_FILE_

/**
 * @brief Issues a kernel call with no arguments.
 *
 * @param kcall_nr Kernel call number.
 *
 * @returns The kernel call return value.
 */
static inline word_t __kcall0(word_t kcall_nr)
{
    word_t ret;

    asm __volatile__("int %2"
                     : "=a"(ret)
                     : "a"(kcall_nr), "N"(TRAP_GATE)
                     : "memory", "cc");

    return (ret);
}

/**
 * @brief Issues a kernel call with one argument.
 *
 * @param kcall_nr Kernel call number.
 * @param arg0 Kernel call number.
 *
 * @returns The kernel call return value.
 */
static inline word_t __kcall1(word_t kcall_nr, word_t arg0)
{
    word_t ret;

    asm __volatile__("int %2"
                     : "=a"(ret)
                     : "a"(kcall_nr), "N"(TRAP_GATE), "b"(arg0)
                     : "memory", "cc");

    return (ret);
}

/**
 * @brief Issues a kernel call with two arguments.
 *
 * @param kcall_nr Kernel call number.
 * @param arg0 Kernel call number.
 * @param arg1 Kernel call number.
 *
 * @returns The kernel call return value.
 */
static inline word_t __kcall2(word_t kcall_nr, word_t arg0, word_t arg1)
{
    word_t ret;

    asm __volatile__("int %2"
                     : "=a"(ret)
                     : "a"(kcall_nr), "N"(TRAP_GATE), "b"(arg0), "c"(arg1)
                     : "memory", "cc");

    return (ret);
}

/**
 * @brief Issues a kernel call with three arguments.
 *
 * @param kcall_nr Kernel call number.
 * @param arg0 Kernel call number.
 * @param arg1 Kernel call number.
 * @param arg2 Kernel call number.
 *
 * @returns The kernel call return value.
 */
static inline word_t __kcall3(word_t kcall_nr, word_t arg0, word_t arg1,
                              word_t arg2)
{
    word_t ret;

    asm __volatile__(
        "int %2"
        : "=a"(ret)
        : "a"(kcall_nr), "N"(TRAP_GATE), "b"(arg0), "c"(arg1), "d"(arg2)
        : "memory", "cc");

    return (ret);
}

/**
 * @brief Issues a kernel call with four arguments.
 *
 * @param kcall_nr Kernel call number.
 * @param arg0 Kernel call number.
 * @param arg1 Kernel call number.
 * @param arg2 Kernel call number.
 * @param arg3 Kernel call number.
 *
 * @returns The kernel call return value.
 */
static inline word_t __kcall4(word_t kcall_nr, word_t arg0, word_t arg1,
                              word_t arg2, word_t arg3)
{
    word_t ret;

    asm __volatile__("int %2"
                     : "=a"(ret)
                     : "a"(kcall_nr),
                       "N"(TRAP_GATE),
                       "b"(arg0),
                       "c"(arg1),
                       "d"(arg2),
                       "S"(arg3)
                     : "memory", "cc");

    return (ret);
}

/**
 * @brief Issues a kernel call with five arguments.
 *
 * @param kcall_nr Kernel call number.
 * @param arg0 Kernel call number.
 * @param arg1 Kernel call number.
 * @param arg2 Kernel call number.
 * @param arg3 Kernel call number.
 * @param arg4 Kernel call number.
 *
 * @returns The kernel call return value.
 */
static inline word_t __kcall5(word_t kcall_nr, word_t arg0, word_t arg1,
                              word_t arg2, word_t arg3, word_t arg4)
{
    word_t ret;

    asm __volatile__("int %2"
                     : "=a"(ret)
                     : "a"(kcall_nr),
                       "N"(TRAP_GATE),
                       "b"(arg0),
                       "c"(arg1),
                       "d"(arg2),
                       "S"(arg3),
                       "D"(arg4)
                     : "memory", "cc");

    return (ret);
}

#endif /* !_ASM_FILE_ */

#endif /* NANVIX_HAL_ARCH_X86_TRAP_H_ */
