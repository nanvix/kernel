// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    arch::cpu::excp::Exception,
    hal::arch::x86::cpu::context::ContextInformation,
};

//==================================================================================================
// Structures
//==================================================================================================

///
/// # Description
///
/// Stores information about an exception.
///
#[repr(C, packed)]
pub struct ExceptionInformation {
    /// Exception number.
    num: u32,
    /// Error code.
    code: u32,
    /// Faulting address.
    addr: u32,
    /// Faulting instruction.
    instruction: u32,
}

// `ExceptionInformation` must be 16 bytes long. This must match low-level assembly dispatcher code.
static_assert_size!(ExceptionInformation, 16);

//==================================================================================================
// Trait Implementations
//==================================================================================================

impl core::fmt::Debug for ExceptionInformation {
    fn fmt(&self, f: &mut core::fmt::Formatter) -> core::fmt::Result {
        // Copy fields to local variables.
        let num: u32 = self.num;
        let code: u32 = self.code;
        let addr: u32 = self.addr;
        let instr: u32 = self.instruction;
        let excp: Exception = num.into();
        write!(
            f,
            "{:?} (error code={}, faulting addr={:#010x}, faulting instruction={:#010x})",
            excp, code, addr, instr
        )
    }
}

//==================================================================================================
// Standalone Functions
//==================================================================================================

///
/// # Description
///
/// High-level exception dispatcher.
///
/// # Parameters
///
/// - `excp` Exception information.
/// - `ctx`  Context information.
///
#[no_mangle]
pub extern "C" fn do_exception(excp: *const ExceptionInformation, ctx: *const ContextInformation) {
    let excp: &ExceptionInformation = unsafe { &*excp };
    let ctx: &ContextInformation = unsafe { &*ctx };
    info!("{:?}", excp);
    info!("{:?}", ctx);
    panic!("unhandled exception")
}
