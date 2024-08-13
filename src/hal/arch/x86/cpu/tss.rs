// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use ::arch::cpu::tss::Tss;
use ::core::{
    arch,
    mem,
};
use ::error::{
    Error,
    ErrorCode,
};

//==================================================================================================
// Structures
//==================================================================================================

///
/// # Description
///
/// A type that enables one to access the task state segment (TSS).
///
pub struct TssRef;

//==================================================================================================
// Global Variables
//==================================================================================================

/// Task state segment (TSS).
#[no_mangle]
static mut TSS: Tss = unsafe { mem::zeroed() };

/// Indicates if the TSS was initialized.
static mut TSS_REF: Option<TssRef> = None;

//==================================================================================================
// Implementations
//==================================================================================================

impl TssRef {
    ///
    /// # Description
    ///
    /// Initializes the task state segment (TSS).
    ///
    /// # Parameters
    ///
    /// - `ss0`: Stack segment for ring 0.
    /// - `esp0`: Stack pointer for ring 0.
    ///
    /// # Returns
    ///
    /// Upon success, a reference to the TSS is returned. Upon failure, an error code is returned.
    ///
    /// # Safety
    ///
    /// This function is unsafe for the following reasons:
    /// - It mutates global variables.
    ///
    pub unsafe fn new(ss0: u32, esp0: u32) -> Result<Self, Error> {
        info!("initializing tss (ss0={:#02x}, esp0={:#08x})", ss0, esp0);

        // Check if the TSS was already initialized.
        if TSS_REF.is_some() {
            let reason: &str = "tss is already initialized";
            error!("new(): {}", reason);
            return Err(Error::new(ErrorCode::OutOfMemory, reason));
        }

        Self::init(ss0, esp0);
        TSS_REF = Some(Self);

        Ok(Self)
    }

    ///
    /// # Description
    ///
    /// Returns a reference to the task state segment (TSS).
    ///
    /// # Returns
    ///
    /// A reference to the task state segment (TSS).
    ///
    pub unsafe fn address(&self) -> usize {
        core::ptr::addr_of!(TSS) as usize
    }

    ///
    /// # Description
    ///
    /// Returns the size of the task state segment (TSS).
    ///
    /// # Returns
    ///
    /// The size of the task state segment (TSS).
    ///
    pub fn size(&self) -> usize {
        mem::size_of::<Tss>()
    }

    #[inline(never)]
    pub unsafe fn load(&self, selector: u16) {
        info!("loading tss (selector={:x})", selector);
        arch::asm!("ltr %ax", in("ax") selector, options(nostack, att_syntax));
    }

    unsafe fn init(ss0: u32, esp0: u32) {
        TSS.link = 0;
        TSS.esp0 = esp0;
        TSS.ss0 = ss0;
        TSS.esp1 = 0;
        TSS.ss1 = 0;
        TSS.esp2 = 0;
        TSS.ss2 = 0;
        TSS.cr3 = 0;
        TSS.eip = 0;
        TSS.eflags = 0;
        TSS.eax = 0;
        TSS.ecx = 0;
        TSS.edx = 0;
        TSS.ebx = 0;
        TSS.esp = 0;
        TSS.ebp = 0;
        TSS.esi = 0;
        TSS.edi = 0;
        TSS.es = 0;
        TSS.cs = 0;
        TSS.ss = 0;
        TSS.ds = 0;
        TSS.fs = 0;
        TSS.gs = 0;
        TSS.ldtr = 0;
        TSS.iomap = 0;
    }
}
