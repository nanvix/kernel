// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    arch::cpu::ioapic::{
        Ioapic,
        IoapicId,
        IoapicRedirectionTable,
        IoapicRedirectionTableHigh,
        IoapicRedirectionTableLow,
        IoapicVersion,
    },
    error::{
        Error,
        ErrorCode,
    },
};
use core::ops::{
    Deref,
    DerefMut,
};

//==================================================================================================
// Implementations
//==================================================================================================

pub struct IoapicPtr {
    intvec_base: u8,
    ptr: Ioapic,
}

impl IoapicPtr {
    /// Initializes an I/O APIC.
    pub fn init(intvec_base: u8, id: u8, addr: usize, gsi: u32) -> Result<Self, Error> {
        info!("initializing ioapic (id={}, addr={:#010x}, gsi={})", id, addr, gsi);

        // Set I/O APIC base address.
        let ptr: Ioapic = Ioapic::new(addr);
        let mut ioapic: IoapicPtr = Self { intvec_base, ptr };

        // Check ID mismatch.
        if IoapicId::id(ioapic.deref_mut()) != id {
            let reason: &str = "id mismatch";
            error!("init(): {}", reason);
            return Err(Error::new(ErrorCode::InvalidArgument, &reason));
        }

        ioapic.print_info();

        let maxintr: u8 = IoapicVersion::maxredirect(ioapic.deref_mut());

        // For all interrupts: set physical destination mode to APIC ID 0; set high
        // activate; set edge-triggered; set disabled; set fixed delivery mode;
        // identity map interrupts.
        for irq in 0..=maxintr {
            IoapicRedirectionTable::write(
                ioapic.deref_mut(),
                irq as u32,
                0,
                IoapicRedirectionTableLow::IOREDTBL_INTMASK_MASK | (intvec_base + irq) as u32,
            );
        }

        Ok(ioapic)
    }

    /// Enables an interrupt line.
    pub fn enable(&mut self, irq: u8, cpunum: u8) -> Result<(), Error> {
        info!("trace(): irq={}, cpunum={}", irq, cpunum);
        // When using physical destination mode, only the lower 4 bits of the
        // destination field are used. The specification is unclear about the
        // behavior of the upper bits. See 82093AA I/O ADVANCED PROGRAMMABLE
        // INTERRUPT CONTROLLER (IOAPIC) for details.
        const MAXIMUM_NUMBER_CPUS: u8 = 16;
        self.print_info();

        // Check IRQ lies in a valid range.
        if irq >= IoapicVersion::maxredirect(self.deref_mut()) {
            let reason: &str = "invalid irq number";
            error!("enable(): {}", reason);
            return Err(Error::new(ErrorCode::InvalidArgument, &reason));
        }

        // Check CPU number lies in a valid range.
        if cpunum > MAXIMUM_NUMBER_CPUS {
            let reason: &str = "invalid cpu number";
            error!("enable(): {}", reason);
            return Err(Error::new(ErrorCode::InvalidArgument, &reason));
        }

        // Set physical destination mode to cpunum; set high activate; set
        // edge-triggered; set enabled; set fixed delivery mode; identity map
        // interrupt.
        // NOTE: The following cast is safe because "cpunum" is a 4-bit value.
        // NOTE: The following cast is safe because "irq" is an 8-bit value.
        let intvec_base: u8 = self.intvec_base;
        info!("intvec_base={}", intvec_base);
        IoapicRedirectionTable::write(
            self.deref_mut(),
            irq as u32,
            (cpunum as u32) << IoapicRedirectionTableHigh::IOREDTBL_DEST_SHIFT,
            (intvec_base + irq) as u32,
        );

        Ok(())
    }

    /// Prints information about I/O APIC.
    pub fn print_info(&mut self) {
        info!("ioapic id: {}", IoapicId::id(self.deref_mut()));
        info!("ioapic version: {}", IoapicVersion::version(self.deref_mut()));
        info!("ioapic max redirection entries: {}", IoapicVersion::maxredirect(self.deref_mut()));
        info!("ioapic pointers: {:?}", self.ptr);
    }
}

impl Deref for IoapicPtr {
    type Target = Ioapic;

    fn deref(&self) -> &Self::Target {
        &self.ptr
    }
}

impl DerefMut for IoapicPtr {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.ptr
    }
}
