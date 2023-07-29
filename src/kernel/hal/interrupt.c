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
#include <stdint.h>

/**
 * @brief Timer handler.
 */
static interrupt_handler_t timer_handler = NULL;
/**
 * @brief Event handler.
 */
static interrupt_handler_t event_handler = NULL;

/**
 * @brief Number of spurious interrupts.
 */
static unsigned spurious = 0;

/**
 * @brief Timer value for archs that lacks RTC.
 */
uint64_t timer_value = 0;

/**
 * @brief Default hardware interrupt handler.
 */
static void default_handler(void)
{
    /* Too many spurious interrupts. */
    if (++spurious >= INTERRUPT_SPURIOUS_THRESHOLD) {
        kprintf("[hal][core] spurious interrupt");
    }

    noop();
}

/**
 * @brief Wrapper for timer interrupt.
 */
static void do_timer(void)
{
    /* Increment timer value. */
    timer_value++;

    /* Forward timer interrupt handling. */
    if (timer_handler != NULL) {
        timer_handler();
    }
}

/**
 * @brief Change interrupt level, i.e., change the minimum interrupt
 * priority that can be accepted.
 */
static inline int interrupts_set_level(int newlevel)
{
    return (lpic_lvl_set(newlevel));
}

/**
 * @details Disables all hardware interrupts.
 */
void interrupts_disable(void)
{
    /* Disable underlying interrupts. */
    lpic_disable();
}

/**
 * @details Enables all hardware interrupts.
 */
void interrupts_enable(void)
{
    /* Enable underlying interrupts. */
    lpic_enable();
}

/**
 * The do_interrupt() function dispatches a hardware interrupt request
 * that was triggered to a previously-registered handler. If no
 * function was previously registered to handle the triggered hardware
 * interrupt request, this function returns immediately.
 *
 * @note This function is called from assembly code.
 *
 * @author Pedro Henrique Penna
 */
void do_interrupt(int intnum)
{
    lpic_ack(intnum);

    /* Nothing to do. */
    if (interrupt_handlers[intnum] == NULL) {
        return;
    }

    interrupt_handlers[intnum]();

    /*
     * Lets also check for external interrupt, if
     * there's any pending, handle.
     */
    while ((intnum = lpic_next()) != 0) {
        /* ack. */
        lpic_ack(intnum);

        /* Nothing to do. */
        if (interrupt_handlers[intnum] == NULL) {
            return;
        }

        /* Call handler. */
        interrupt_handlers[intnum]();
    }
}

/**
 * The interrupt_register() function registers @p handler as the
 * handler function for the interrupt whose number is @p num. If a
 * handler function was previously registered with this number, the
 * interrupt_register() function fails.
 */
int interrupt_register(int num, interrupt_handler_t handler)
{
    /* Invalid interrupt number. */
    if ((num < 0) || (num >= INTERRUPTS_NUM))
        return (-1);

    if (num != INTERRUPT_TIMER && num != INTERRUPT_IPI) {
        /* Handler function already registered. */
        if (interrupt_handlers[num] != NULL)
            return (-1);

        interrupt_handlers[num] = handler;
    } else {
        if (num == INTERRUPT_TIMER) {
            /* Handler function already registered. */
            if (timer_handler != NULL)
                return (-1);

            timer_handler = handler;
        } else {
            /* Handler function already registered. */
            if (event_handler != NULL)
                return (-1);

            event_handler = handler;
        }
    }

    lpic_unmask(num);

    kprintf("[hal][core] interrupt handler registered for irq %d", num);

    return (0);
}

/**
 * The interrupt_unregister() function unregisters a handler function
 * for the interrupt whose number is @p num. If no handler function
 * was previously registered with this number, the
 * interrupt_unregister() function fails.
 */
int interrupt_unregister(int num)
{
    /* Invalid interrupt number. */
    if ((num < 0) || (num >= INTERRUPTS_NUM))
        return (-1);

    if (num != INTERRUPT_TIMER && num != INTERRUPT_IPI) {
        /* No handler function is registered. */
        if (interrupt_handlers[num] == NULL)
            return (-1);

        interrupt_handlers[num] = NULL;
    } else {
        if (num == INTERRUPT_TIMER) {
            /* No handler function is registered. */
            if (timer_handler == NULL)
                return (-1);

            timer_handler = NULL;
        } else {
            /* No handler function is registered. */
            if (event_handler == NULL)
                return (-1);

            event_handler = NULL;
        }
    }

    lpic_mask(num);

    kprintf("[hal][core] interrupt handler unregistered for irq %d", num);

    return (0);
}

/**
 * @details Initializes hardware interrupts by registering a default handler to
 * all available interrupts.
 */
void interrupts_init(void)
{
    kputs("[hal][core] initializing interrupts...\n");

    for (int i = 0; i < INTERRUPTS_NUM; i++) {
        interrupt_handler_t handler;

        if (i == INTERRUPT_TIMER)
            handler = do_timer;
        else
            handler = default_handler;

        interrupt_handlers[i] = handler;
    }

    lpic_lvl_set(IRQLVL_4);
}
