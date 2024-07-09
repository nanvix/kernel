// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use kcall::{
    GroupIdentifier,
    UserIdentifier,
};

//==================================================================================================
// Structures
//==================================================================================================

///
/// # Description
///
/// A type that represents the identity of a process.
///
#[derive(Clone, Copy)]
pub struct ProcessIdentity {
    /// Real user identifier.
    uid: UserIdentifier,
    /// Effective user identifier.
    euid: UserIdentifier,
    /// Saved user identifier.
    suid: UserIdentifier,
    /// Real group identifier.
    gid: GroupIdentifier,
    /// Effective group identifier.
    egid: GroupIdentifier,
    /// Saved group identifier.
    sgid: GroupIdentifier,
}

//==================================================================================================
// Implementations
//==================================================================================================

impl ProcessIdentity {
    ///
    /// # Description
    ///
    /// Instantiates a new process identity.
    ///
    /// # Parameters
    ///
    /// - `uid`: Real user identifier.
    /// - `euid`: Effective user identifier.
    /// - `suid`: Saved user identifier.
    /// - `gid`: Real group identifier.
    /// - `egid`: Effective group identifier.
    /// - `sgid`: Saved group identifier.
    ///
    /// # Returns
    ///
    /// A new process identity.
    ///
    pub fn new(
        uid: UserIdentifier,
        euid: UserIdentifier,
        suid: UserIdentifier,
        gid: GroupIdentifier,
        egid: GroupIdentifier,
        sgid: GroupIdentifier,
    ) -> Self {
        Self {
            uid,
            euid,
            suid,
            gid,
            egid,
            sgid,
        }
    }

    ///
    /// # Description
    ///
    /// Constructs a process identity from another one.
    ///
    /// # Parameters
    ///
    /// - `other`: The other process identity.
    ///
    /// # Returns
    ///
    /// A new process identity.
    ///
    pub fn from(other: &Self) -> Self {
        Self {
            uid: other.uid,
            euid: other.euid,
            suid: other.suid,
            gid: other.gid,
            egid: other.egid,
            sgid: other.sgid,
        }
    }
}
