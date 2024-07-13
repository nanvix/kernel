// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    mm::Vmem,
    pm::process::identity::ProcessIdentity,
};
use ::kcall::{
    Error,
    GroupIdentifier,
    ProcessIdentifier,
    UserIdentifier,
};

//==================================================================================================
// Process
//==================================================================================================

///
/// # Description
///
/// A type that represents the inner state of a process.
///
pub struct ProcessState {
    /// Process identifier.
    pid: ProcessIdentifier,
    /// Process identity.
    identity: ProcessIdentity,
    /// Memory address space.
    vmem: Vmem,
}

impl ProcessState {
    pub fn new(pid: ProcessIdentifier, identity: ProcessIdentity, vmem: Vmem) -> Self {
        Self {
            pid,
            identity,
            vmem,
        }
    }

    pub fn pid(&self) -> ProcessIdentifier {
        self.pid
    }

    pub fn identity(&self) -> ProcessIdentity {
        self.identity.clone()
    }

    pub fn get_uid(&self) -> UserIdentifier {
        self.identity.get_uid()
    }

    pub fn set_uid(&mut self, uid: UserIdentifier) -> Result<(), Error> {
        self.identity.set_uid(uid)
    }

    pub fn get_euid(&self) -> UserIdentifier {
        self.identity.get_euid()
    }

    pub fn set_euid(&mut self, euid: UserIdentifier) -> Result<(), Error> {
        self.identity.set_euid(euid)
    }

    pub fn get_gid(&self) -> GroupIdentifier {
        self.identity.get_gid()
    }

    pub fn set_gid(&mut self, gid: GroupIdentifier) -> Result<(), Error> {
        self.identity.set_gid(gid)
    }

    pub fn get_egid(&self) -> GroupIdentifier {
        self.identity.get_egid()
    }

    pub fn set_egid(&mut self, egid: GroupIdentifier) -> Result<(), Error> {
        self.identity.set_egid(egid)
    }

    pub fn vmem(&self) -> &Vmem {
        &self.vmem
    }

    pub fn vmem_mut(&mut self) -> &mut Vmem {
        &mut self.vmem
    }
}
