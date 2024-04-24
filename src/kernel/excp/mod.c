/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/errno.h>
#include <nanvix/kernel/excp.h>
#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/log.h>
#include <nanvix/kernel/pm.h>
#include <nanvix/types.h>
#include <stdbool.h>
#include <stddef.h>

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Exception table.
 */
static struct {
    /** Signaled when an exception is triggered. */
    struct condvar exception_triggered;
    struct {
        pid_t owner;                 /** Owner process.                       */
        bool pending;                /** Pending exception?                   */
        const struct excpinfo *info; /** Exception information                */
        struct condvar ack;          /** Signaled when an exception is acked. */
    } lines[EXCEPTIONS_NUM];
} table;

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Initializes the exception table.
 */
static void excpline_init(void)
{
    for (int i = 0; i < EXCEPTIONS_NUM; i++) {
        table.lines[i].owner = -1;
        table.lines[i].pending = false;
        table.lines[i].info = NULL;
        cond_init(&table.lines[i].ack);
    }
    cond_init(&table.exception_triggered);
}

/**
 * @brief Checks if the current process owns an exception.
 *
 * @param excpnum Number of target exception.
 *
 * @note This function panics if any of its parameters are invalid.
 */
static bool excpline_owns(int excpnum)
{
    KASSERT(WITHIN(excpnum, 0, EXCEPTIONS_NUM));
    const struct process *p = process_get_curr();
    return (table.lines[excpnum].owner == p->pid);
}

/**
 * @brief Checks if the current process owns any exception.
 */
static bool excpline_owns_any(void)
{
    const struct process *p = process_get_curr();

    // Traverse exception table.
    for (int i = 0; i < EXCEPTIONS_NUM; i++) {
        if (table.lines[i].owner == p->pid) {
            return (true);
        }
    }
    return (false);
}

/**
 * @brief Checks if an exception is assigned.
 *
 * @param excpnum Number of target exception.
 *
 * @return Non-zero if the exception is assigned to some process, and zero
 * otherwise.
 *
 * @note This function panics if any of its parameters are invalid.
 */
static bool excpline_is_assigned(int excpnum)
{
    KASSERT(WITHIN(excpnum, 0, EXCEPTIONS_NUM));
    return (table.lines[excpnum].owner != -1);
}

/**
 * @brief Assigns an exception to the current process.
 *
 * @param excpnum Number of target exception.
 *
 * @note This function panics if any of its parameters are invalid.
 */
static void excpline_assign(int excpnum)
{
    KASSERT(WITHIN(excpnum, 0, EXCEPTIONS_NUM));
    const struct process *p = process_get_curr();
    table.lines[excpnum].owner = p->pid;
}

/**
 * @brief Unassigns an exception from the current process.
 *
 * @param excpnum Number of target exception.
 *
 * @note This function panics if any of its parameters are invalid.
 */
static void excpline_unassign(int excpnum)
{
    KASSERT(WITHIN(excpnum, 0, EXCEPTIONS_NUM));
    table.lines[excpnum].owner = -1;
}

/**
 * @brief Sets an exception as pending.
 *
 * @param excpnum Number of target exception.
 * @param info Exception information.
 *
 * @note This function panics if any of its parameters are invalid.
 */
static void excpline_trigger(int excpnum, const struct excpinfo *info)
{
    KASSERT(WITHIN(excpnum, 0, EXCEPTIONS_NUM));
    KASSERT(info != NULL);
    KASSERT(!table.lines[excpnum].pending);

    table.lines[excpnum].pending = true;
    table.lines[excpnum].info = info;
    cond_broadcast(&table.exception_triggered);
}

/**
 * @brief Checks if an exception is pending.
 *
 * @param excpnum Number of target exception.
 *
 * @return Non-zero if the exception is pending, and zero otherwise.
 *
 * @note This function panics if any of its parameters are invalid.
 */
static bool excpline_is_triggered(int excpnum)
{
    KASSERT(WITHIN(excpnum, 0, EXCEPTIONS_NUM));
    return (table.lines[excpnum].pending);
}

/**
 * @brief Acknowledges an exception.
 *
 * @param excpnum Number of target exception.
 *
 * @note This function panics if any of its parameters are invalid.
 */
static void excpline_ack(int excpnum)
{
    KASSERT(WITHIN(excpnum, 0, EXCEPTIONS_NUM));
    table.lines[excpnum].pending = false;
    cond_broadcast(&table.lines[excpnum].ack);
}

/**
 * @brief Waits for an exception to be acknowledged.
 *
 * @param excpnum Number of target exception.
 *
 * @note This function panics if any of its parameters are invalid.
 */
static void excpline_wait_ack(int excpnum)
{
    KASSERT(WITHIN(excpnum, 0, EXCEPTIONS_NUM));
    cond_wait(&table.lines[excpnum].ack);
    KASSERT(!table.lines[excpnum].pending);
}

/**
 * @brief Waits for any exception to be triggered.
 *
 * @param info Exception information.
 */
static void excpline_wait_any(struct excpinfo *info)
{
    const struct process *p = process_get_curr();
    while (true) {
        cond_wait(&table.exception_triggered);

        for (int i = 0; i < EXCEPTIONS_NUM; i++) {
            if (table.lines[i].owner == p->pid) {
                __memcpy(info, table.lines[i].info, sizeof(struct excpinfo));
                return;
            }
        }

        warn("spurious wake up");
    }

    UNREACHABLE();
}

/**
 * @brief Kernel exception handler.
 *
 * @param excp Exception information
 * @param ctx  Context information.
 */
static void kernel_exception_handler(const struct exception *excp,
                                     const struct context *ctx)
{
    const int excpnum = excp->num;

    // Check if exception is assigned.
    if (!excpline_is_assigned(excpnum)) {
        context_dump(ctx);
        exception_dump(excp);
        kpanic("unassigned exception");
    }

    // Check for exception reentrancy.
    if (excpline_is_triggered(excpnum)) {
        context_dump(ctx);
        exception_dump(excp);
        // TODO: support reentrant exceptions.
        kpanic("reentrant exceptions are not supported");
    }

    const struct excpinfo info = {
        .num = excpnum,
        .addr = get_page_fault_addr(),
        .pc = excp->instruction,
    };

    excpline_trigger(excpnum, &info);

    // FIXME: Race condition with excp_wait().
    // An exception may trigger just before the process calls excp_wait(). Thus
    // causing the exception signal to be lost. This can be solved by Modeling
    // this as a single consumer (the excp_wait() function) and single producer
    // (the kernel_exception_handler() function) queue. If exception reentrancy
    // is considered, this queue should support multiple produces (reentrant
    // calls to the kernel_exception_handler() function).
    warn("exception %d was triggered", excpnum);

    excpline_wait_ack(excpnum);
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Controls which action to take when an exception happens.
 */
int excp_control(int excpnum, int action)
{
    trace("excpnum=%d, action=%x", excpnum, action);

    // Check for invalid exception number.
    if (!WITHIN(excpnum, 0, EXCEPTIONS_NUM)) {
        error("invalid exception number %d", excpnum);
        return (-EINVAL);
    }

    // Parse action.
    switch (action) {
        // Handle exception.
        case EXCP_HANDLE: {
            // Check if this exception is already being handled.
            if (excpline_is_assigned(excpnum)) {
                error("exception %d is assigned to other process", excpnum);
                return (-EBUSY);
            }
            // Assign exception handling to the current process.
            excpline_assign(excpnum);
        } break;

        // Defer exception to kernel.
        case EXCP_DEFER: {
            // Check if the current process is handling this exception.
            if (excpline_owns(excpnum)) {
                error("exception %d is not assigned to current process",
                      excpnum);
                return (-EPERM);
            }

            // TODO: check if there are any pending events to be handled.
            // If there are any, we should fail with EBUSY.

            // Unassign exception handling from the current process.
            excpline_unassign(excpnum);
        } break;

        // Invalid action.
        default: {
            error("invalid action %d", action);
            return (-EINVAL);
        } break;
    }

    return (0);
}

/**
 * @details Resumes the execution of a faulting process.
 */
int excp_resume(int excpnum)
{
    // Check for invalid exception number.
    if (!WITHIN(excpnum, 0, EXCEPTIONS_NUM)) {
        error("invalid exception number %d", excpnum);
        return (-EINVAL);
    }

    // Check if the calling process is handling the exception.
    if (excpline_owns(excpnum)) {
        error("exception %d is not assigned to current process", excpnum);
        return (-EPERM);
    }

    // Resume the faulting process.
    excpline_ack(excpnum);

    return (0);
}

/**
 * @details Waits for an exception to happen.
 */
int excp_wait(struct excpinfo *info)
{
    // Check if storage location is valid.
    if (info == NULL) {
        error("invalid storage location");
        return (-EINVAL);
    }

    // Check if the calling process is handling any exception.
    if (!excpline_owns_any()) {
        error("process is not assigned to handle any exception");
        return (-EPERM);
    }

    // TODO: fix race condition with kernel_exception_handler().

    excpline_wait_any(info);

    return (0);
}

/**
 * @details Initialize the exceptions management subsystem.
 */
int excp_init(void)
{
    info("initializing exception manager...");

    // Sanity check sizes.
    KASSERT_SIZE(sizeof(struct excpinfo), __SIZEOF_EXCPINFO);

    excpline_init();

    // Register exception handlers.
    for (int i = 0; i < EXCEPTIONS_NUM; i++) {
        const int ret = exception_register(i, kernel_exception_handler);
        if (ret < 0) {
            warn("failed to register kernel exception handler %d (errno=%d)",
                 i,
                 ret);
        }
    }

    return (0);
}
