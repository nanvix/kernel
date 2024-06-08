// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

use crate::{
    arch::cpu::eflags::{
        self,
        EflagsRegister,
    },
    error::{
        Error,
        ErrorCode,
    },
    hal::arch::x86::mem::gdt,
};

//==================================================================================================
// Structures
//==================================================================================================

pub type InterruptHandler = fn();

pub struct InterruptHandlersRef(&'static mut [Option<InterruptHandler>; 256]);

#[derive(Debug, Clone, Copy)]
#[repr(u32)]
pub enum InterruptNumber {
    Timer = 0,
    Keyboard = 1,
    Com2 = 3,
    Com1 = 4,
    Lpt2 = 5,
    Floppy = 6,
    Lpt1 = 7,
    Cmos = 8,
    Free1 = 9,
    Free2 = 10,
    Free3 = 11,
    Mouse = 12,
    Coprocessor = 13,
    PrimaryAta = 14,
    SecondaryAta = 15,
}

impl InterruptNumber {
    pub const VALUES: [InterruptNumber; 15] = [
        InterruptNumber::Timer,
        InterruptNumber::Keyboard,
        InterruptNumber::Com2,
        InterruptNumber::Com1,
        InterruptNumber::Lpt2,
        InterruptNumber::Floppy,
        InterruptNumber::Lpt1,
        InterruptNumber::Cmos,
        InterruptNumber::Free1,
        InterruptNumber::Free2,
        InterruptNumber::Free3,
        InterruptNumber::Mouse,
        InterruptNumber::Coprocessor,
        InterruptNumber::PrimaryAta,
        InterruptNumber::SecondaryAta,
    ];
}

//==================================================================================================
// Global Variables
//==================================================================================================

#[no_mangle]
static mut INTERRUPT_HANDLERS: [Option<InterruptHandler>; 256] = [None; 256];

//==================================================================================================
// Implementations
//==================================================================================================

static mut BORROWED: bool = false;

impl InterruptHandlersRef {
    pub fn try_borrow() -> Result<InterruptHandlersRef, Error> {
        unsafe {
            if BORROWED {
                Err(Error::new(ErrorCode::ResourceBusy, "interrupts already initialized"))
            } else {
                Ok(InterruptHandlersRef(&mut INTERRUPT_HANDLERS))
            }
        }
    }

    pub fn get_handler(&self, intnum: InterruptNumber) -> Option<InterruptHandler> {
        self.0[intnum as usize]
    }

    pub fn set_handler(&mut self, intnum: InterruptNumber, handler: Option<InterruptHandler>) {
        self.0[intnum as usize] = handler;
    }
}

impl Drop for InterruptHandlersRef {
    fn drop(&mut self) {
        unsafe {
            BORROWED = false;
        }
    }
}

//==================================================================================================
// Standalone Functions
//==================================================================================================

///
/// # Description
///
/// Forges a stack frame that is suitable for returning from an interrupt with privilege transition.
///
/// # Parameters
///
/// - `kernel_stack_top`: Pointer to the top of the kernel stack.
/// - `user_stack_top`: Top address of user stack.
/// - `user_func`: User function.
/// - `kernel_func`: Kernel function.
///
/// # Returns
///
/// A pointer to the forged stack frame.
///
/// # Safety
///
/// Behavior is undefined if any of the following conditions are violated:
///
/// - `stackp` must point to a valid location in memory.
///
pub unsafe fn forge_user_stack(
    kernel_stack_top: *mut u8,
    user_stack_top: usize,
    user_func: usize,
    kernel_func: usize,
) -> *mut u8 {
    let mut stackp: *mut u32 = kernel_stack_top as *mut u32;

    // User DS
    stackp = stackp.offset(-1);
    *stackp = gdt::SegmentSelector::UserData as u32;

    // User ESP
    stackp = stackp.offset(-1);
    *stackp = user_stack_top as u32;

    // EFLAGS
    let mut eflags: EflagsRegister = eflags::EflagsRegister::default();
    eflags.interrupt = eflags::InterruptFlag::Set;
    stackp = stackp.offset(-1);
    *stackp = eflags.into_raw_value();

    // User CS
    stackp = stackp.offset(-1);
    *stackp = gdt::SegmentSelector::UserCode as u32;

    // User EIP
    stackp = stackp.offset(-1);
    *stackp = user_func as u32;

    // Kernel EIP
    stackp = stackp.offset(-1);
    *stackp = kernel_func as u32;

    stackp as *mut u8
}
