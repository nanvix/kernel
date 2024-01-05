/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
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
 * @brief Gets the segment selector for the kernel code segment.
 *
 * @returns The segment selector of the kernel code segment.
 */
extern unsigned gdt_kernel_cs(void);

/**
 * @brief Gets the segment selector for the user code segment.
 *
 * @return The segment selector for the user code segment.
 */
extern unsigned gdt_user_cs(void);

/**
 * @brief Gets the segment selector for the user data segment.
 *
 * @return The segment selector for the user data segment.
 */
extern unsigned gdt_user_ds(void);

#endif /* !_ASM_FILE_ */

/*============================================================================*/

/**@}*/

#endif /* NANVIX_KERNEL_HAL_ARCH_X86_GDT_H_ */
