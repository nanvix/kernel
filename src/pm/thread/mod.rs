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

//==================================================================================================
// Thread
//==================================================================================================

///
/// # Description
///
/// A type that represents a thread identifier.
///
#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub struct ThreadIdentifier(pub usize);

impl ThreadIdentifier {
    /// Instantiates a thread identifier.
    pub fn new(id: usize) -> Self {
        Self(id)
    }
}

impl Debug for ThreadIdentifier {
    fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
        write!(f, "{:?}", self.0)
    }
}

#[derive(Debug)]
struct Thread {
    /// Thread identifier.
    id: ThreadIdentifier,
    /// Execution context.
    context: Pin<Box<ContextInformation>>,
    /// Kernel stack.
    #[allow(dead_code)]
    kstack: Stack,
}

impl Thread {
    pub fn new(id: ThreadIdentifier, context: ContextInformation, stack: Stack) -> Self {
        Self {
            id,
            context: Box::pin(context),
            kstack: stack,
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
    pub fn new(id: ThreadIdentifier, context: ContextInformation, stack: Stack) -> Self {
        Self(Thread::new(id, context, stack))
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
    fn new(kstack: &mut u8) -> (ReadyThread, Self) {
        let kernel: ReadyThread = ReadyThread::new(
            ThreadIdentifier(0),
            ContextInformation::default(),
            Stack::from_raw_parts(kstack, config::KSTACK_SIZE),
        );
        (
            kernel,
            Self {
                next_id: ThreadIdentifier(1),
            },
        )
    }

    pub fn create_thread(
        &mut self,
        context: ContextInformation,
        stack: Stack,
    ) -> Result<ReadyThread, Error> {
        let id: ThreadIdentifier = self.next_id;
        self.next_id = ThreadIdentifier(self.next_id.0 + 1);

        Ok(ReadyThread(Thread::new(id, context, stack)))
    }
}

//==================================================================================================
// Standalone Functions
//==================================================================================================

/// Initializes the thread manager.
pub fn init(kstack: &mut u8) -> (ReadyThread, ThreadManager) {
    // TODO: check for double initialization.

    ThreadManager::new(kstack)
}
