// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::arch::io;

//==================================================================================================
// Structures
//==================================================================================================

///
/// **Description**
/// Types of I/O ports.
///
#[derive(Debug, PartialEq, Eq)]
pub(super) enum IoPortType {
    ReadOnly,
    WriteOnly,
    ReadWrite,
}

/// **Description**
/// Information about an I/O port.
///
pub(super) struct IoPortInfo {
    number: u16,
}

///
/// **Description**
/// An I/O port.
///
pub(super) struct IoPort {
    info: IoPortInfo,
    pub(super) typ: IoPortType,
}

//==================================================================================================
// Implementations
//==================================================================================================

impl IoPortInfo {
    ///
    /// **Description**
    /// Instantiates an I/O port information structure.
    ///
    /// **Parameters**
    /// - `number`: Number of the I/O port.
    ///
    /// **Returns**
    /// The I/O port information structure.
    ///
    pub(super) fn new(number: u16) -> Self {
        Self { number }
    }
}

impl IoPort {
    ///
    /// **Description**
    /// Instantiates a read-only I/O port.
    ///
    /// **Parameters**
    /// - `info`: Information about the I/O port.
    ///
    /// **Returns**
    /// A read-only I/O port.
    ///
    pub(super) fn new_read_only(info: IoPortInfo) -> Self {
        Self {
            info,
            typ: IoPortType::ReadOnly,
        }
    }

    ///
    /// **Description**
    /// Instantiates a write-only I/O port.
    ///
    /// **Parameters**
    /// - `info`: Information about the I/O port.
    ///
    /// **Returns**
    /// A write-only I/O port.
    ///
    pub(super) fn new_write_only(info: IoPortInfo) -> Self {
        Self {
            info,
            typ: IoPortType::WriteOnly,
        }
    }

    ///
    /// **Description**
    /// Instantiates a read-write I/O port.
    ///
    /// **Parameters**
    /// - `info`: Information about the I/O port.
    ///
    /// **Returns**
    /// A read-write I/O port.
    ///
    pub(super) fn new_read_write(info: IoPortInfo) -> Self {
        Self {
            info,
            typ: IoPortType::ReadWrite,
        }
    }

    ///
    /// **Description**
    /// Gets the number of the target I/O port.
    ///
    /// **Returns**
    /// The number of the target I/O port.
    ///
    pub(super) fn number(&self) -> u16 {
        self.info.number
    }

    ///
    /// **Description**
    /// Reads a byte from the target I/O port.
    ///
    /// **Returns**
    /// The byte read from the target I/O port.
    ///
    pub(super) fn readb(&self) -> u8 {
        unsafe { io::in8(self.info.number) }
    }

    ///
    /// **Description**
    /// Reads a word from the target I/O port.
    ///
    /// **Returns**
    /// The word read from the target I/O port.
    ///
    pub(super) fn readw(&self) -> u16 {
        unsafe { io::in16(self.info.number) }
    }

    ///
    /// **Description**
    /// Writes a byte to the target I/O port.
    ///
    /// **Parameters**
    /// - `b`: Byte to write.
    ///
    pub(super) fn writeb(&self, b: u8) {
        unsafe { io::out8(self.info.number, b) }
    }

    ///
    /// **Description**
    /// Writes a word to the target I/O port.
    ///
    /// **Parameters**
    /// - `w`: Word to write.
    ///
    pub(super) fn writew(&self, w: u16) {
        unsafe { io::out16(self.info.number, w) }
    }
}
