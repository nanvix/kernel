// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Exports
//==================================================================================================

pub mod acpi;
pub mod eflags;
pub mod excp;
pub mod idt;
pub mod idtr;
pub mod ioapic;
pub mod madt;
pub mod pic;
pub mod pit;
pub mod ring;
pub mod tss;
pub mod xapic;

//==================================================================================================
// Standalone Functions
//==================================================================================================

///
/// # Description
///
/// Checks if the CPU supports the CPUID instruction.
///
/// # Return Values
///
/// If the CPU supports the CPUID instruction, `true` is returned. Otherwise, `false` is returned.
///
pub fn has_cpuid() -> bool {
    extern "C" {
        fn __has_cpuid() -> bool;
    }

    unsafe { __has_cpuid() }
}
