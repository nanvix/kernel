// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    error::Error,
    pm::sync::{
        condvar::Condvar,
        spinlock::{
            Spinlock,
            SpinlockGuard,
        },
    },
};
use core::cell::RefCell;

//==================================================================================================
// Structures
//==================================================================================================

///
/// # Description
///
/// A type that represents a semaphore.
///
pub struct Semaphore {
    /// Underlying spinlock.
    lock: Spinlock,
    /// Value.
    value: RefCell<usize>,
    /// Threads that are sleeping on the semaphore.
    sleeping: Condvar,
}

//==================================================================================================
// Implementations
//==================================================================================================

impl Semaphore {
    ///
    /// # Description
    ///
    /// Initializes a new semaphore.
    ///
    /// # Parameters
    ///
    /// - `value`: Initial value of the semaphore.
    ///
    /// # Returns
    ///
    /// A new semaphore.
    ///
    pub fn new(value: usize) -> Self {
        Self {
            lock: Spinlock::new(),
            value: RefCell::new(value),
            sleeping: Condvar::new(),
        }
    }

    ///
    /// # Description
    ///
    /// Acquires the semaphore.
    ///
    /// # Returns
    ///
    /// Upon success, empty result is returned. Upon failure, an error is returned instead.
    ///
    pub fn down(&self) -> Result<(), Error> {
        let guard: SpinlockGuard = self.lock.lock();

        while *self.value.borrow() == 0 {
            self.sleeping.wait(&guard)?;
        }

        *self.value.borrow_mut() -= 1;

        Ok(())
    }

    ///
    /// # Description
    ///
    /// Releases the semaphore.
    ///
    /// # Returns
    ///
    /// Upon success, empty result is returned. Upon failure, an error is returned instead.
    ///
    pub fn up(&self) -> Result<(), Error> {
        let _guard: SpinlockGuard = self.lock.lock();

        *self.value.borrow_mut() += 1;
        self.sleeping.notify()
    }
}

unsafe impl Send for Semaphore {}

unsafe impl Sync for Semaphore {}
