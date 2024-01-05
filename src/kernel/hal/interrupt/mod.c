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
#include <stdint.h>

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Timer handler.
 */
static interrupt_handler_t timer_handler = NULL;

/**
 * @brief Number of spurious interrupts.
 */
static unsigned spurious = 0;

/**
 * @brief Timer value.
 */
static uint64_t timer_value = 0;

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Default hardware interrupt handler.
 */
static void default_handler(void)
{
    // Warn if we got too many spurious interrupts.
    if (++spurious >= INTERRUPT_SPURIOUS_THRESHOLD) {
        kprintf(MODULE_NAME " INFO: spurious interrupt");
    }

    noop();
}

/**
 * @brief Wrapper for timer interrupt.
 */
static void do_timer(void)
{
    timer_value++;

    // Check if we have a timer handler.
    if (LIKELY(timer_handler != NULL)) {
        timer_handler();
    }
}

/**
 * @brief Changes interrupt level.
 */
static inline int interrupts_set_level(int newlevel)
{
    return (lpic_lvl_set(newlevel));
}

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Disables all hardware interrupts.
 */
void interrupts_disable(void)
{
    lpic_disable();
}

/**
 * @details Enables all hardware interrupts.
 */
void interrupts_enable(void)
{
    lpic_enable();
}

/**
 * @details This function dispatches a hardware interrupt to the a
 * previously-registered handler function. If no handler function was
 * registered for the interrupt, this function returns immediately.
 *
 * @note This function is called from assembly code.
 */
void do_interrupt(int intnum)
{
    // Check if there are more pending interrupts to handle.
    do {
        // Acknowledge interrupt.
        lpic_ack(intnum);

        // Check if there is a handler for this interrupt.
        if (UNLIKELY(interrupt_handlers[intnum] == NULL)) {
            return;
        }

        // Call handler.
        interrupt_handlers[intnum]();
    } while ((intnum = lpic_next()) != 0);
}

/**
 * @details This function registers @p handler as the handler function for the
 * interrupt whose number is @p num. If a handler function was previously
 * registered with this number, this function fails.
 */
int interrupt_register(int num, interrupt_handler_t handler)
{
    // Check for invalid interrupt number.
    if ((num < 0) || (num >= INTERRUPTS_NUM)) {
        kprintf(MODULE_NAME " ERROR: invalid interrupt number %d", num);
        return (-1);
    }

    // Check if target interrupt number concerns the timer.
    if (num != INTERRUPT_TIMER) {
        // It doesn't, check if we have a handler function already registered.
        if (interrupt_handlers[num] != NULL) {
            // We do, fail.
            kprintf(MODULE_NAME
                    " ERROR: interrupt handler already registered for irq %d",
                    num);
            return (-1);
        }

        interrupt_handlers[num] = handler;
    } else {
        // It does, check if we have a handler function already registered.
        if (timer_handler != NULL) {
            // We do, fail.
            kprintf(MODULE_NAME
                    " ERROR: timer interrupt handler already registered");
            return (-1);
        }

        timer_handler = handler;
    }

    lpic_unmask(num);

    kprintf(MODULE_NAME " INFO: interrupt handler registered for irq %d", num);

    return (0);
}

/**
 * @details This function unregisters a handler function for the interrupt whose
 * number is @p num. If no handler function was previously registered with this
 * number, this function fails.
 */
int interrupt_unregister(int num)
{
    // Check for invalid interrupt number.
    if ((num < 0) || (num >= INTERRUPTS_NUM)) {
        kprintf(MODULE_NAME " ERROR: invalid interrupt number %d", num);
        return (-1);
    }

    // Check if target interrupt number concerns the timer.
    if (num != INTERRUPT_TIMER) {
        // It doesn't, check if we have a handler function registered.
        if (interrupt_handlers[num] == NULL) {
            // We don't, fail.
            kprintf(MODULE_NAME
                    " ERROR: no interrupt handler registered for irq %d",
                    num);
            return (-1);
        }

        interrupt_handlers[num] = NULL;
    } else {
        // It does, check if we have a handler function registered.
        if (timer_handler == NULL) {
            // We don't, fail.
            kprintf(MODULE_NAME
                    " ERROR: no timer interrupt handler registered");
            return (-1);
        }

        timer_handler = NULL;
    }

    lpic_mask(num);

    kprintf(MODULE_NAME " INFO: interrupt handler unregistered for irq %d",
            num);

    return (0);
}

/**
 * @details Initializes hardware interrupts by registering a default handler to
 * all available interrupts.
 */
void interrupts_init(void)
{
    kprintf(MODULE_NAME " INFO: initializing...");

    // Register default handler for all interrupts.
    for (int i = 0; i < INTERRUPTS_NUM; i++) {
        const interrupt_handler_t handler =
            (i == INTERRUPT_TIMER) ? do_timer : default_handler;

        interrupt_handlers[i] = handler;
    }

    // Raise interrupt level so as to enable timer interrupts.
    lpic_lvl_set(IRQLVL_4);

    test_interrupts();
}
