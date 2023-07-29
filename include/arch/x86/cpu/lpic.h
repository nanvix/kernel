/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef ARCH_X86_CPU_LPIC_H_
#define ARCH_X86_CPU_LPIC_H_

#ifndef _ASM_FILE_
#include <stdint.h>
#endif /* !_ASM_FILE_ */

/**
 * @brief Number of interrupt lines.
 */
#define IRQ_NUM 16

/**
 * @brief Number of interrupts.
 */
#define INTERRUPTS_NUM IRQ_NUM

/**
 * @name Hardware Interrupts for the IBM PC Target
 */
/**@{*/
#define INTERRUPT_TIMER 0    /** Programmable interrupt timer.              */
#define INTERRUPT_KEYBOARD 1 /** Keyboard.                                  */
#define INTERRUPT_COM2 3     /** COM2.                                      */
#define INTERRUPT_COM1 4     /** COM1.                                      */
#define INTERRUPT_LPT2 5     /** LPT2.                                      */
#define INTERRUPT_FLOPPY 6   /** Floppy disk.                               */
#define INTERRUPT_LPT1 7     /** LPT1.                                      */
#define INTERRUPT_CMOS 8     /** CMOS real-time timer.                      */
#define INTERRUPT_SCSI1 9    /** Free for peripherals (legacy SCSI or NIC). */
#define INTERRUPT_SCSI2 10   /** Free for peripherals (legacy SCSI or NIC). */
#define INTERRUPT_SCSI3 11   /** Free for peripherals (legacy SCSI or NIC). */
#define INTERRUPT_MOUSE 12   /** PS2 mouse.                                 */
#define INTERRUPT_COPROC 13  /** FPU, coprocessor or inter-processor.       */
#define INTERRUPT_ATA1 14    /** Primary ATA hard disk.                     */
#define INTERRUPT_ATA2 15    /** Secondary ATA hard disk.                   */
#define INTERRUPT_IPI 256    /** Dummy IPI interrupt.                       */
/**@}*/

/**
 * @name Interrupt Levels
 */
/**@{*/
#define IRQLVL_5 5 /** Level 0: all hardware interrupts disabled. */
#define IRQLVL_4 4 /** Level 1: timer interrupts enabled.         */
#define IRQLVL_3 3 /** Level 2: disk interrupts enabled.          */
#define IRQLVL_2 2 /** Level 3: network interrupts enabled        */
#define IRQLVL_1 1 /** Level 4: terminal interrupts enabled.      */
#define IRQLVL_0 0 /** Level 5: all hardware interrupts enabled.  */
/**@}*/

/**
 * @name Master LPIC Registers
 */
/**@{*/
#define LPIC_CTRL_MASTER 0x20 /** Control */
#define LPIC_DATA_MASTER 0x21 /** Data    */
/**@}*/

#ifndef _ASM_FILE_

/**
 * @brief Initializes the LPIC.
 *
 * @param offset1 Vector offset for master PIC.
 * @param offset2 Vector offset for slave PIC.
 */
extern void lpic_init(uint8_t offset1, uint8_t offset2);

/**
 * @brief Masks an interrupt.
 *
 * @param irq Number of the target interrupt.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern int lpic_mask(int irq);

/**
 * @brief Unmasks an interrupt.
 *
 * @param irq Number of the target interrupt.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern int lpic_unmask(int irq);

/**
 * @brief Gets the interrupt level of the calling core.
 *
 * @returns The current interrupt level.
 */
extern int lpic_lvl_get(void);

/**
 * @brief Sets the interrupt level of the calling core.
 *
 * @param new_level New interrupt level.
 *
 * @returns The old interrupt level.
 */
extern int lpic_lvl_set(int new_level);

/**
 * @brief Gets the next pending interrupt.
 *
 * @returns The number of the next pending interrupt, or zero if
 * no interrupt is pending.
 */
extern int lpic_next(void);

/**
 * @brief Acknowledges an interrupt.
 *
 * @param irq Number of the target interrupt.
 */
extern void lpic_ack(int irq);

/**
 * @brief Enables hardware interrupts.
 *
 * The i486_hwint_enable() function enables all hardware interrupts in the
 * underlying i486 core.
 */
extern void lpic_enable(void);

/**
 * @brief Disables hardware interrupts.
 *
 * The i486_hwint_disable() function disables all hardware interrupts in the
 * underlying i486 core.
 */
extern void lpic_disable(void);

#endif /* !_ASM_FILE */

#endif /* ARCH_X86_CPU_LPIC_H_ */
