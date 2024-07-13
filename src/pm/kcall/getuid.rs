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

fn do_getuid(pm: &ProcessManager) -> Result<UserIdentifier, Error> {
    pm.getuid()
}

pub fn getuid(pm: &ProcessManager, _args: &KcallArgs) -> i32 {
    match do_getuid(pm) {
        Ok(uid) => uid.into(),
        Err(e) => e.code.into_errno(),
    }
}
