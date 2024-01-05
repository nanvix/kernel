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
 * @details Lookup table with information about exceptions.
 */
struct exception_info exceptions[EXCEPTIONS_NUM] = {
    {NULL, "division-by-zero error"},
    {NULL, "debug exception"},
    {NULL, "non-maskable interrupt"},
    {NULL, "breakpoint exception"},
    {NULL, "overflow exception"},
    {NULL, "bounds check exception"},
    {NULL, "invalid opcode exception"},
    {NULL, "coprocessor not available"},
    {NULL, "double fault"},
    {NULL, "coprocessor segment overrun"},
    {NULL, "invalid task state segment"},
    {NULL, "segment not present"},
    {NULL, "static segment fault"},
    {NULL, "general protection fault"},
    {NULL, "page fault"},
    {NULL, "floating point unit exception"},
    {NULL, "alignment check"},
    {NULL, "machine exception"},
    {NULL, "smid unit exception"},
    {NULL, "virtual exception"},
    {NULL, "security exception"},
};

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details This function dumps information about the exception pointed to by
 * @p excp on the standard output device.
 */
void exception_dump(const struct exception *excp)
{
    kprintf("%s", exceptions[excp->num].name);
}

/**
 * @details This function gets the exception number stored in the exception
 * information structure pointed to by @p excp.
 */
int exception_get_num(const struct exception *excp)
{
    return (excp->num);
}
