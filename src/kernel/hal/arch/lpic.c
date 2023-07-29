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
 * @name Slave Slave LPIC Registers
 */
/**@{*/
#define LPIC_CTRL_SLAVE 0xa0 /** Control register. */
#define LPIC_DATA_SLAVE 0xa1 /** Data register.    */
/**@}*/

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
 * Private Variables                                                          *
 *============================================================================*/

/**
 * @brief interrupt handlers.
 */
interrupt_handler_t interrupt_handlers[INTERRUPTS_NUM] = {NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL};

/*============================================================================*
 * Public Functions                                                      *
 *============================================================================*/

/**
 * @details The lpic_mask() function masks the interrupt request line in which
 * the interrupt @p irq is hooked up.
 */
int lpic_mask(int irq)
{
    uint16_t port;
    uint8_t value;
    uint16_t newmask;

    /* Invalid interrupt number. */
    if ((irq < 0) || (irq >= IRQ_NUM)) {
        return (-1);
    }

    if (irq < 8) {
        port = LPIC_DATA_MASTER;
        newmask = currmask | (1 << irq);
        value = newmask & 0xff;
    } else {
        port = LPIC_DATA_SLAVE;
        newmask = currmask | (1 << irq);
        value = (newmask >> 8) & 0xff;
    }

    currmask = newmask;

    output8(port, value);

    return (0);
}

/**
 * @details The lpic_unmask() function unmasks the interrupt request line in
 * which the interrupt @p irq is hooked up.
 */
int lpic_unmask(int irq)
{
    uint16_t port;
    uint8_t value;
    uint16_t newmask;

    /* Invalid interrupt number. */
    if ((irq < 0) || (irq >= IRQ_NUM))
        return (-1);

    if (irq < 8) {
        port = LPIC_DATA_MASTER;
        newmask = currmask & ~(1 << irq);
        value = newmask & 0xff;
    } else {
        port = LPIC_DATA_SLAVE;
        newmask = currmask & ~(1 << irq);
        value = (newmask >> 8) & 0xff;
    }

    currmask = newmask;

    output8(port, value);

    return (0);
}

/**
 * @details The lpic_get() function gets the interrupt level of the calling core
 * to @p new_level. The old interrupt level is returned.
 */
int lpic_lvl_get(void)
{
    return (currlevel);
}

/**
 * @details The lpic_set() function sets the interrupt level of the calling core
 * to @p new_level. The old interrupt level is returned.
 */
int lpic_lvl_set(int new_level)
{
    int oldlevel;
    uint16_t mask;

    mask = intlvl_masks[new_level];

    output8(LPIC_DATA_MASTER, mask & 0xff);
    output8(LPIC_DATA_SLAVE, mask >> 8);

    currmask = mask;
    oldlevel = currlevel;
    currlevel = new_level;

    return (oldlevel);
}

/**
 * @brief Acknowledges an interrupt.
 *
 * @param irq Number of the target interrupt.
 */
void lpic_ack(int irq)
{
    if (irq >= 8) {
        output8(LPIC_CTRL_SLAVE, LPIC_EOI);
    }

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
 * @brief Enables hardware interrupts.
 *
 * The i486_hwint_enable() function enables all hardware interrupts in the
 * underlying i486 core.
 */
void lpic_enable(void)
{
    asm("sti");
}

/**
 * @brief Disables hardware interrupts.
 *
 * The i486_hwint_disable() function disables all hardware interrupts in the
 * underlying i486 core.
 */
void lpic_disable(void)
{
    asm("cli");
}

/**
 * @details The lpic_init() function initializes the programmable interrupt
 * controller of the i486 core. Upon completion, it drops the interrupt level to
 * the slowest ones, so that all interrupt lines are enabled.
 */
void lpic_init(uint8_t offset1, uint8_t offset2)
{
    kprintf("[hal] initializing lpic...");

    /*
     * Starts initialization sequence
     * in cascade mode.
     */
    output8(LPIC_CTRL_MASTER, LPIC_ICW1_INIT | LPIC_ICW1_ICW4);
    iowait();
    output8(LPIC_CTRL_SLAVE, LPIC_ICW1_INIT | LPIC_ICW1_ICW4);
    iowait();

    /* Send new vector offset. */
    output8(LPIC_DATA_MASTER, offset1);
    iowait();
    output8(LPIC_DATA_SLAVE, offset2);
    iowait();

    /*
     * Tell the master that there is a slave
     * LPIC hired up at IRQ line 2 and tell
     * the slave LPIC that it is the second LPIC.
     */
    output8(LPIC_DATA_MASTER, 0x04);
    iowait();
    output8(LPIC_DATA_SLAVE, 0x02);
    iowait();

    /* Set 8086 mode. */
    output8(LPIC_DATA_MASTER, LPIC_ICW4_8086);
    iowait();
    output8(LPIC_DATA_SLAVE, LPIC_ICW4_8086);
    iowait();

    /* Clears interrupt mask. */
    lpic_lvl_set(IRQLVL_5);
}
