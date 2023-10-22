/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

//==============================================================================
// Imports
//==============================================================================

use core::arch;

//==============================================================================
// Enumerations
//==============================================================================

#[repr(C)]
enum KcallNumbers {
    Void0 = 0,
    Void1 = 1,
    Void2 = 2,
    Void3 = 3,
    Void4 = 4,
    Shutdown = 6,
    Write = 7,
}

//==============================================================================
// Private Standalone Functions
//==============================================================================

///
/// **Description**
///
/// Issues a kernel call with no arguments.
///
/// **Parameters**
/// - `kcall_nr` - Kernel call number.
///
/// **Return**
///
/// This function returns the value returned by the kernel call.
///
#[inline]
unsafe fn kcall0(kcall_nr: u32) -> u32 {
    let ret: u32;
    arch::asm!("int 0x80",
        inout("eax") kcall_nr => ret,
        options(nostack, preserves_flags)
    );
    ret
}

///
/// **Description**
///
/// Issues a kernel call with one argument.
///
/// **Parameters**
/// - `kcall_nr` - Kernel call number.
/// - `arg0` - First argument for the kernel call.
///
/// **Return**
///
/// This function returns the value returned by the kernel call.
///
#[inline]
unsafe fn kcall1(kcall_nr: u32, arg0: u32) -> u32 {
    let ret: u32;
    arch::asm!("int 0x80",
        inout("eax") kcall_nr => ret,
        in("ebx") arg0,
        options(nostack, preserves_flags)
    );
    ret
}

///
/// **Description**
///
/// Issues a kernel call with two arguments.
///
/// **Parameters**
/// - `kcall_nr` - Kernel call number.
/// - `arg0` - First argument for the kernel call.
/// - `arg1` - Second argument for the kernel call.
///
/// **Return**
///
/// This function returns the value returned by the kernel call.
///
#[inline]
unsafe fn kcall2(kcall_nr: u32, arg0: u32, arg1: u32) -> u32 {
    let ret: u32;
    arch::asm!("int 0x80",
        inout("eax") kcall_nr => ret,
        in("ebx") arg0,
        in("ecx") arg1,
        options(nostack, preserves_flags)
    );
    ret
}

///
/// **Description**
///
/// Issues a kernel call with three arguments.
///
/// **Parameters**
/// - `kcall_nr` - Kernel call number.
/// - `arg0` - First argument for the kernel call.
/// - `arg1` - Second argument for the kernel call.
/// - `arg2` - Third argument for the kernel call.
///
/// **Return**
///
/// This function returns the value returned by the kernel call.
///
#[inline]
unsafe fn kcall3(kcall_nr: u32, arg0: u32, arg1: u32, arg2: u32) -> u32 {
    let ret: u32;
    arch::asm!("int 0x80",
        inout("eax") kcall_nr => ret,
        in("ebx") arg0,
        in("ecx") arg1,
        in("edx") arg2,
        options(nostack, preserves_flags)
    );
    ret
}

///
/// **Description**
///
/// Issues a kernel call with four arguments.
///
/// **Parameters**
/// - `kcall_nr` - Kernel call number.
/// - `arg0` - First argument for the kernel call.
/// - `arg1` - Second argument for the kernel call.
/// - `arg2` - Third argument for the kernel call.
/// - `arg3` - Fourth argument for the kernel call.
///
/// **Return**
///
/// This function returns the value returned by the kernel call.
///
#[inline]
unsafe fn kcall4(
    kcall_nr: u32,
    arg0: u32,
    arg1: u32,
    arg2: u32,
    arg3: u32,
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

//==============================================================================
// Public Standalone Functions
//==============================================================================

///
/// **Description**
///
/// Issues a void kernel call that takes no arguments.
///
/// **Return**
///
///  This function always returns zero.
///
pub fn void0() -> u32 {
    unsafe { kcall0(KcallNumbers::Void0 as u32) }
}

///
/// **Description**
///
/// Issues a void kernel call that takes one argument.
///
/// **Parameters**
/// - `arg0` - First argument for the kernel call.
///
/// **Return**
///
///  This function returns `arg0`
///
pub fn void1(arg0: u32) -> u32 {
    unsafe { kcall1(KcallNumbers::Void1 as u32, arg0) }
}

///
/// **Description**
///
/// Issues a void kernel call that takes two arguments.
///
/// **Parameters**
/// - `arg0` - First argument for the kernel call.
/// - `arg1` - Second argument for the kernel call.
///
/// **Return**
///
/// This function returns `arg0 + arg1`
///
pub fn void2(arg0: u32, arg1: u32) -> u32 {
    unsafe { kcall2(KcallNumbers::Void2 as u32, arg0, arg1) }
}

///
/// **Description**
///
/// Issues a void kernel call that takes three arguments.
///
/// **Parameters**
/// - `arg0` - First argument for the kernel call.
/// - `arg1` - Second argument for the kernel call.
/// - `arg2` - Third argument for the kernel call.
///
/// **Return**
///
/// This function returns `arg0 + arg1 + arg2`
///
pub fn void3(arg0: u32, arg1: u32, arg2: u32) -> u32 {
    unsafe { kcall3(KcallNumbers::Void3 as u32, arg0, arg1, arg2) }
}

///
/// **Description**
///
/// Issues a void kernel call that takes four arguments.
///
/// **Parameters**
/// - `arg0` - First argument for the kernel call.
/// - `arg1` - Second argument for the kernel call.
/// - `arg2` - Third argument for the kernel call.
/// - `arg3` - Fourth argument for the kernel call.
///
/// **Return**
///
/// This function returns `arg0 + arg1 + arg2 + arg3`
///
pub fn void4(arg0: u32, arg1: u32, arg2: u32, arg3: u32) -> u32 {
    unsafe { kcall4(KcallNumbers::Void4 as u32, arg0, arg1, arg2, arg3) }
}

///
/// **Description**
///
/// Writes a buffer to the kernel's standard output device.
///
/// **Parameters**
/// - `fd` - File descriptor.
/// - `buf` - Buffer to write.
/// - `size` - Number of bytes to write.
///
/// **Return**
///
/// The number of bytes written is returned.
///
pub fn write(fd: u32, buf: *const u8, size: usize) -> u32 {
    unsafe { kcall3(KcallNumbers::Write as u32, fd, buf as u32, size as u32) }
}

///
/// **Description**
///
/// Shutdowns the system.
///
pub fn shutdown() -> ! {
    unsafe {
        kcall0(KcallNumbers::Shutdown as u32);
    }
    // Never gets here.
    loop {}
}
