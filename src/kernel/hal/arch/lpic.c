/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#include <nanvix/kernel/hal.h>
#include <nanvix/kernel/lib.h>
#include <stdint.h>

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Name of this module.
 */
#define MODULE_NAME "[hal][cpu][lpci]]"

/**
 * @name Commands Codes
 */
/**@{*/
#define LPIC_EOI 0x20             /** End of Interrupt            */
#define LPIC_ICW1_ICW4 0x01       /** ICW4 (not) needed           */
#define LPIC_ICW1_SINGLE 0x02     /** Single (Cascade) mode       */
#define LPIC_ICW1_INTERVAL4 0x04  /** Call Address Interval 4 (8) */
#define LPIC_ICW1_LEVEL 0x08      /** Level triggered (edge) mode */
#define LPIC_ICW1_INIT 0x10       /** Initialization Required     */
#define LPIC_ICW4_8086 0x01       /** 8086/88 (MCS-80/85) Mode    */
#define LPIC_ICW4_AUTO 0x02       /** Auto EOI                    */
#define LPIC_ICW4_BUF_SLAVE 0x08  /** Buffered Mode/Slave         */
#define LPIC_ICW4_BUF_MASTER 0x0C /** Buffered Mode/Master        */
#define LPIC_ICW4_SFNM 0x10       /** Special Fully Nested (not)  */
/**@}*/

/**
 * @brief Number of interrupt levels.
 */
#define NUM_IRQLVL 6

/**
 * @name Interrupt Levels Masks
 */
/**@{*/
#define IRQLVL_MASK_5 0xfffb /** Mask for interrupt level 5.  */
#define IRQLVL_MASK_4 0xfefa /** Mask for interrupt level 4.  */
#define IRQLVL_MASK_3 0x3eba /** Mask for interrupt level 3.  */
#define IRQLVL_MASK_2 0x30ba /** Mask for interrupt level 2.  */
#define IRQLVL_MASK_1 0x2000 /** Mask for interrupt level 1.  */
#define IRQLVL_MASK_0 0x0000 /** Mask for interrupt level 0.  */
/**@}*/

/*============================================================================*
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief Masks of interrupt levels.
 *
 * Lookup table for masks of interrupt levels.
 */
static uint16_t intlvl_masks[NUM_IRQLVL] = {
    IRQLVL_MASK_0,
    IRQLVL_MASK_1,
    IRQLVL_MASK_2,
    IRQLVL_MASK_3,
    IRQLVL_MASK_4,
    IRQLVL_MASK_5,
};

/**
 * @brief Current interrupt level.
 *
 * Current interrupt level of the underlying i486 core.
 */
static int currlevel = IRQLVL_5;

/**
 * @brief Current interrupt mask.
 *
 * Current interrupt mask of the underlying i486 core.
 */
static uint16_t currmask = IRQLVL_MASK_5;

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

/**
 * @details This function masks the interrupt request line @p irq.
 */
int lpic_mask(int irq)
{
    uint16_t port;
    uint8_t value;
    uint16_t newmask;

    // Check for invalid interrupt request. IRQ 2 is reserved for the cascade.
    if (UNLIKELY((irq < 0) || (irq == 2) || (irq >= LPIC_NUM_IRQS))) {
        kprintf(MODULE_NAME " ERROR: mask invalid irq=%d", irq);
        return (-1);
    }

    // Check which pick owns the target IRQ.
    if (irq < LPIC_NUM_IRQS_MASTER) {
        // Mask IRQ in master PIC.
        port = LPIC_DATA_MASTER;
        newmask = currmask | (1 << irq);
        value = newmask & 0xff;
    } else {
        // Mask IRQ in slave PIC.
        port = LPIC_DATA_SLAVE;
        newmask = currmask | (1 << irq);
        value = (newmask >> 8) & 0xff;
    }

    currmask = newmask;

    output8(port, value);

    return (0);
}

/**
 * @details This function function unmasks the interrupt request line @p irq.
 */
int lpic_unmask(int irq)
{
    uint16_t port;
    uint8_t value;
    uint16_t newmask;

    // Check for invalid interrupt request. IRQ 2 is reserved for the cascade.
    if (UNLIKELY((irq < 0) || (irq == 2) || (irq >= LPIC_NUM_IRQS))) {
        kprintf(MODULE_NAME " ERROR: unmkask invalid irq=%d", irq);
        return (-1);
    }

    // Check which pick owns the target IRQ.
    if (irq < LPIC_NUM_IRQS_MASTER) {
        // Unmask IRQ in master PIC.
        port = LPIC_DATA_MASTER;
        newmask = currmask & ~(1 << irq);
        value = newmask & 0xff;
    } else {
        // Unmask IRQ in slave PIC.
        port = LPIC_DATA_SLAVE;
        newmask = currmask & ~(1 << irq);
        value = (newmask >> 8) & 0xff;
    }

    currmask = newmask;

    output8(port, value);

    return (0);
}

/**
 * @details This function gets the current interrupt level.
 */
int lpic_lvl_get(void)
{
    return (currlevel);
}

/**
 * @details This function sets the interrupt level to @p new_level. The old
 * interrupt level is returned.
 */
int lpic_lvl_set(int new_level)
{
    const uint16_t mask = intlvl_masks[new_level];

    output8(LPIC_DATA_MASTER, mask & 0xff);
    output8(LPIC_DATA_SLAVE, mask >> 8);

    currmask = mask;
    const int oldlevel = currlevel;
    currlevel = new_level;

    return (oldlevel);
}

/**
 * @details Acknowledges interrupt request @p irq.
 */
void lpic_ack(int irq)
{
    // Check for invalid interrupt request. IRQ 2 is reserved for the cascade.
    if (UNLIKELY((irq < 0) || (irq == 2) || (irq >= LPIC_NUM_IRQS))) {
        kprintf(MODULE_NAME " ERROR: ack invalid irq=%d", irq);
        return;
    }

    // Check if EOI is managed by slave PIC.
    if (irq >= LPIC_NUM_IRQS_MASTER) {
        // Send EOI to slave PIC.
        output8(LPIC_CTRL_SLAVE, LPIC_EOI);
    }

    // Send EOI to master PIC.
    output8(LPIC_CTRL_MASTER, LPIC_EOI);
}

/**
 * @details Gets the next pending interrupt.
 */
int lpic_next(void)
{
    return (0);
}

/**
 * @details This function enables all hardware interrupts in the underlying
 * core.
 */
void lpic_enable(void)
{
    asm("sti");
}

/**
 * @details This function disables all hardware interrupts in the underlying
 * core.
 */
void lpic_disable(void)
{
    asm("cli");
}

/**
 * @details This function initializes the programmable interrupt controller of
 * the underlying core. Upon completion, it raises the interrupt level to the
 * slowest one, so that all interrupt lines are disable.
 */
void lpic_init(unsigned hwint_off)
{
    kprintf(MODULE_NAME " initializing lpic...");

    // Starts initialization sequence in cascade mode.
    output8(LPIC_CTRL_MASTER, LPIC_ICW1_INIT | LPIC_ICW1_ICW4);
    iowait();
    output8(LPIC_CTRL_SLAVE, LPIC_ICW1_INIT | LPIC_ICW1_ICW4);
    iowait();

    // Send new vector offset.
    output8(LPIC_DATA_MASTER, hwint_off + 0);
    iowait();
    output8(LPIC_DATA_SLAVE, hwint_off + LPIC_NUM_IRQS_MASTER);
    iowait();

    // Tell the master that there is a slave
    // LPIC hired up at IRQ line 2 and tell
    // the slave LPIC that it is the second LPIC.
    output8(LPIC_DATA_MASTER, 0x04);
    iowait();
    output8(LPIC_DATA_SLAVE, 0x02);
    iowait();

    // Set 8086 mode.
    output8(LPIC_DATA_MASTER, LPIC_ICW4_8086);
    iowait();
    output8(LPIC_DATA_SLAVE, LPIC_ICW4_8086);
    iowait();

    // Clears interrupt mask.
    lpic_lvl_set(IRQLVL_5);
}
