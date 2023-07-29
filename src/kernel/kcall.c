/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/pm.h>
#include <stdbool.h>
#include <stdnoreturn.h>

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

static struct semaphore kernel_semahore = SEMAPHORE_INITIALIZER(0);
static struct semaphore user_semaphore = SEMAPHORE_INITIALIZER(0);

static volatile struct {
    word_t kcall_nr; /** Kernel call number.             */
    word_t arg0;     /** First argument of kernel call.  */
    word_t arg1;     /** Second argument of kernel call. */
    word_t arg2;     /** Third argument of kernel call.  */
    word_t arg3;     /** Fourth argument of kernel call. */
    word_t arg4;     /** Fifth argument of kernel call.  */
    word_t ret;      /** Return value of kernel call.    */
} scoreboard;

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

noreturn void handle_syscall(void)
{

    while (true) {
        semaphore_down(&kernel_semahore);

        scoreboard.ret = 0;

        semaphore_up(&user_semaphore);
    }
}

/**
 * @brief Kernel call dispatcher.
 *
 * @param arg0     First kernel call argument.
 * @param arg1     Second kernel call argument.
 * @param arg2     Third kernel call argument.
 * @param arg3     Fourth kernel call argument.
 * @param arg4     Fifth kernel call argument.
 * @param kcall_nr Kernel call number.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
int do_kcall(word_t arg0, word_t arg1, word_t arg2, word_t arg3, word_t arg4,
             word_t kcall_nr)
{
    // Copy kernel call parameters.
    scoreboard.kcall_nr = kcall_nr;
    scoreboard.arg0 = arg0;
    scoreboard.arg1 = arg1;
    scoreboard.arg2 = arg2;
    scoreboard.arg3 = arg3;
    scoreboard.arg4 = arg4;

    semaphore_up(&kernel_semahore);
    semaphore_down(&user_semaphore);

    return (scoreboard.ret);
}
