/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/cc.h>
#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <stdnoreturn.h>

/*===========================================================================*
 * Private Functions                                                         *
 *===========================================================================*/

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

    kpanic("unhandled exception");

    UNREACHABLE();
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * The exception_register() function registers @p handler as the
 * exception handler for the exception @p excpnum.
 */
int exception_register(int excpnum, exception_handler_t handler)
{
    /* Invalid exception number. */
    if ((excpnum < 0) || (excpnum >= EXCEPTIONS_NUM)) {
        kprintf("[hal][core] invalid exception number");
        return (-1);
    }

    /* Invalid handler. */
    if (handler == NULL) {
        kprintf("[hal][core] invalid exception handler");
        return (-1);
    }

    /* Overwriting handler. */
    if (exceptions[excpnum].handler != default_handler) {
        if (exceptions[excpnum].handler != NULL) {
            kprintf("[hal][core] overwriting handler %x for %s",
                    exceptions[excpnum].handler,
                    exceptions[excpnum].name);
        }
    }

    exceptions[excpnum].handler = handler;

    kprintf("[hal][core] exception handler %x registered for %s",
            exceptions[excpnum].handler,
            exceptions[excpnum].name);

    return (0);
}

/**
 * @details High-level exception dispatcher.
 */
void do_exception(const struct exception *excp, const struct context *ctx)
{
    int excpnum = exception_get_num(excp);
    exception_handler_t handler = exceptions[excpnum].handler;

    /* Switch to default handler. */
    if (handler == NULL) {
        handler = default_handler;
    }

    /* Call handler. */
    handler(excp, ctx);
}

/**
 * @details Initializes the exception module.
 */
void exceptions_init(void)
{
    for (int i = 0; i < EXCEPTIONS_NUM; i++) {
        /* Skip early registered handlers. */
        if (exceptions[i].handler != NULL) {
            continue;
        }

        exceptions[i].handler = default_handler;
    }
}
