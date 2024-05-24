/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/errno.h>
#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <nanvix/kernel/log.h>

/*============================================================================*
 * Private Functions                                                          *
 *============================================================================*/

/**
 * @brief Maps logical IRQs to physical IRQs.
 *
 * @note We perform static initialization, as this is required to be initialized
 * in early startup stages of the kernel.
 */
static uint8_t irqs[PIC_NUM_IRQS] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Remaps an IRQ.
 */
int irq_remap(uint8_t irq_logical, uint8_t irq_physical_new)
{
    // Check if the logical IRQ is valid.
    if (irq_logical >= PIC_NUM_IRQS) {
        error("invalid logical irq number (irq=%d)", irq_physical_new);
        return (-EINVAL);
    }

    // Remap IRQ.
    int irq_physical_old = irqs[irq_logical];
    irqs[irq_logical] = irq_physical_new;

    // Issue an INFO message, as this may severely impact the system.
    info("remapped irq %d to %d", irq_logical, irq_physical_new);

    return (irq_physical_old);
}

/**
 * @details Lookups the physical number of an IRQ.
 */
int irq_lookup(uint8_t irq_logical)
{
    // Check if logical the IRQ is valid.
    if (irq_logical >= PIC_NUM_IRQS) {
        error("invalid logical irq number (irq=%d)", irq_logical);
        return (-EINVAL);
    }

    return (irqs[irq_logical]);
}
