/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#ifndef ARCH_X86_CPU_GDT_H_
#define ARCH_X86_CPU_GDT_H_

/**
 * @addtogroup x86-cpu-gdt x86 GDT
 * @ingroup x86
 *
 * @brief x86 GDT
 */
/**@{*/

/*============================================================================*
 * Constants                                                                  *
 *============================================================================*/

/**
 * @brief Size of a GDT entry (in bytes).
 */
#define GDTE_SIZE 8

/**
 * @brief Size of GDTPTR (in bytes).
 */
#define GDTPTR_SIZE 6

/*============================================================================*
 * Structures                                                                 *
 *============================================================================*/

#ifndef _ASM_FILE_

/**
 * @brief Global descriptor table entry (GDTE).
 */
struct gdte {
    unsigned limit_low : 16;  /** Limit low.   */
    unsigned base_low : 24;   /** Base low.    */
    unsigned access : 8;      /** Access.      */
    unsigned limit_high : 4;  /** Limit high.  */
    unsigned granularity : 4; /** Granularity. */
    unsigned base_high : 8;   /** Base high.   */
} __attribute__((packed));

/**
 * @brief Global descriptor table pointer (GDTPTR).
 */
struct gdtptr {
    unsigned size : 16; /** GDT size.            */
    unsigned ptr : 32;  /** GDT virtual address. */
} __attribute__((packed));

#endif /* _ASM_FILE_ */

/*============================================================================*/

/**@}*/

#endif /* ARCH_X86_CPU_GDT_H_ */
