/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

//==============================================================================
// Imports
//==============================================================================

use crate::kcall::{
    kcall1,
    kcall2,
    KcallNumbers,
};
use core::fmt;

pub enum ExceptionNumber {
    /// Division-by-Zero Error
    DivideByZero = 0,
    /// Debug Exception
    Debug = 1,
    /// Non-Maskable Interrupt
    NonMaskableInterrupt = 2,
    /// Breakpoint Exception
    Breakpoint = 3,
    /// Overflow Exception
    Overflow = 4,
    /// Bounds Check Exception
    BoundsCheck = 5,
    /// Invalid Opcode Exception
    InvalidOpcode = 6,
    /// Coprocessor Not Available
    CoprocessorNotAvailable = 7,
    /// Double Fault
    DoubleFault = 8,
    /// Coprocessor Segment Overrun
    CoprocessorSegmentOverrun = 9,
    /// Invalid TSS
    InvalidTss = 10,
    /// Segment Not Present
    SegmentNotPresent = 11,
    /// Stack Segment Fault
    StackSegmentFault = 12,
    /// General Protection Fault
    GeneralProtectionFault = 13,
    /// Page Fault
    PageFault = 14,
    /// Floating Point Exception
    FpuError = 16,
    /// Alignment Check Exception
    AlignmentCheck = 17,
    /// Machine Check Exception
    MachineCheck = 18,
    /// SMID Unit Exception
    SimdError = 19,
    /// Virtual Exception
    VirtualException = 20,
    /// Security Exception
    SecurityException = 30,
}

impl Into<u32> for ExceptionNumber {
    fn into(self) -> u32 {
        match self {
            ExceptionNumber::DivideByZero => 0,
            ExceptionNumber::Debug => 1,
            ExceptionNumber::NonMaskableInterrupt => 2,
            ExceptionNumber::Breakpoint => 3,
            ExceptionNumber::Overflow => 4,
            ExceptionNumber::BoundsCheck => 5,
            ExceptionNumber::InvalidOpcode => 6,
            ExceptionNumber::CoprocessorNotAvailable => 7,
            ExceptionNumber::DoubleFault => 8,
            ExceptionNumber::CoprocessorSegmentOverrun => 9,
            ExceptionNumber::InvalidTss => 10,
            ExceptionNumber::SegmentNotPresent => 11,
            ExceptionNumber::StackSegmentFault => 12,
            ExceptionNumber::GeneralProtectionFault => 13,
            ExceptionNumber::PageFault => 14,
            ExceptionNumber::FpuError => 16,
            ExceptionNumber::AlignmentCheck => 17,
            ExceptionNumber::MachineCheck => 18,
            ExceptionNumber::SimdError => 19,
            ExceptionNumber::VirtualException => 20,
            ExceptionNumber::SecurityException => 30,
        }
    }
}

pub enum ExcpAction {
    Handle = 0,
    Defer = 1,
}

impl Into<u32> for ExcpAction {
    fn into(self) -> u32 {
        match self {
            ExcpAction::Defer => 0,
            ExcpAction::Handle => 1,
        }
    }
}

#[repr(C)]
#[derive(Clone, Copy, Default)]
pub struct ExcpInfo {
    pub num: u32,
    pub addr: u32,
    pub pc: u32,
}

impl core::fmt::Debug for ExcpInfo {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(
            f,
            "ExcpInfo {{ num: {}, addr: {:#08x}, pc: {:#08x} }}",
            self.num, self.addr, self.pc
        )
    }
}

pub fn excpctrl(excpnum: ExceptionNumber, action: ExcpAction) -> i32 {
    unsafe {
        kcall2(KcallNumbers::ExcpCtrl as u32, excpnum.into(), action.into())
            as i32
    }
}

pub fn excpwait() -> ExcpInfo {
    let mut info: ExcpInfo = ExcpInfo::default();
    unsafe {
        kcall1(
            KcallNumbers::ExcpWait as u32,
            &mut info as *mut ExcpInfo as u32,
        );
        info
    }
}

pub fn excpresume(excpnum: ExceptionNumber) {
    unsafe {
        kcall1(KcallNumbers::ExcpResume as u32, excpnum.into());
    }
}
