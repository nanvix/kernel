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
    arch::io,
    error::{
        Error,
        ErrorCode,
    },
    hal::io::allocator::{
        IoPortAllocator,
        ReadWriteIoPort,
    },
};
use alloc::rc::Rc;
use core::{
    cell::RefCell,
    ops::{
        Deref,
        DerefMut,
    },
};

//==================================================================================================
// Enumerations
//==================================================================================================

/// Initialization Command Word 1
mod icw1 {
    /// ICW4 (not) needed
    pub enum Ic4 {
        /// ICW4 (not) needed
        NotNeeded = 0x10,
        /// ICW4 needed
        Needed = (1 << 0) | 0x10,
    }

    /// Single or Cascade mode
    pub enum Single {
        /// Single (Cascade) mode
        Cascade = 0x10,
        /// Single mode
        Single = (1 << 1) | 0x10,
    }

    /// Call Address Interval
    pub enum Interval {
        /// Call Address Interval 4 (8)
        Interval4 = 0x10,
        /// Call Address Interval 8 (16)
        Interval8 = (1 << 2) | 0x10,
    }

    /// Level triggered (edge) mode
    pub enum Level {
        /// Level triggered (edge) mode
        Edge = 0x10,
        /// Level triggered mode
        Level = (1 << 3) | 0x10,
    }
}

/// Initialization Command Word 4
mod icw4 {
    /// Microprocessor Mode
    pub enum MicroprocessorMode {
        /// MCS-80/85 Mode
        Mode8085 = 0,
        /// 8086/8088 Mode
        Mode8086 = (1 << 0),
    }

    /// Auto End of Interrupt
    pub enum AutoEoi {
        /// Normal EOI
        Normal = 0,
        /// Auto EOI
        Auto = (1 << 1),
    }

    /// Buffer Mode
    pub enum BufMode {
        /// Non-Buffered Mode
        NonBuffered = 0,
        /// Buffered Mode (Slave)
        BufferedSlave = (0b10 << 2),
        /// Buffered Mode (Master)
        BufferedMaster = (0b11 << 2),
    }

    /// Special Fully Nested Mode
    pub enum SpecialFullyNested {
        /// Not Special Fully Nested Mode
        NotSpecialFullyNested = 0,
        /// Special Fully Nested Mode
        SpecialFullyNested = (1 << 4),
    }
}

/// Operation Command Word 2
mod ocw2 {
    pub enum Eoi {
        /// Non-specific EOI
        NonSpecific = (0b001 << 5),
    }
}

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
    pub const PIC_NUM_IRQS: u8 = 8; // Number of IRQs

    /// Master PIC Registers
    pub const PIC_CTRL_MASTER: u8 = 0x20; // Control
    pub const PIC_DATA_MASTER: u8 = 0x21; // Data

    /// Slave PIC Registers
    pub const PIC_CTRL_SLAVE: u8 = 0xa0; // Control register
    pub const PIC_DATA_SLAVE: u8 = 0xa1; // Data register

    fn new(ioports: &mut IoPortAllocator, offset: u8) -> Result<Self, Error> {
        let ctrl_master: ReadWriteIoPort =
            ioports.allocate_read_write(Self::PIC_CTRL_MASTER as u16)?;
        let data_master: ReadWriteIoPort =
            ioports.allocate_read_write(Self::PIC_DATA_MASTER as u16)?;
        let ctrl_slave: ReadWriteIoPort =
            ioports.allocate_read_write(Self::PIC_CTRL_SLAVE as u16)?;
        let data_slave: ReadWriteIoPort =
            ioports.allocate_read_write(Self::PIC_DATA_SLAVE as u16)?;

        let mut pic = Self {
            ctrl_master,
            data_master,
            ctrl_slave,
            data_slave,
        };

        pic.init(offset);

        Ok(pic)
    }

    fn init(&mut self, offset: u8) {
        info!("initializing pic...");
        // Starts initialization sequence in cascade mode.
        self.ctrl_master.writeb(icw1::Ic4::Needed as u8);
        io::wait();
        self.ctrl_slave.writeb(icw1::Ic4::Needed as u8);
        io::wait();

        // Send new vector offset.
        self.data_master.writeb(offset);
        io::wait();
        self.data_slave.writeb(offset + Self::PIC_NUM_IRQS);
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
        if irq == 2 || irq >= Self::PIC_NUM_IRQS as u32 {
            error!("invalid irq {}", irq);
            return;
        }

        // Check if EOI is managed by slave PIC.
        if irq >= Self::PIC_NUM_IRQS as u32 {
            // Send EOI to slave PIC.
            self.ctrl_slave.writeb(ocw2::Eoi::NonSpecific as u8);
        }

        // Send EOI to master PIC.
        self.ctrl_master.writeb(ocw2::Eoi::NonSpecific as u8);
    }
}

//==================================================================================================
// Global Variables
//==================================================================================================

static mut PIC: Option<Rc<RefCell<Pic>>> = None;

pub struct PicRef(Rc<RefCell<Pic>>);

impl Deref for PicRef {
    type Target = Rc<RefCell<Pic>>;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

impl DerefMut for PicRef {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.0
    }
}

impl PicRef {
    pub fn ack(&self, irq: u32) -> Result<(), Error> {
        match self.try_borrow_mut() {
            Ok(mut pic) => {
                pic.ack(irq);
                Ok(())
            },
            Err(_) => Err(Error::new(ErrorCode::ResourceBusy, "pic is already borrowed")),
        }
    }

    pub fn mask(&self, mask: u16) -> Result<(), Error> {
        match self.try_borrow_mut() {
            Ok(mut pic) => {
                pic.mask(mask);
                Ok(())
            },
            Err(_) => Err(Error::new(ErrorCode::ResourceBusy, "pic is already borrowed")),
        }
    }
}

pub fn init(ioports: &mut IoPortAllocator, offset: u8) -> Result<PicRef, Error> {
    // Check if the PIC is already initialized.
    if unsafe { PIC.is_some() } {
        return Err(Error::new(ErrorCode::ResourceBusy, "pic is already initialized"));
    }

    let pic = Rc::new(RefCell::new(Pic::new(ioports, offset)?));

    unsafe { PIC = Some(pic.clone()) }

    Ok(PicRef(pic))
}

pub fn try_get() -> Result<PicRef, Error> {
    match unsafe { PIC.as_ref() } {
        Some(pic) => Ok(PicRef(pic.clone())),
        None => Err(Error::new(ErrorCode::TryAgain, "pic is not initialized"))?,
    }
}
