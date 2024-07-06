// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

use crate::{
    arch::cpu::xapic::{
        self,
        Xapic,
        XapicCmci,
        XapicEsr,
        XapicIcrDeliveryMode,
        XapicIcrDeliveryStatus,
        XapicIcrHi,
        XapicIcrLo,
        XapicId,
        XapicIntvec,
        XapicPcint,
        XapicSvr,
        XapicThermal,
        XapicTimer,
        XapicTpr,
        XapicVer,
    },
    error::Error,
};

pub struct XapicRef {
    id: u8,
    ptr: Xapic,
}

impl XapicRef {
    pub fn ack(&mut self) {
        unsafe { self.ptr.write(xapic::XAPIC_EOI, 0) };
    }
}

pub fn init(id: u8, base: usize) -> Result<XapicRef, Error> {
    info!("initializing xapic (id={}, base={:#010x})", id, base);
    let xapic: Xapic = Xapic::new(base as *mut u32);

    // Check ID matches the one in the APIC.
    let apic_id: XapicId = XapicId::from_u32(unsafe { xapic.read(xapic::XAPIC_ID) });
    if apic_id.id() != id as u32 {
        unimplemented!("set apic id");
    }
    // Setup spurious interrupt vector.
    let svr: XapicSvr = XapicSvr::new(XapicIntvec::Spurious as u32, true, false, false);
    unsafe { xapic.write(xapic::XAPIC_SVR, svr.to_u32()) };

    // Clear error status register (requires back-to-back writes).
    let esr: XapicEsr = XapicEsr::new(false, false, false, false, false, false, false, false);
    unsafe { xapic.write(xapic::XAPIC_ESR, esr.to_u32()) };
    unsafe { xapic.write(xapic::XAPIC_ESR, esr.to_u32()) };

    // Ack any outstanding interrupts.
    unsafe { xapic.write(xapic::XAPIC_EOI, 0) };

    // Send an Init Level De-Assert to synchronize arbitration ID's.
    let icrhi: XapicIcrHi = XapicIcrHi::new(0);
    unsafe { xapic.write(xapic::XAPIC_ICRHI, icrhi.to_u32()) };
    let icrlo: XapicIcrLo = XapicIcrLo::from_u32(0x00080000 | 0x00000500 | 0x00008000);
    unsafe { xapic.write(xapic::XAPIC_ICRLO, icrlo.to_u32()) };

    // Poll delivery status until it is set to zero.
    loop {
        let icrlo: XapicIcrLo = XapicIcrLo::from_u32(unsafe { xapic.read(xapic::XAPIC_ICRLO) });
        if icrlo.delivery_status() as u8 == XapicIcrDeliveryStatus::Idle as u8 {
            break;
        }
    }

    // Disable timer interrupt.
    let timer: XapicTimer = XapicTimer::new(
        XapicIntvec::Timer as u32,
        false,
        true,
        XapicIcrDeliveryMode::FixedDelivery as u32,
    );
    unsafe { xapic.write(xapic::XAPIC_TIMER, timer.to_u32()) };

    // Read version number.
    let version: XapicVer = XapicVer::new(unsafe { xapic.read(xapic::XAPIC_VER) });

    // Check if performance counter register is supported
    if version.max_lvt() >= 4 {
        info!("performance counter interrupt supported");
        // It is, so disable performance counter interrupt.
        let perf: XapicPcint = XapicPcint::new(
            XapicIntvec::Pcint as u32,
            XapicIcrDeliveryMode::FixedDelivery as u32,
            false,
            true,
        );
        unsafe { xapic.write(xapic::XAPIC_PCINT, perf.to_u32()) };
    }

    // Check if thermal register is supported.
    if version.max_lvt() >= 5 {
        info!("thermal interrupt supported");
        // It is, so disable thermal interrupt.
        let thermal: XapicThermal = XapicThermal::new(
            XapicIntvec::Thermal as u32,
            XapicIcrDeliveryMode::FixedDelivery as u32,
            false,
            true,
        );
        unsafe { xapic.write(xapic::XAPIC_THERM, thermal.to_u32()) };
    }

    // Check if CMCI register is supported.
    if version.max_lvt() >= 6 {
        info!("cmci interrupt supported");
        // It is, so disable CMCI interrupt.
        let cmci: XapicCmci = XapicCmci::new(
            XapicIntvec::Cmci as u32,
            XapicIcrDeliveryMode::FixedDelivery as u32,
            false,
            true,
        );
        unsafe { xapic.write(xapic::XAPIC_CMCI, cmci.to_u32()) };
    }

    // Disable local interrupt 0.
    let lint0: XapicPcint = XapicPcint::new(
        XapicIntvec::Lint0 as u32,
        XapicIcrDeliveryMode::FixedDelivery as u32,
        false,
        false,
    );
    unsafe { xapic.write(xapic::XAPIC_LINT0, lint0.to_u32()) };

    // Disable local interrupt 1.
    let lint1: XapicPcint = XapicPcint::new(
        XapicIntvec::Lint1 as u32,
        XapicIcrDeliveryMode::FixedDelivery as u32,
        false,
        false,
    );
    unsafe { xapic.write(xapic::XAPIC_LINT1, lint1.to_u32()) };

    // Disable error interrupt.
    let error: XapicPcint = XapicPcint::new(
        XapicIntvec::Error as u32,
        XapicIcrDeliveryMode::FixedDelivery as u32,
        false,
        true,
    );
    unsafe { xapic.write(xapic::XAPIC_ERROR, error.to_u32()) };

    // Enable interrupts on the APIC (but not on the processor).
    let tpr: XapicTpr = XapicTpr::new(0, 0);
    unsafe { xapic.write(xapic::XAPIC_TPR, tpr.to_u32()) };

    Ok(XapicRef { id, ptr: xapic })
}
