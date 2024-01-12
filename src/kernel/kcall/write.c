/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/mm.h>
#include <nanvix/libcore.h>
#include <stddef.h>

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @brief Write buffer size.
 */
#define WRITE_BUFFER_SIZE 128

/**
 * @details Writes a buffer to the kernel's standard output device.
 */
size_t kcall_write(int fd, const char *buf, size_t n)
{
    char buf2[WRITE_BUFFER_SIZE + 1];

    UNUSED(fd);

    /* Invalid file descriptor. */
    if (fd < 0)
        return (-1);

    /* Invalid buffer. */
    if (buf == NULL)
        return (-1);

    /* Invalid buffer size. */
    if (n > WRITE_BUFFER_SIZE)
        return (-1);

    /* Invalid buffer location. */
    if (!mm_check_area(VADDR(buf), n, UMEM_AREA)) {
        return (-1);
    }

    /* Avoid overflow. */
    __memcpy(buf2, buf, n);
    buf2[n++] = '\0';

    stdout_write(buf2, n);

    return (n);
}
