/*
 * Copyright(c) 2011-2023 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

//==============================================================================
// Imports
//==============================================================================

use core::fmt;
use nanvix::devices;

//==============================================================================
// Structures
//==============================================================================

/// A formatter object
pub struct Logger();

//==============================================================================
// Associated Functions
//==============================================================================

impl Logger {
    ///
    /// **Description**
    ///
    /// Gets a logger for a module.
    ///
    /// **Parameters**
    ///
    /// - `module` - Name of the target module.
    ///
    /// **Return**
    ///
    /// A logger for the module.
    ///
    pub fn get(module: &str) -> Logger {
        use core::fmt::Write;
        let mut writer: Logger = Logger();
        let _ = core::write!(&mut writer, "[{}] ", module);
        writer
    }
}

//==============================================================================
// Trait Implementations
//==============================================================================

impl ::core::ops::Drop for Logger {
    fn drop(&mut self) {
        use core::fmt::Write;
        let _ = write!(self, "\n");
    }
}

impl fmt::Write for Logger {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        devices::write(0, s.as_ptr(), s.len());
        Ok(())
    }
}
