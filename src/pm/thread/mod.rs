// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    config,
    error::Error,
    hal::arch::x86::cpu::context::ContextInformation,
    pm::stack::Stack,
};
use alloc::boxed::Box;
use core::{
    fmt::Debug,
    pin::Pin,
};
use kcall::ThreadIdentifier;

//==================================================================================================
// Thread
//==================================================================================================

#[derive(Debug)]
struct Thread {
    /// Thread identifier.
    id: ThreadIdentifier,
    /// Execution context.
    context: Pin<Box<ContextInformation>>,
}

impl Thread {
    pub fn new(id: ThreadIdentifier, context: ContextInformation) -> Self {
        Self {
            id,
            context: Box::pin(context),
        }
    }

    pub fn context_mut(&mut self) -> &mut ContextInformation {
        self.context.as_mut().get_mut()
    }
}

pub struct RunningThread(Thread);

impl RunningThread {
    pub fn suspend(self) -> ReadyThread {
        ReadyThread(self.0)
    }

    ///
    /// # Description
    ///
    /// Returns the identifier of the target thread.
    ///
    /// # Returns
    ///
    /// The identifier of the target thread.
    ///
    pub fn id(&self) -> ThreadIdentifier {
        self.0.id
    }
}

pub struct ReadyThread(Thread);

impl ReadyThread {
    pub fn new(id: ThreadIdentifier, context: ContextInformation) -> Self {
        Self(Thread::new(id, context))
    }

    pub fn resume(self) -> RunningThread {
        RunningThread(self.0)
    }

    pub fn context_mut(&mut self) -> &mut ContextInformation {
        self.0.context_mut()
    }

    ///
    /// # Description
    ///
    /// Returns the identifier of the target thread.
    ///
    /// # Returns
    ///
    /// The identifier of the target thread.
    ///
    pub fn id(&self) -> ThreadIdentifier {
        self.0.id
    }
}


//==================================================================================================
// Thread Manager
//==================================================================================================

pub struct ThreadManager {
    next_id: ThreadIdentifier,
}

impl ThreadManager {
    fn new() -> (ReadyThread, Self) {
        let kernel: ReadyThread =
            ReadyThread::new(ThreadIdentifier::from(0), ContextInformation::default());
        (
            kernel,
            Self {
                next_id: ThreadIdentifier::from(1),
            },
        )
    }

    pub fn create_thread(&mut self, context: ContextInformation) -> Result<ReadyThread, Error> {
        let id: ThreadIdentifier = self.next_id;
        self.next_id = ThreadIdentifier::from(Into::<usize>::into(self.next_id) + 1);

        Ok(ReadyThread(Thread::new(id, context)))
    }
}

//==================================================================================================
// Standalone Functions
//==================================================================================================

/// Initializes the thread manager.
pub fn init() -> (ReadyThread, ThreadManager) {
    // TODO: check for double initialization.

    ThreadManager::new()
}
