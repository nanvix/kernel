// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

use crate::{
    debug,
    error::ErrorCode,
    event,
    hal::Hal,
    kcall::ScoreBoard,
    mm::VirtMemoryManager,
    pm::{
        self,
        ProcessManager,
    },
};
use ::kcall::KcallNumber;

//==================================================================================================
//  Standalone Functions
//==================================================================================================
///
/// # Description
///
/// Kernel call handler.
///
pub fn kcall_handler(mut hal: Hal, mut _mm: VirtMemoryManager, mut pm: ProcessManager) {
    event::init(&mut hal);

    loop {
        // Read kernel call arguments from the scoreboard.
        match ScoreBoard::get_mut() {
            Ok(scoreboard) => match scoreboard.handle() {
                Ok(args) => {
                    let ret: i32 = match KcallNumber::from(args.number) {
                        KcallNumber::Debug => debug::debug(args),
                        KcallNumber::GetPid => {
                            // NOTE: this should be handled by the dispatcher.
                            // However we emit an invalid system call, just in case.
                            error!("cannot handle getpid()");
                            ErrorCode::InvalidSysCall.into_errno()
                        },
                        KcallNumber::GetTid => {
                            // NOTE: this should be handled by the dispatcher.
                            // However we emit an invalid system call, just in case.
                            error!("cannot handle gettid()");
                            ErrorCode::InvalidSysCall.into_errno()
                        },
                        KcallNumber::GetUid => pm::getuid(&pm, args),
                        KcallNumber::GetGid => pm::getgid(&pm, args),
                        KcallNumber::GetEuid => pm::geteuid(&pm, args),
                        KcallNumber::GetEgid => pm::getegid(&pm, args),
                        KcallNumber::SetUid => pm::setuid(&mut pm, args),
                        KcallNumber::SetGid => pm::setgid(&mut pm, args),
                        KcallNumber::SetEuid => pm::seteuid(&mut pm, args),
                        KcallNumber::SetEgid => pm::setegid(&mut pm, args),
                        KcallNumber::CapCtl => pm::capctl(&mut pm, args),
                        KcallNumber::Terminate => pm::terminate(&mut pm, args),
                        KcallNumber::EventCtrl => event::evctrl(&mut pm, args),
                        _ => {
                            error!("invalid kernel call");
                            ErrorCode::InvalidSysCall.into_errno()
                        },
                    };
                    if let Err(e) = scoreboard.handled(ret) {
                        warn!("failed to signal kernel call handled: {:?}", e)
                    }
                },
                Err(e) => match e.code {
                    ErrorCode::Interrupted => break,
                    ErrorCode::OperationWouldBlock => {
                        if let Err(e) = ProcessManager::switch() {
                            error!("context switch failed: {:?}", e);
                        }
                    },
                    _ => {
                        error!("failed to handle kernel call: {:?}", e);
                    },
                },
            },
            Err(e) => {
                warn!("failed to get scoreboard: {:?}", e)
            },
        };

        if let Err(e) = pm.harvest_zombies() {
            error!("failed to harvest zombies: {:?}", e);
        }
    }

    if let Err(e) = pm.harvest_zombies() {
        error!("failed to harvest zombies: {:?}", e);
    }

    trace!("shutting down");
}
