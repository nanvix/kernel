/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef ARCH_X86_CPU_IRQ_H_
#define ARCH_X86_CPU_IRQ_H_

/**
 * @addtogroup x86-cpu-irq x86 IRQ
 * @ingroup x86
 *
 * @brief x86 IRQ
 */
/**@{*/

/*============================================================================*
 * Public Functions                                                           *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Lookups the physical number of an IRQ.
 *
 * @param irq_logical Logical IRQ number.
 *
 * @returns Upon successful completion, the physical IRQ number associated to
 * logical IRQ number returned. Upon failure, a negative error code is returned
 * instead.
 */
extern int irq_lookup(int irq_logical);

/**
 * @brief Remaps an IRQ.
 *
 * @param irq_logical Logical IRQ number.
 * @param irq_physical_new New physical IRQ number.
 *
 * @returns Upon successful completion, the old physical IRQ number that was
 * mapped to the logical IRQ number is returned.  Upon failure, a negative error
 * code is returned instead.
 */
extern int irq_remap(int irq_logical, int irq_physical_new);

#endif /* !_ASM_FILE_ */

/*============================================================================*/

/**@}*/

#endif /* ARCH_X86_CPU_IRQ_H_ */
