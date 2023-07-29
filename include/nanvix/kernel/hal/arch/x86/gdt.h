/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef NANVIX_KERNEL_HAL_ARCH_X86_GDT_H_
#define NANVIX_KERNEL_HAL_ARCH_X86_GDT_H_

/**
 * @addtogroup x86-cpu-gdt x86 GDT
 * @ingroup x86
 */
/**@{*/

/*============================================================================*
 * Functions                                                                  *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Initializes the Global Descriptor Table (GDT).
 */
extern void gdt_init(void);

/**
 * @brief Returns the segment selector of the kernel code segment.
 *
 * @returns The segment selector of the kernel code segment.
 */
extern unsigned gdt_kernel_cs(void);

#endif /* !_ASM_FILE_ */

/*============================================================================*/

/**@}*/

#endif /* NANVIX_KERNEL_HAL_ARCH_X86_GDT_H_ */
