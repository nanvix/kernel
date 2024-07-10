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
    GroupIdentifier,
};

//==================================================================================================
// Standalone Functions
//==================================================================================================

pub fn do_getgid(pm: &ProcessManager) -> Result<GroupIdentifier, Error> {
    let running: RunningProcess = pm.get_running()?;
    Ok(running.get_gid())
}

pub fn do_setgid(pm: &ProcessManager, gid: GroupIdentifier) -> Result<(), Error> {
    let mut running: RunningProcess = pm.get_running()?;
    running.set_gid(gid)
}
