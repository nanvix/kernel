// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

mod controller;
mod ioapic;
mod number;
mod pic;
mod vector;
mod xapic;

//==================================================================================================
// Imports
//==================================================================================================

use super::madt::madt::MadtInfo;
use crate::{
    arch::cpu::{
        eflags::{
            self,
            EflagsRegister,
        },
        madt::MadtEntryLocalApic,
    },
    error::Error,
    hal::{
        arch::x86::{
            cpu::idt,
            mem::gdt,
        },
        io::allocator::IoPortAllocator,
    },
};
use ioapic::IoapicPtr;
use pic::Pic;
use xapic::XapicRef;

//==================================================================================================
// Exports
//==================================================================================================

pub use controller::InterruptController;
pub use number::InterruptNumber;
pub use vector::InterruptHandlersRef;

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

/// Initializes the interrupt controller.
pub fn init(
    ioports: &mut IoPortAllocator,
    madt: Option<MadtInfo>,
) -> Result<InterruptController, Error> {
    info!("initializing interrupt controller...");
    match madt {
        // MADT is present.
        Some(madt) => {
            info!("retriving information from madt");
            // Check if the 8259 PIC is present.
            let pic: Option<Pic> = match madt.has_8259_pic() {
                true => {
                    info!("8259 pic found");
                    Some(pic::init(ioports, idt::INT_OFF)?)
                },
                false => {
                    info!("8259 pic not found");
                    None
                },
            };

            let ioapic: Option<IoapicPtr> = None;
            let xapic: Option<XapicRef> = None;

            InterruptController::new(pic, xapic, ioapic)
        },

        // MADT is not present.
        None => {
            info!("madt not present, falling back to 8259 pic");
            let pic: Pic = pic::init(ioports, idt::INT_OFF)?;
            Ok(InterruptController::new(Some(pic), None, None)?)
        },
    }
}
