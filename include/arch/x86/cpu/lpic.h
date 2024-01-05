/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef ARCH_X86_CPU_LPIC_H_
#define ARCH_X86_CPU_LPIC_H_

/**
 * @addtogroup x86-cpu-lpic x86 LPIC
 * @ingroup x86
 *
 * @brief x86 LPIC
 */
/**@{*/

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Number of interrupt request (IRQs) lines in the master LPIC
 */
#define LPIC_NUM_IRQS_MASTER 8

/**
 * @brief Number of interrupt request (IRQs) lines in the slave LPIC
 */
#define LPIC_NUM_IRQS_SLAVE 8

/**
 * @brief TOtal number of Interrupt Requests (IRQ) lines.
 */
#define LPIC_NUM_IRQS (LPIC_NUM_IRQS_MASTER + LPIC_NUM_IRQS_SLAVE)

/**
 * @name Master LPIC Registers
 */
/**@{*/
#define LPIC_CTRL_MASTER 0x20 /** Control */
#define LPIC_DATA_MASTER 0x21 /** Data    */
/**@}*/

/**
 * @name Slave Slave LPIC Registers
 */
/**@{*/
#define LPIC_CTRL_SLAVE 0xa0 /** Control register. */
#define LPIC_DATA_SLAVE 0xa1 /** Data register.    */
/**@}*/

/*============================================================================*/

/**@}*/

#endif /* ARCH_X86_CPU_LPIC_H_ */
