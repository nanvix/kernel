// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

mod interrupted;
mod runnable;
mod running;
mod state;
mod suspended;
mod zombie;

//==================================================================================================
// Exports
//==================================================================================================

pub use interrupted::InterruptedProcess;
pub use runnable::RunnableProcess;
pub use running::RunningProcess;
pub use suspended::SleepingProcess;
pub use zombie::ZombieProcess;
