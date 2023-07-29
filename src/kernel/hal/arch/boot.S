/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/* Must come first. */
#define _ASM_FILE_

/*============================================================================*
 * Imported Symbols                                                           *
 *============================================================================*/

#include <grub/mboot.h>
#include <arch/x86.h>
#include <asm/x86.S>

/**
 * @brief Multiboot flags.
 */
#define MBOOT_FLAGS (MBOOT_PAGE_ALIGN | MBOOT_MEMORY_INFO)

.extern kmain

/*============================================================================*
 * Exported Symbols                                                           *
 *============================================================================*/

.globl _do_start

/*============================================================================*
 * Bootstrap Section                                                          *
 *============================================================================*/

.section .bootstrap,"ax",@progbits

/*----------------------------------------------------------------------------*
 * multiboot_header                                                           *
 *----------------------------------------------------------------------------*/

/**
 * @brief Multiboot header.
 */
.align 4
 mboot_header:
    .long  MBOOT_MAGIC                  /* Magic number.              */
    .long  MBOOT_FLAGS                  /* Flags.                     */
    .long  -(MBOOT_MAGIC + MBOOT_FLAGS) /* Checksum.                  */
    .long  mboot_header                 /* Pointer to this structure. */


/*----------------------------------------------------------------------------*
 * _do_start()                                                                *
 *----------------------------------------------------------------------------*/

/*
 * Entry point.
 */
.align 4
_do_start:

    /* Reset stack. */
    reset_stack

    push %ebx

    /*
     * Clear all general purpose registers,
     * so we have a deterministic startup.
     */
    clear_gprs

    call kmain

    /* Halt execution. */
   halt

/*============================================================================*
 * Data Section                                                               *
 *============================================================================*/

.section .data

/*
 * Kernel stack.
 */
kstack:
    .skip PAGE_SIZE
