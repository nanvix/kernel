/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_ARCH_X86_LPIC_H_
#define NANVIX_KERNEL_HAL_ARCH_X86_LPIC_H_

/**
 * @addtogroup x86-cpu-lpic x86 LPIC
 * @ingroup x86
 */
/**@{*/

/*============================================================================*
 * Imports                                                                    *
 *============================================================================*/

#ifndef _ASM_FILE_
#include <stdint.h>
#endif /* !_ASM_FILE_ */

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @name Interrupt Levels
 */
/**@{*/
#define IRQLVL_5 5 /** Level 5: all hardware interrupts disabled. */
#define IRQLVL_4 4 /** Level 4: timer interrupts enabled.         */
#define IRQLVL_3 3 /** Level 3: disk interrupts enabled.          */
#define IRQLVL_2 2 /** Level 2: network interrupts enabled        */
#define IRQLVL_1 1 /** Level 1: terminal interrupts enabled.      */
#define IRQLVL_0 0 /** Level 0: all hardware interrupts enabled.  */
/**@}*/

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Initializes the LPIC.
 *
 * @param hwint_off Vector offset for hardware interrupts.
 */
extern void lpic_init(unsigned hwint_off);

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
 * @brief Gets the current interrupt level.
 *
 * @returns The current interrupt level.
 */
extern int lpic_lvl_get(void);

/**
 * @brief Sets the interrupt level.
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
 * @param irq Number of the target interrupt request line.
 */
extern void lpic_ack(int irq);

#endif /* !_ASM_FILE */

/*============================================================================*/

/**@}*/

#endif /* NANVIX_KERNEL_HAL_ARCH_X86_LPIC_H_ */
