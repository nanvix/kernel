/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include "mod.h"
#include <nanvix/cc.h>
#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <stdnoreturn.h>

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Generic exception handler.
 *
 * @param excp Exception information.
 * @param ctx  Interrupted context.
 */
static noreturn void default_handler(const struct exception *excp,
                                     const struct context *ctx)
{
    context_dump(ctx);
    exception_dump(excp);

    kpanic(MODULE_NAME "unhandled exception");

    UNREACHABLE();
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details This function registers @p handler as the exception handler for the
 * exception @p excpnum.
 */
int exception_register(int excpnum, exception_handler_t handler)
{
    // Check for invalid exception number.
    if ((excpnum < 0) || (excpnum >= EXCEPTIONS_NUM)) {
        kprintf(MODULE_NAME " ERROR: invalid exception number %d", excpnum);
        return (-1);
    }

    // Check for invalid exception handler.
    if (handler == NULL) {
        kprintf(MODULE_NAME " ERROR: invalid exception handler %x", handler);
        return (-1);
    }

    // Check if we are overwriting a handler.
    if (exceptions[excpnum].handler != default_handler) {
        if (exceptions[excpnum].handler != NULL) {
            // We are, thus issue a warning.
            kprintf(MODULE_NAME " WARNING: overwriting handler %x for %s",
                    exceptions[excpnum].handler,
                    exceptions[excpnum].name);
        }
    }

    exceptions[excpnum].handler = handler;

    kprintf(MODULE_NAME " INFO: exception handler %x registered for %s",
            exceptions[excpnum].handler,
            exceptions[excpnum].name);

    return (0);
}

/**
 * @details This function unregisters @p handler as the exception handler for
 * the exception @p excpnum.
 *
 * @author Pedro Henrique Penna
 */
int exception_unregister(int excpnum)
{
    // Check for invalid exception number.
    if ((excpnum < 0) || (excpnum >= EXCEPTIONS_NUM)) {
        kprintf(MODULE_NAME " ERROR: invalid exception number %d", excpnum);
        return (-1);
    }

    // Check if there is a handler registered.
    if (exceptions[excpnum].handler == default_handler) {
        kprintf(MODULE_NAME " ERROR: no handler for exception %d", excpnum);
        return (-1);
    }

    exceptions[excpnum].handler = default_handler;

    return (0);
}

/**
 * @details High-level exception dispatcher.
 */
void do_exception(const struct exception *excp, const struct context *ctx)
{
    int excpnum = exception_get_num(excp);
    exception_handler_t handler = exceptions[excpnum].handler;

    // Check if we have to fallback to the default handler.
    if (handler == NULL) {
        handler = default_handler;
    }

    /* Call handler. */
    handler(excp, ctx);
}

/**
 * @details Initializes the software exceptions module.
 */
void exceptions_init(void)
{
    // Register the default handler for all exceptions.
    for (int i = 0; i < EXCEPTIONS_NUM; i++) {
        // Skip early registered handlers.
        if (exceptions[i].handler != NULL) {
            continue;
        }

        exceptions[i].handler = default_handler;
    }

    // Run self-tests.
    test_exception();
}
