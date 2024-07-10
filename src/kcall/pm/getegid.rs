// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    kcall::KcallArgs,
    pm::process::{
        ProcessManager,
        RunningProcess,
    },
};
use ::kcall::{
    Error,
    GroupIdentifier,
};

//==================================================================================================
// Standalone Functions
//==================================================================================================

fn do_getegid(pm: &ProcessManager) -> Result<GroupIdentifier, Error> {
    let running: RunningProcess = pm.get_running()?;
    Ok(running.get_egid())
}

pub fn getegid(pm: &ProcessManager, _args: &KcallArgs) -> i32 {
    match do_getegid(pm) {
        Ok(egid) => egid.into(),
        Err(e) => e.code.into_errno(),
    }
}
