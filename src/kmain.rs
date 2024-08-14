// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Configuration
//==================================================================================================

#![deny(clippy::all)]
#![forbid(clippy::large_stack_frames)]
#![forbid(clippy::large_stack_arrays)]
#![allow(internal_features)]
#![feature(core_intrinsics)] // bios require this.
#![feature(allocator_api)] // kheap uses this.
#![feature(pointer_is_aligned_to)] // mboot uses this.
#![feature(asm_const)] // gdt uses this.
#![feature(linked_list_remove)] // vmem uses this.
#![feature(linked_list_retain)] // vmem uses this.
#![feature(never_type)] // exit() uses this.
#![no_std]
#![no_main]

//==================================================================================================
// Imports
//==================================================================================================

extern crate alloc;

use crate::{
    hal::{
        arch::x86::cpu::madt::MadtInfo,
        mem::{
            AccessPermission,
            Address,
            MemoryRegion,
            MemoryRegionType,
            TruncatedMemoryRegion,
            VirtualAddress,
        },
        Hal,
    },
    kargs::KernelArguments,
    kimage::KernelImage,
    kmod::KernelModule,
    mm::{
        elf::Elf32Fhdr,
        kheap,
        VirtMemoryManager,
        Vmem,
    },
    pm::ProcessManager,
};
use ::alloc::{
    collections::LinkedList,
    string::String,
};
use ::sys::pm::ProcessIdentifier;

//==================================================================================================
// Modules
//==================================================================================================

#[macro_use]
mod macros;

mod debug;
mod event;
mod hal;
mod io;
mod ipc;
mod kargs;
mod kcall;
mod kimage;
mod klog;
mod kmod;
mod kpanic;
mod mboot;
mod mm;
mod pm;
mod stdout;
mod uart;

//==================================================================================================
// Standalone Functions
//==================================================================================================

fn test() {
    if !crate::hal::mem::test() {
        panic!("memory tests failed");
    }
}

///
/// # Description
///
/// Spawn bootstrap servers.
///
/// # Parameters
///
/// - `mm`: A reference to the virtual memory manager to use.
/// - `pm`: A reference to the process manager to use.
/// - `kmods`: A reference to the list of kernel modules to spawn.
///
/// # Returns
///
/// The number of servers that were successfully spawned.
///
fn spawn_servers(
    mm: &mut VirtMemoryManager,
    pm: &mut ProcessManager,
    kmods: &LinkedList<KernelModule>,
) -> usize {
    let mut count: usize = 0;
    // Spawn all servers.
    for kmod in kmods.iter() {
        let elf: &Elf32Fhdr = Elf32Fhdr::from_address(kmod.start().into_raw_value());
        let pid: ProcessIdentifier = {
            match pm.create_process(mm) {
                Ok(pid) => pid,
                Err(err) => {
                    warn!("failed to create server process: {:?}", err);
                    continue;
                },
            }
        };
        match pm.exec(mm, pid, elf) {
            Ok(_) => {
                count += 1;
            },
            Err(err) => {
                warn!("failed to load server image: {:?}", err);
                unimplemented!("kill server process");
            },
        }

        info!("server {} spawned, pid={:?}", kmod.cmdline(), pid);
    }

    count
}

#[no_mangle]
pub extern "C" fn kmain(kargs: &KernelArguments) {
    info!("initializing the kernel...");

    // Initialize the kernel heap.
    if let Err(e) = unsafe { kheap::init() } {
        panic!("failed to initialize kernel heap: {:?}", e);
    }

    test();

    // Parse kernel arguments.
    info!("parsing kernel arguments...");
    let (madt, mut memory_regions, mut mmio_regions, kernel_modules): (
        Option<MadtInfo>,
        LinkedList<MemoryRegion<VirtualAddress>>,
        LinkedList<TruncatedMemoryRegion<VirtualAddress>>,
        LinkedList<KernelModule>,
    ) = match kargs.parse() {
        Ok(bootinfo) => {
            (bootinfo.madt, bootinfo.memory_regions, bootinfo.mmio_regions, bootinfo.kernel_modules)
        },
        Err(err) => {
            panic!("failed to parse kernel arguments: {:?}", err);
        },
    };

    info!("parsing kernel image...");
    let kimage: KernelImage = match KernelImage::new() {
        Ok(kimage) => kimage,
        Err(err) => {
            panic!("failed to initialize kernel image: {:?}", err);
        },
    };

    // Add kernel image to list of memory regions.
    memory_regions.push_back(kimage.text());
    memory_regions.push_back(kimage.rodata());
    if let Some(data) = kimage.data() {
        memory_regions.push_back(data);
    }
    memory_regions.push_back(kimage.bss());
    memory_regions.push_back(kimage.kpool());

    // Add kernel modules to list of memory regions.
    for module in kernel_modules.iter() {
        let name: String = module.cmdline();
        let start: VirtualAddress = module.start().into_virtual_address();
        let size: usize = module.size();
        let typ: MemoryRegionType = MemoryRegionType::Reserved;
        if let Ok(region) = MemoryRegion::new(&name, start, size, typ, AccessPermission::RDONLY) {
            memory_regions.push_back(region);
        }
    }

    let mut hal: Hal = match hal::init(&mut memory_regions, &mut mmio_regions, madt) {
        Ok(hal) => hal,
        Err(err) => {
            panic!("failed to initialize hardware abstraction layer: {:?}", err);
        },
    };

    // Initialize the memory manager.
    let (root, mut mm): (Vmem, VirtMemoryManager) =
        match mm::init(&kimage, memory_regions, mmio_regions) {
            Ok((root, mm)) => (root, mm),
            Err(err) => {
                panic!("failed to initialize memory manager: {:?}", err);
            },
        };

    let mut pm: ProcessManager = match pm::init(&mut hal, root) {
        Ok(pm) => pm,
        Err(err) => {
            panic!("failed to initialize process manager: {:?}", err);
        },
    };

    // Start application core.
    // TODO: get core count from the HAL.
    #[cfg(feature = "smp")]
    for coreid in 1..=1 {
        trace!("starting application core {}...", coreid);
        if let Err(e) = hal.intman.start_core(coreid, hal::arch::TRAMPOLINE_ADDRESS) {
            panic!("failed to start application core (e={:?}", e);
        }
    }

    if spawn_servers(&mut mm, &mut pm, &kernel_modules) > 0 {
        // Initialize kernel call dispatcher.
        kcall::init();

        // Enable timer interrupts.
        if let Err(e) = hal.intman.unmask(hal::arch::InterruptNumber::Timer) {
            panic!("failed to mask timer interrupt: {:?}", e);
        }

        kcall::handler(&mut hal, &mut mm, &mut pm)
    }

    trace!("the system will shutdown now!");
    kernel_magic_string();
}

#[no_mangle]
pub extern "C" fn do_ap_start(coreid: u32) {
    trace!("hello from core {}", coreid);
    loop {
        core::hint::spin_loop();
    }
}

///
/// # Description
///
/// Outputs a magic string to the console and enters an infinite loop. The continuous integration
/// system expects this to be the last thing that the kernel, and thus leverages this behavior to
/// assert for a successful execution.
///
/// # Returns
///
/// This function never returns.
///
pub fn kernel_magic_string() -> ! {
    let magic_string: &str = "PANIC: Hello World!\n";
    unsafe { crate::stdout::puts(magic_string) }
    hal::arch::shutdown();
}
