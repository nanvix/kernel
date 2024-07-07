// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

use crate::{
    error::Error,
    pm::{
        sync::spinlock::{
            Spinlock,
            SpinlockGuard,
        },
        thread::ThreadIdentifier,
        ProcessManager,
    },
};
use alloc::collections::LinkedList;
use core::cell::RefCell;

//==================================================================================================
// Structures
//==================================================================================================

///
/// # Description
///
/// A type that represents a condition variable.
///
pub struct Condvar {
    /// Underlying spinlock.
    lock: Spinlock,
    /// Threads that are sleeping on the condition variable.
    sleeping: RefCell<LinkedList<ThreadIdentifier>>,
}

//==================================================================================================
// Implementations
//==================================================================================================

impl Condvar {
    ///
    /// # Description
    ///
    /// Initializes a new condition variable.
    ///
    /// # Returns
    ///
    /// A new condition variable.
    ///
    pub fn new() -> Self {
        Self {
            lock: Spinlock::new(),
            sleeping: RefCell::new(LinkedList::new()),
        }
    }

    ///
    /// # Description
    ///
    /// Wakes a single thread that is waiting on the target condition variable.
    ///
    pub fn notify(&self) -> Result<(), Error> {
        if let Some(tid) = self.sleeping.borrow_mut().pop_front() {
            ProcessManager::wakeup(tid)?;
        }

        Ok(())
    }

    ///
    /// # Description
    ///
    /// Waits on the condition variable.
    ///
    pub fn wait(&self, guard: &SpinlockGuard) -> Result<(), Error> {
        let _condvar_guard: SpinlockGuard = self.lock.lock();

        self.sleeping
            .borrow_mut()
            .push_back(ProcessManager::get_tid()?);

        guard.unlock();

        ProcessManager::sleep()?;

        guard.lock();

        Ok(())
    }
}

unsafe impl Send for Condvar {}

unsafe impl Sync for Condvar {}