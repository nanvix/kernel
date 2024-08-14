/*
 * Copyright(c) The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imported Symbols                                                           *
 *============================================================================*/

.extern _do_ap_start

/*============================================================================*
 * Exported Symbols                                                           *
 *============================================================================*/

.globl _ap_trampoline

/*----------------------------------------------------------------------------*
 * _ap_trampoline()                                                           *
 *----------------------------------------------------------------------------*/

.section .trampoline,"ax",@progbits

/*
 * AP Entry point.
 *
 * This is the initial boot code for the application processors.
 * It is responsible for setting up the stack and jumping to the
 * C entry point.
 * This code is copied to MP_BOOT_ADDR which is not linked,
 * thus we must write explicit addresses.
 */

#define SEG_NULLASM                                             \
        .word 0, 0;                                             \
        .byte 0, 0, 0, 0

# The 0xC0 means the limit is in 4096-byte units
# and (for executable segments) 32-bit mode.
#define SEG_ASM(type,base,lim)                                  \
        .word (((lim) >> 12) & 0xffff), ((base) & 0xffff);      \
        .byte (((base) >> 16) & 0xff), (0x90 | (type)),         \
                (0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)

#define STA_X     0x8       // Executable segment
#define STA_W     0x2       // Writeable (non-executable segments)
#define STA_R     0x2       // Readable (executable segments)

.align 4
.code16
_ap_trampoline:
  cli

  # Zero data segment registers DS, ES, and SS.
  xorw    %ax,%ax
  movw    %ax,%ds
  movw    %ax,%es
  movw    %ax,%fs
  movw    %ax,%gs
  movw    %ax,%ss

  lgdt   gdtptr
  movl    %cr0, %eax
  orl     $1, %eax
  mov    %eax, %cr0
  jmpl $0x8, $_do_ap_start

.p2align 2
gdt:
  SEG_NULLASM
  SEG_ASM(STA_X|STA_R, 0, 0xffffffff)
  SEG_ASM(STA_W, 0, 0xffffffff)

gdtptr:
  .word   (gdtptr - gdt - 1)
  .long   gdt
