/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

#![no_std]
#![feature(panic_info_message)]

use core::arch;
pub mod unwind;

pub unsafe fn __kcall5(
    kcall_nr: u32,
    arg0: u32,
    arg1: u32,
    arg2: u32,
    arg3: u32,
    _arg4: u32,
) -> u32 {
    let ret: u32;
    arch::asm!("int 0x80",
        inout("eax") kcall_nr => ret,
        in("ebx") arg0,
        in("ecx") arg1,
        in("edx") arg2,
        in("edi") arg3,
        options(nostack, preserves_flags)
    );
    ret
}

#[no_mangle]
pub fn main() {
    unsafe { __kcall5(0, 1, 2, 3, 4, 5) };
    loop {}
}
