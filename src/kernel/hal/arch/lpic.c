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
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details Masks an interrupt.
 */
int lpic_mask(int irq)
{
    return (pic_mask(irq));
}

/**
 * @details Unmasks an interrupt.
 */
int lpic_unmask(int irq)
{
    return (pic_unmask(irq));
}

/**
 * @details Gets the current interrupt level.
 */
int lpic_lvl_get(void)
{
    return (pic_lvl_get());
}

/**
 * @details Sets the interrupt level.
 */
int lpic_lvl_set(int new_level)
{
    return pic_lvl_set(new_level);
}

/**
 * @details Gets the next pending interrupt.
 */
int lpic_next(void)
{
    return (pic_next());
}

/**
 * @details Acknowledges an interrupt.
 */
void lpic_ack(int irq)
{
    pic_ack(irq);
}

/**
 * @details Initializes the LPIC.
 */
void lpic_init(unsigned hwint_off)
{
    pic_init(hwint_off);

    // Initialize I/O APIC.
    // TODO: Support configurations without I/O APICs.
    struct madt_ioapic_info info = {0};
    KASSERT(madt_ioapic_get_info(&info) == 0);
    KASSERT(ioapic_init(info.id, info.addr, info.gsi) == 0);
}
