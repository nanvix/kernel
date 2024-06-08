// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Modules
//==================================================================================================

use alloc::sync::Arc;
use core::{
    hint,
    sync::atomic::{
        AtomicBool,
        Ordering,
    },
};

//==================================================================================================
// Structures
//==================================================================================================

///
/// # Description
///
/// A type that represents a non-reentrant spinlock.
///
#[derive(Clone)]
pub struct Spinlock(Arc<AtomicBool>);

///
/// # Description
///
/// A type that represents a guard for a spinlock.
///
pub struct SpinlockGuard(Spinlock);

//==================================================================================================
// Implementations
//==================================================================================================

impl Spinlock {
    ///
    /// # Description
    ///
    /// Creates a new unlocked spinlock.
    ///
    /// # Returns
    ///
    /// A new unlocked spinlock.
    ///
    pub fn new() -> Self {
        Spinlock(Arc::new(AtomicBool::new(false)))
    }

    ///
    /// # Description
    ///
    /// Locks the target spinlock and returns a guard that releases the lock when dropped.
    ///
    /// # Returns
    ///
    /// A guard that releases the lock when dropped.
    ///
    pub fn lock(&self) -> SpinlockGuard {
        while !self.try_lock() {
            hint::spin_loop();
        }
        SpinlockGuard(self.clone())
    }

    ///
    /// # Description
    ///
    /// Attempts to lock the target spinlock.
    ///
    /// # Returns
    ///
    /// - `true` if the lock was acquired.
    /// - `false` if the lock was not acquired.
    ///
    fn try_lock(&self) -> bool {
        !self.0.swap(true, Ordering::Acquire)
    }

    ///
    /// # Description
    ///
    /// Unlocks the target spinlock.
    ///
    /// # Safety
    ///
    /// This function leads to undefined behavior if any of the following conditions are violated:
    /// - The lock is held by the caller.
    ///
    unsafe fn unlock_unchecked(&self) {
        self.0.store(false, Ordering::Release);
    }
}

impl SpinlockGuard {
    ///
    /// # Description
    ///
    /// Unlocks the target spinlock.
    ///
    pub(super) fn unlock(&self) {
        // Safety: The lock is ensured to be held by the caller.
        unsafe { self.0.unlock_unchecked() };
    }

    ///
    /// # Description
    ///
    /// Locks the target spinlock guard.
    ///
    pub(super) fn lock(&self) {
        while !self.0.try_lock() {
            hint::spin_loop();
        }
    }
}

impl Drop for SpinlockGuard {
    fn drop(&mut self) {
        // Safety: The lock is ensured to be held by the caller.
        unsafe { self.0.unlock_unchecked() };
    }
}

unsafe impl Send for Spinlock {}

unsafe impl Sync for Spinlock {}
