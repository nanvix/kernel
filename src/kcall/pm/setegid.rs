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

fn do_setegid(pm: &ProcessManager, gid: GroupIdentifier) -> Result<(), Error> {
    let mut running: RunningProcess = pm.get_running()?;
    running.set_egid(gid)
}

pub fn setegid(pm: &ProcessManager, args: &KcallArgs) -> i32 {
    match do_setegid(pm, GroupIdentifier::from(args.arg0)) {
        Ok(_) => 0,
        Err(e) => e.code.into_errno(),
    }
}
