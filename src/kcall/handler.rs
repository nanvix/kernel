// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

use crate::{
    error::{
        Error,
        ErrorCode,
    },
    event,
    hal::{
        mem::VirtualAddress,
        Hal,
    },
    kcall::{
        KcallArgs,
        ScoreBoard,
    },
    mm::{
        KernelPage,
        VirtMemoryManager,
    },
    pm::{
        self,
        ProcessManager,
    },
};
use ::kcall::KcallNumber;

//==================================================================================================
//  Standalone Functions
//==================================================================================================

fn debug(buf: usize, count: usize) -> i32 {
    unsafe {
        let buf: &[u8] = core::slice::from_raw_parts(buf as *const u8, count as usize);
        let message: &str = core::str::from_utf8_unchecked(buf);
        crate::stdout::puts(message)
    }

    count as i32
}

fn do_debug(
    pm: &mut ProcessManager,
    mm: &mut VirtMemoryManager,
    args: &KcallArgs,
) -> Result<i32, Error> {
    let buf: usize = args.arg0 as usize;
    let size: usize = args.arg1 as usize;

    let src: VirtualAddress = VirtualAddress::new(buf);
    let dst: KernelPage = mm.alloc_kpage(true)?;

    pm.vmcopy_from_user(args.pid, dst.base().into_virtual_address().into_inner(), src, size)?;

    Ok(debug(dst.base().into_raw_value(), size))
}

///
/// # Description
///
/// Kernel call handler.
///
pub fn kcall_handler(mut hal: Hal, mut mm: VirtMemoryManager, mut pm: ProcessManager) {
    event::init(&mut hal);

    loop {
        // Read kernel call arguments from the scoreboard.
        match ScoreBoard::get_mut() {
            Ok(scoreboard) => match scoreboard.handle() {
                Ok(args) => {
                    let ret: i32 = match KcallNumber::from(args.number) {
                        KcallNumber::Debug => match do_debug(&mut pm, &mut mm, args) {
                            Ok(ret) => ret,
                            Err(e) => e.code.into_errno(),
                        },
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
