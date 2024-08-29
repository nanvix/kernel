// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use ::core::mem;
use ::error::{
    Error,
    ErrorCode,
};
use ::sys::ipc::{
    Message,
    MessageType,
};

//==================================================================================================
// Constants
//==================================================================================================

/// End of file flag.
const EOF_FLAG: u32 = 1 << 31;

//==================================================================================================
// Standalone Functions
//==================================================================================================

///
/// # Description
///
/// Writes an inter-kernel communication message to the kernel's standard output.
///
/// # Parameters
///
/// - `message`: Message to write.
///
/// # Returns
///
/// Upon success, empty is returned. Upon failure, an error is returned instead.
///
pub fn write(message: Message) -> Result<(), Error> {
    trace!(
        "write(): message.type={:?}, message.source={:?}, message.destination={:?}",
        message.message_type,
        message.source,
        message.destination
    );

    // Checks if message type is not supported.
    if message.message_type != MessageType::Ikc {
        let reason: &str = "unsupported message type";
        error!("write(): {}", reason);
        return Err(Error::new(ErrorCode::InvalidArgument, reason));
    }

    let bytes: [u8; mem::size_of::<Message>()] = message.to_bytes();

    // Write message to the kernel's standard output.
    // SAFETY: The standard output is present, initialized and thread-safe to write.
    unsafe {
        for b in bytes {
            crate::hal::platform::out32(b as u32);
        }
        let eof: u32 = EOF_FLAG;
        crate::hal::platform::out32(eof);
    }

    Ok(())
}

///
/// # Description
///
/// Reads an inter-kernel communication message from the kernel's standard input.
///
/// # Returns
///
/// Upon success, this function either returns the message read or `None` if there are no more
/// messages.  Upon failure, an error is returned instead.
///
pub fn read() -> Result<Option<Message>, Error> {
    // Read first byte.
    let value: u32 = unsafe { crate::hal::platform::in32() };

    // Check for EOF.
    if value & EOF_FLAG != 0 {
        return Ok(None);
    }

    // NOTE: trace command after reading the first byte, to avoid flooding the log.
    trace!("read()");

    const NBYTES: usize = core::mem::size_of::<Message>();
    let mut message: [u8; NBYTES] = [0; NBYTES];

    message[0] = (value & 0xff) as u8;

    // Read message from platform.
    for byte in message[1..].iter_mut() {
        let value: u32 = unsafe { crate::hal::platform::in32() };

        // Check for EOF.
        if value & EOF_FLAG != 0 {
            break;
        }

        *byte = (value & 0xff) as u8;
    }

    // Convert message to Message struct.
    match Message::try_from_bytes(message) {
        Ok(message) => Ok(Some(message)),
        Err(e) => {
            warn!("read(): {:?} ", e);
            Err(e)
        },
    }
}
