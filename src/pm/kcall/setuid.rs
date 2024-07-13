// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    kcall::KcallArgs,
    pm::process::ProcessManager,
};
use ::kcall::{
    Error,
    UserIdentifier,
};

//==================================================================================================
// Standalone Functions
//==================================================================================================

fn do_setuid(pm: &mut ProcessManager, uid: UserIdentifier) -> Result<(), Error> {
    pm.setuid(uid)
}

pub fn setuid(pm: &mut ProcessManager, args: &KcallArgs) -> i32 {
    match do_setuid(pm, UserIdentifier::from(args.arg0)) {
        Ok(_) => 0,
        Err(e) => e.code.into_errno(),
    }
}
