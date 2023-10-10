/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

.globl _do_start
.extern main

.text

_do_start:
    movl %esp, %ebp
    call main
1:  jmp 1b
