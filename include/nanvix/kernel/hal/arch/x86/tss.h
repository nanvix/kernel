/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_ARCH_X86_TSS_H_
#define NANVIX_KERNEL_HAL_ARCH_X86_TSS_H_

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
 * @param ss_selector IOPL 0 stack segment selector.
 *
 * @return Returns a pointer to the TSS.
 */
extern const struct tss *tss_init(unsigned ss_selector);

#endif /* !_ASM_FILE_ */

#endif /* NANVIX_KERNEL_HAL_ARCH_X86_TSS_H_ */
