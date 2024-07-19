// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::event::dispatcher::Dispatcher;
use ::kcall::EventInformation;

//==================================================================================================
// Standalone Functions
//==================================================================================================

pub fn wait(info: *mut EventInformation, interrupts: usize, exceptions: usize) -> i32 {
    let dispatcher: &mut Dispatcher = match Dispatcher::try_get_mut() {
        Ok(dispatcher) => dispatcher,
        Err(e) => return e.code.into_errno(),
    };

    match dispatcher.do_wait(info, interrupts, exceptions) {
        Ok(()) => 0,
        Err(e) => e.code.into_errno(),
    }
}
