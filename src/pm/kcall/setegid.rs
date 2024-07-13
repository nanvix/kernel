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

fn do_setegid(pm: &mut ProcessManager, gid: GroupIdentifier) -> Result<(), Error> {
    pm.setegid(gid)
}

pub fn setegid(pm: &mut ProcessManager, args: &KcallArgs) -> i32 {
    match do_setegid(pm, GroupIdentifier::from(args.arg0)) {
        Ok(_) => 0,
        Err(e) => e.code.into_errno(),
    }
}
