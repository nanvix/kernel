/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#include <nanvix/kernel/lib.h>
#include <nanvix/libcore.h>

/**
 * @details The kputs() function writes the null-terminated string pointed to
 * by @p str into the standard output device of the kernel.
 */
void kputs(const char *str)
{
    /*
     * Don't do anything, if kernel library
     * was not initialized.
     */
    if (__kwrite_fn == NULL) {
        return;
    }

    __kwrite_fn(str, __strlen(str));
}
