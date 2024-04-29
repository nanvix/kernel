/*
 * Copyright(c) 2011-2024 The Maintainers of Nanvix.
 * Licensed under the MIT License.
 */

//==============================================================================
// Constants
//==============================================================================

/// Maximum number of mailboxes open per process.
pub const MAILBOX_OPEN_MAX: u32 = 16;

/// Maximum size of a message
pub const MAILBOX_MESSAGE_SIZE: u32 = 64;
