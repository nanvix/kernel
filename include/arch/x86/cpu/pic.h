/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef ARCH_X86_CPU_PIC_H_
#define ARCH_X86_CPU_PIC_H_

/**
 * @addtogroup x86-cpu-pic x86 PIC
 * @ingroup x86
 *
 * @brief x86 PIC
 */
/**@{*/

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Number of interrupt request (IRQs) lines in the master PIC
 */
#define PIC_NUM_IRQS_MASTER 8

/**
 * @brief Number of interrupt request (IRQs) lines in the slave PIC
 */
#define PIC_NUM_IRQS_SLAVE 8

/**
 * @brief TOtal number of Interrupt Requests (IRQ) lines.
 */
#define PIC_NUM_IRQS (PIC_NUM_IRQS_MASTER + PIC_NUM_IRQS_SLAVE)

/*============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Initializes the PIC.
 *
 * @param hwint_off Vector offset for hardware interrupts.
 */
extern void pic_init(unsigned hwint_off);

/**
 * @brief Masks an interrupt.
 *
 * @param irq Number of the target interrupt.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern int pic_mask(int irq);

/**
 * @brief Unmasks an interrupt.
 *
 * @param irq Number of the target interrupt.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern int pic_unmask(int irq);

/**
 * @brief Gets the current interrupt level.
 *
 * @returns The current interrupt level.
 */
extern int pic_lvl_get(void);

/**
 * @brief Sets the interrupt level.
 *
 * @param new_level New interrupt level.
 *
 * @returns The old interrupt level.
 */
extern int pic_lvl_set(int new_level);

/**
 * @brief Gets the next pending interrupt.
 *
 * @returns The number of the next pending interrupt, or zero if
 * no interrupt is pending.
 */
extern int pic_next(void);

/**
 * @brief Acknowledges an interrupt.
 *
 * @param irq Number of the target interrupt request line.
 */
extern void pic_ack(int irq);

#endif /* !_ASM_FILE_ */

/**@}*/

#endif /* ARCH_X86_CPU_PIC_H_ */
