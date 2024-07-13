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
    GroupIdentifier,
};

//==================================================================================================
// Standalone Functions
//==================================================================================================

fn do_getgid(pm: &ProcessManager) -> Result<GroupIdentifier, Error> {
    pm.getgid()
}

pub fn getgid(pm: &ProcessManager, _args: &KcallArgs) -> i32 {
    match do_getgid(pm) {
        Ok(gid) => gid.into(),
        Err(e) => e.code.into_errno(),
    }
}
