// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

///
/// # Description
///
/// Waits for a given number of microseconds in a spin loop.
///
/// # Parameters
///
/// - `delay`: Number of microseconds to wait.
///
pub fn microdelay(delay: u64) {
    // TODO: probe this value from the hardware.
    const CPU_FREQUENCY: u64 = 2_000_000_000;
    let tscdelay: u64 = (CPU_FREQUENCY * delay) / 1_000_000;
    // TODO: use `arch::cpu::rdtsc()` once it is available.
    let s: u64 = unsafe { core::arch::x86::_rdtsc() };
    while (unsafe { core::arch::x86::_rdtsc() } - s < tscdelay) {
        arch::cpu::pause();
    }
}
