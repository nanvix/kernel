/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_ARCH_X86_IDT_H_
#define NANVIX_KERNEL_HAL_ARCH_X86_IDT_H_

/**
 * @addtogroup x86-cpu-idt x86 IDT
 * @ingroup x86
 */
/**@{*/

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Trap gate.
 */
#define TRAP_GATE 0x80

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Initializes the Interrupt Descriptor Table (IDT).
 *
 * @param cs_selector IOPL 0 code segment selector.
 */
extern void idt_init(unsigned cs_selector);

#endif /* !_ASM_FILE_ */

/*============================================================================*/

/**@}*/

#endif /* NANVIX_KERNEL_HAL_ARCH_X86_IDT_H_ */
