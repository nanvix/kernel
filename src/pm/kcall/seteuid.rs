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
    UserIdentifier,
};

//==================================================================================================
// Standalone Functions
//==================================================================================================

fn do_seteuid(pm: &ProcessManager, uid: UserIdentifier) -> Result<(), Error> {
    let mut running: RunningProcess = pm.get_running()?;
    running.set_euid(uid)
}

pub fn seteuid(pm: &ProcessManager, args: &KcallArgs) -> i32 {
    match do_seteuid(pm, UserIdentifier::from(args.arg0)) {
        Ok(_) => 0,
        Err(e) => e.code.into_errno(),
    }
}
