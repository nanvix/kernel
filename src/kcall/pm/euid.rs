// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::pm::process::{
    ProcessManager,
    RunningProcess,
};
use ::kcall::{
    Error,
    UserIdentifier,
};

//==================================================================================================
// Standalone Functions
//==================================================================================================

pub fn do_getuid(pm: &ProcessManager) -> Result<UserIdentifier, Error> {
    let running: RunningProcess = pm.get_running()?;
    Ok(running.get_uid())
}

pub fn do_setuid(pm: &ProcessManager, uid: UserIdentifier) -> Result<(), Error> {
    let mut running: RunningProcess = pm.get_running()?;
    running.set_uid(uid)
}
