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
        Vmem,
    },
    pm::process::{
        Process,
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
    pm: &ProcessManager,
    mm: &mut VirtMemoryManager,
    args: &KcallArgs,
) -> Result<i32, Error> {
    let buf: usize = args.arg0 as usize;
    let size: usize = args.arg1 as usize;

    let src: VirtualAddress = VirtualAddress::new(buf);

    let process: Process = pm.find_process(args.pid)?;
    let mut dst: KernelPage = mm.alloc_kpage()?;
    let vm: &Vmem = process.vmem();

    vm.copy_from_user_unaligned(&mut dst, src, size)?;

    Ok(debug(dst.base().into_raw_value(), size))
}

///
/// # Description
///
/// Kernel call handler.
///
pub fn kcall_handler(hal: Hal, mut mm: VirtMemoryManager, pm: ProcessManager) -> ! {
    loop {
        // Read kernel call arguments from the scoreboard.
        match ScoreBoard::get_mut() {
            Ok(scoreboard) => match scoreboard.handle() {
                Ok(args) => {
                    let ret: i32 = match KcallNumber::from(args.number) {
                        KcallNumber::Debug => match do_debug(&pm, &mut mm, args) {
                            Ok(ret) => ret,
                            Err(e) => e.code.into_errno(),
                        },
                        _ => {
                            error!("invalid kernel call");
                            ErrorCode::InvalidSysCall.into_errno()
                        },
                    };
                    if let Err(e) = scoreboard.handled(ret) {
                        warn!("failed to signal kernel call handled: {:?}", e)
                    }
                },
                Err(e) => {
                    warn!("failed to handle kernel call: {:?}", e)
                },
            },
            Err(e) => {
                warn!("failed to get scoreboard: {:?}", e)
            },
        };
    }
}
