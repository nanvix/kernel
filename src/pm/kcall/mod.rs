// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

mod capctl;
mod getegid;
mod geteuid;
mod getgid;
mod getuid;
mod mmap;
mod setegid;
mod seteuid;
mod setgid;
mod setuid;
mod terminate;

//==================================================================================================
// Exports
//==================================================================================================

pub use capctl::capctl;
pub use getegid::getegid;
pub use geteuid::geteuid;
pub use getgid::getgid;
pub use getuid::getuid;
pub use mmap::mmap;
pub use setegid::setegid;
pub use seteuid::seteuid;
pub use setgid::setgid;
pub use setuid::setuid;
pub use terminate::terminate;
