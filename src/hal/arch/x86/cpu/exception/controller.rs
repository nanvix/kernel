// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use super::ExceptionInformation;
use crate::hal::arch::x86::cpu::context::ContextInformation;

//==================================================================================================
// Types
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
    panic!("unhandled exception");
}
