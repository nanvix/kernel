// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Lint Exceptions
//==================================================================================================

// Not all functions are used.
#![allow(dead_code)]

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    arch::{
        cpu::pic::*,
        io,
    },
    error::Error,
    hal::io::allocator::{
        IoPortAllocator,
        ReadWriteIoPort,
    },
};

//==================================================================================================
// Structures
//==================================================================================================

/// Programmable Interrupt Controller
pub struct Pic {
    /// Master PIC Control Register
    ctrl_master: ReadWriteIoPort,
    /// Master PIC Data Register
    data_master: ReadWriteIoPort,
    /// Slave PIC Control Register
    ctrl_slave: ReadWriteIoPort,
    /// Slave PIC Data Register
    data_slave: ReadWriteIoPort,
}
//==================================================================================================
// Implementations
//==================================================================================================

impl Pic {
    fn new(ioports: &mut IoPortAllocator, offset: u8) -> Result<Self, Error> {
        let ctrl_master: ReadWriteIoPort = ioports.allocate_read_write(PIC_CTRL_MASTER as u16)?;
        let data_master: ReadWriteIoPort = ioports.allocate_read_write(PIC_DATA_MASTER as u16)?;
        let ctrl_slave: ReadWriteIoPort = ioports.allocate_read_write(PIC_CTRL_SLAVE as u16)?;
        let data_slave: ReadWriteIoPort = ioports.allocate_read_write(PIC_DATA_SLAVE as u16)?;

        let mut pic = Self {
            ctrl_master,
            data_master,
            ctrl_slave,
            data_slave,
        };

        pic.init(offset);

        Ok(pic)
    }

    ///
    /// # Description
    ///
    /// Initializes the target PIC.
    ///
    /// # Parameters
    ///
    /// - `offset`: Vector offset.
    ///
    fn init(&mut self, offset: u8) {
        info!("initializing pic (offset={})", offset);
        // Starts initialization sequence in cascade mode.
        self.ctrl_master.writeb(icw1::Ic4::Needed as u8);
        io::wait();
        self.ctrl_slave.writeb(icw1::Ic4::Needed as u8);
        io::wait();

        // Send new vector offset.
        self.data_master.writeb(offset);
        io::wait();
        self.data_slave.writeb(offset + PIC_NUM_IRQS);
        io::wait();

        // Tell the master that there is a slave PIC at IRQ line 2 and
        // tell the slave PIC that it is cascaded.
        self.data_master.writeb(0x04);
        io::wait();
        self.data_slave.writeb(0x02);
        io::wait();

        // Set 8086 mode.
        self.data_master
            .writeb(icw4::MicroprocessorMode::Mode8086 as u8);
        io::wait();
        self.data_slave
            .writeb(icw4::MicroprocessorMode::Mode8086 as u8);
        io::wait();

        // Mask all interrupts.
        self.disable();
    }

    pub fn mask(&mut self, mask: u16) {
        self.data_master.writeb(mask as u8);
        io::wait();
        self.data_slave.writeb((mask >> 8) as u8);
        io::wait();
    }

    ///
    /// # Description
    ///
    /// Disables the target PIC.
    ///
    pub fn disable(&mut self) {
        self.mask(u16::MAX);
    }

    pub fn ack(&mut self, irq: u32) {
        // Check for invalid interrupt request. IRQ 2 is reserved for the cascade.
        if irq == 2 || irq >= PIC_NUM_IRQS as u32 {
            error!("invalid irq {}", irq);
            return;
        }

        // Check if EOI is managed by slave PIC.
        if irq >= PIC_NUM_IRQS as u32 {
            // Send EOI to slave PIC.
            self.ctrl_slave.writeb(ocw2::Eoi::NonSpecific as u8);
        }

        // Send EOI to master PIC.
        self.ctrl_master.writeb(ocw2::Eoi::NonSpecific as u8);
    }
}

//==================================================================================================
// Standalone Functions
//==================================================================================================

pub fn init(ioports: &mut IoPortAllocator, offset: u8) -> Result<Pic, Error> {
    Pic::new(ioports, offset)
}
