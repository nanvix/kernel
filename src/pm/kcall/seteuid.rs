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

fn do_seteuid(pm: &mut ProcessManager, uid: UserIdentifier) -> Result<(), Error> {
    pm.seteuid(uid)
}

pub fn seteuid(pm: &mut ProcessManager, args: &KcallArgs) -> i32 {
    match do_seteuid(pm, UserIdentifier::from(args.arg0)) {
        Ok(_) => 0,
        Err(e) => e.code.into_errno(),
    }
}
