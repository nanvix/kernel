/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

/*============================================================================*
 * Imported Symbols                                                           *
 *============================================================================*/

.extern main

/*============================================================================*
 * Exported Symbols                                                           *
 *============================================================================*/

.globl _do_start
.globl memset
.globl memcpy
.globl memcmp

/*============================================================================*
 * Text Section                                                               *
 *============================================================================*/

.text

_do_start:
    movl %esp, %ebp
    call main
1:  jmp 1b

/*
 *  +8 = Destination
 * +12 = Value
 * +16 = Count
*/
.section .text.memset
memset:
	push %ebp
	mov %esp, %ebp
	push %edi

	mov 8(%ebp), %edi
	mov 12(%ebp), %eax
	mov 16(%ebp), %ecx
	rep stosb

	pop %edi
	pop %ebp
	ret
/*
 *  +8 = Destination
 * +12 = Source
 * +16 = Count
*/
.section .text.memcpy
memcpy:
	push %ebp
	mov %esp, %ebp
	push %esi
	push %edi

	mov 8(%ebp), %edi
	mov 12(%ebp), %esi
	mov 16(%ebp), %ecx

	rep movsb

	pop %edi
	pop %esi
	pop %ebp
	ret


/*
 *  +8 = Destination
 * +12 = Source
 * +16 = Count
*/
.section .text.memcmp
memcmp:
    push %ebp
    mov %esp, %ebp
	push %esi
	push %edi

    mov 8(%ebp), %esi
    mov 12(%ebp), %edi
    mov 16(%ebp), %ecx

    repe cmpsb

    seta %al
    setb %ah
    sub %ah, %al

	pop %edi
	pop %esi
	pop %ebp
	ret
