// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
//  Imports
//==================================================================================================

use crate::{
    ipc::typ::MessageType,
    pm::ProcessIdentifier,
};
use ::core::mem;
use crate::error::Error;

//==================================================================================================
//  Structures
//==================================================================================================

///
/// # Description
///
/// A structure that represents a message that can be sent between processes.
///
#[derive(Debug)]
#[repr(C)]
pub struct Message {
    /// Type of the message.
    pub message_type: MessageType,
    /// Process that sent the message.
    pub source: ProcessIdentifier,
    /// Process that should receive the message.
    pub destination: ProcessIdentifier,
    /// Payload of the message.
    pub payload: [u8; Self::PAYLOAD_SIZE],
}
crate::static_assert_size!(Message, Message::TOTAL_SIZE);

//==================================================================================================
//  Implementations
//==================================================================================================

impl Message {
    /// Total Size of a message.
    pub const TOTAL_SIZE: usize = 64;
    /// The size of the message header fields (source, destination and type).
    pub const HEADER_SIZE: usize = 2 * mem::size_of::<ProcessIdentifier>() + MessageType::SIZE;
    /// The size of the message's payload.
    pub const PAYLOAD_SIZE: usize = Self::TOTAL_SIZE - Self::HEADER_SIZE;

    ///
    /// # Description
    ///
    /// Creates a new message.
    ///
    /// # Parameters
    ///
    /// - `source`: The source process.
    /// - `destination`: The destination process.
    /// - `message_type`: The type of the message.
    /// - `payload`: The message payload.
    ///
    /// # Returns
    ///
    /// The new message.
    ///
    pub fn new(
        source: ProcessIdentifier,
        destination: ProcessIdentifier,
        message_type: MessageType,
        payload: [u8; Self::PAYLOAD_SIZE],
    ) -> Self {
        Self {
            message_type,
            source,
            destination,
            payload,
        }
    }

    ///
    /// # Description
    ///
    /// Converts the target message to a byte array.
    ///
    /// # Returns
    ///
    /// A byte array that represents the target message.
    ///
    pub fn to_bytes(self) -> [u8; Self::HEADER_SIZE + Self::PAYLOAD_SIZE] {
        unsafe { mem::transmute(self) }
    }

    ///
    /// # Description
    ///
    /// Attempts to convert a byte array to a message.
    ///
    /// # Parameters
    ///
    /// - `bytes`: The byte array to convert.
    ///
    /// # Returns
    ///
    /// Upon success, the message is returned. Upon failure, an error is returned instead.
    ///
    pub fn try_from_bytes(
        bytes: [u8; Self::HEADER_SIZE + Self::PAYLOAD_SIZE],
    ) -> Result<Self, Error> {
        Ok(unsafe { mem::transmute(bytes) })
    }
}

impl Default for Message {
    fn default() -> Self {
        Self {
            message_type: MessageType::Empty,
            source: ProcessIdentifier::KERNEL,
            destination: ProcessIdentifier::KERNEL,
            payload: [0; Self::PAYLOAD_SIZE],
        }
    }
}
