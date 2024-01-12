/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/* Must come first. */
#define _ASM_FILE_

/*============================================================================*
 * Imported Symbols                                                           *
 *============================================================================*/

#include <grub/mboot2.h>
#include <arch/x86.h>
#include <asm/x86.S>

.extern kmain

/*============================================================================*
 * Exported Symbols                                                           *
 *============================================================================*/

.globl _do_start
.globl kstack

/*============================================================================*
 * Bootstrap Section                                                          *
 *============================================================================*/

.section .bootstrap,"ax",@progbits

/*----------------------------------------------------------------------------*
 * mboot_header                                                           *
 *----------------------------------------------------------------------------*/

/*
 *  Multiboot2 header.
 */
.align 8
mboot_header:
        .long   MULTIBOOT2_HEADER_MAGIC /*  Magic number. */
        .long  MBOOT_ARCHITECTURE_I386 /* Architecture */
        .long   mboot_header_end - mboot_header /* Header Length */
        /*  checksum */
        .long   -(MULTIBOOT2_HEADER_MAGIC + MBOOT_ARCHITECTURE_I386 + (mboot_header_end - mboot_header))
        framebuffer_tag_end:
        .short MBOOT_HEADER_TAG_END
        .short 0
        .long 8
mboot_header_end:

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

    /* Save multiboot parameters on the stack. */
    push %ebx /* Address of multiboot2 structure. */
    push %eax /* Magic value.                     */

    /*
     * Clear all general purpose registers,
     * so we have a deterministic startup.
     */
    clear_gprs

    /* Call kernel main function. */
    push %esp
    call kmain
    addl $4, %esp

    /* Cleanup multiboot parameters. */
    addl $8, %esp

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
