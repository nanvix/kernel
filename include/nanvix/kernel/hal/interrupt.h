/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_INTERRUPT_H_
#define NANVIX_KERNEL_HAL_INTERRUPT_H_

#include <nanvix/kernel/hal/arch.h>

/**
 * @brief Threshold for spurious interrupts.
 *
 * INTERRUPT_SPURIOUS_THRESHOLD states the number of spurious
 * interrupts that we are willing to get, before enter in verbose
 * mode.
 */
#define INTERRUPT_SPURIOUS_THRESHOLD 100

#ifndef _ASM_FILE_

/**
 * @brief Hardware interrupt handler.
 */
typedef void (*interrupt_handler_t)(void);

/**
 * @brief High-level hardware interrupt dispatcher.
 *
 * @param intnum Number of the interrupt.
 */
extern void do_interrupt(int intnum);

/**
 * @brief Registers an interrupt handler.
 *
 * @param num     Number of the interrupt.
 * @param handler Interrupt handler.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern int interrupt_register(int num, interrupt_handler_t handler);

/**
 * @brief Unregisters an interrupt handler.
 *
 * @param num Number of the interrupt.
 *
 * @returns Upon successful completion, zero is returned. Upon
 * failure, a negative error code is returned instead.
 */
extern int interrupt_unregister(int num);

/**
 * @brief Disables all hardware interrupts.
 */
extern void interrupts_disable(void);

/**
 * @brief Enables all hardware interrupts.
 */
extern void interrupts_enable(void);

/**
 * @brief Setups hardware interrupts.
 */
extern void interrupts_init(void);

/**
 * @brief interrupt handlers.
 */
extern interrupt_handler_t interrupt_handlers[INTERRUPTS_NUM];

#endif /* !_ASM_FILE_ */

#endif /* NANVIX_KERNEL_HAL_INTERRUPT_H_ */
