/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_ARCH_X86_TSS_H_
#define NANVIX_KERNEL_HAL_ARCH_X86_TSS_H_

/**
 * @addtogroup x86-cpu-tss x86 TSS
 * @ingroup x86
 *
 * @brief x86 TSS
 */
/**@{*/

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Loads the Task State Segment (TSS).
 *
 * @param tss_selector TSS selector.
 */
extern void tss_load(unsigned tss_selector);

/**
 * @brief Initializes the Task State Segment (TSS).
 *
 * @param ss0 GDT selector for ring 0 data segment.
 *
 * @return Returns a pointer to the TSS.
 */
extern const struct tss *tss_init(unsigned ss0);

#endif /* !_ASM_FILE_ */

/*============================================================================*/

/**@}*/

#endif /* NANVIX_KERNEL_HAL_ARCH_X86_TSS_H_ */
