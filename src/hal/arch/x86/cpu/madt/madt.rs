// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    arch::cpu::{
        acpi::AcpiSdtHeader,
        madt::{
            Madt,
            MadtEntryHeader,
            MadtEntryIoApic,
            MadtEntryIoApicNmi,
            MadtEntryIoApicSourceOverride,
            MadtEntryLocalApic,
            MadtEntryLocalApicNmi,
            MadtEntryLocalX2Apic,
            MadtEntryType,
            MadtLocalApicAddressOverride,
        },
    },
    error::Error,
};
use alloc::collections::LinkedList;

pub struct MadtInfo {
    pub sdt: AcpiSdtHeader,
    pub local_apic_addr: u32,
    pub flags: u32,
    pub entries: LinkedList<MadtEntry>,
}

pub enum MadtEntry {
    LocalApic(MadtEntryLocalApic),
    IoApicSourceOverride(MadtEntryIoApicSourceOverride),
    IoApic(MadtEntryIoApic),
    IoApicNmi(MadtEntryIoApicNmi),
    LocalApicNmi(MadtEntryLocalApicNmi),
    LocalApicAddressOverride(MadtLocalApicAddressOverride),
    LocalX2Apic(MadtEntryLocalX2Apic),
}

impl MadtInfo {
    pub unsafe fn from_ptr(ptr: *const Madt) -> Result<Self, Error> {
        Ok(Self {
            sdt: AcpiSdtHeader::from_ptr(ptr as *const AcpiSdtHeader)?,
            local_apic_addr: ptr.read_unaligned().local_apic_addr,
            flags: ptr.read_unaligned().flags,
            entries: LinkedList::new(),
        })
    }

    pub fn display(&self) {
        info!("MADT:");
        info!("  Local APIC Address: 0x{:x}", self.local_apic_addr);
        info!("  Flags: 0x{:x}", self.flags);
    }
}

impl Madt {
    pub unsafe fn parse(madt: *const Madt) -> Result<MadtInfo, Error> {
        let base: *const u8 = (*madt).entries.as_ptr() as *const u8;
        let madt_len: usize = madt.read_unaligned().header.length as usize;
        let mut offset: usize = 0;

        let mut madt: MadtInfo = MadtInfo::from_ptr(madt)?;

        while offset < madt_len as usize {
            let ptr: *const MadtEntryHeader = unsafe { base.add(offset) as *const MadtEntryHeader };
            let header: MadtEntryHeader = MadtEntryHeader::from_ptr(ptr);

            match MadtEntryType::from(header.typ) {
                MadtEntryType::LocalApic => {
                    let entry: *const MadtEntryLocalApic =
                        unsafe { base.add(offset) as *const MadtEntryLocalApic };
                    let local_apic_info: MadtEntryLocalApic = MadtEntryLocalApic::from_ptr(entry);
                    local_apic_info.display();
                    madt.entries
                        .push_back(MadtEntry::LocalApic(local_apic_info));
                },
                MadtEntryType::IoApic => {
                    let entry: *const MadtEntryIoApic =
                        unsafe { base.add(offset) as *const MadtEntryIoApic };
                    let ioapic_info: MadtEntryIoApic = MadtEntryIoApic::from_ptr(entry);
                    ioapic_info.display();
                    madt.entries.push_back(MadtEntry::IoApic(ioapic_info));
                },
                MadtEntryType::IoApicSourceOverride => {
                    let entry: *const MadtEntryIoApicSourceOverride =
                        unsafe { base.add(offset) as *const MadtEntryIoApicSourceOverride };
                    let ioapic_override_info: MadtEntryIoApicSourceOverride =
                        MadtEntryIoApicSourceOverride::from_ptr(entry);
                    ioapic_override_info.display();
                    madt.entries
                        .push_back(MadtEntry::IoApicSourceOverride(ioapic_override_info));
                },
                MadtEntryType::IoApicNmi => {
                    let entry: *const MadtEntryIoApicNmi =
                        unsafe { base.add(offset) as *const MadtEntryIoApicNmi };
                    let ioapic_nmi_info: MadtEntryIoApicNmi = MadtEntryIoApicNmi::from_ptr(entry);
                    ioapic_nmi_info.display();
                    madt.entries
                        .push_back(MadtEntry::IoApicNmi(ioapic_nmi_info));
                },
                MadtEntryType::LocalApicNmi => {
                    let entry: *const MadtEntryLocalApicNmi =
                        unsafe { base.add(offset) as *const MadtEntryLocalApicNmi };
                    let local_apic_nmi_info: MadtEntryLocalApicNmi =
                        MadtEntryLocalApicNmi::from_ptr(entry);
                    local_apic_nmi_info.display();
                    madt.entries
                        .push_back(MadtEntry::LocalApicNmi(local_apic_nmi_info));
                },
                MadtEntryType::LocalApicAddressOverride => {
                    let entry: *const MadtLocalApicAddressOverride =
                        unsafe { base.add(offset) as *const MadtLocalApicAddressOverride };
                    let local_apic_address_override_info: MadtLocalApicAddressOverride =
                        MadtLocalApicAddressOverride::from_ptr(entry);
                    local_apic_address_override_info.display();
                    madt.entries.push_back(MadtEntry::LocalApicAddressOverride(
                        local_apic_address_override_info,
                    ));
                },
                MadtEntryType::LocalX2Apic => {
                    let entry: *const MadtEntryLocalX2Apic =
                        unsafe { base.add(offset) as *const MadtEntryLocalX2Apic };
                    let local_x2apic_info: MadtEntryLocalX2Apic =
                        MadtEntryLocalX2Apic::from_ptr(entry);
                    local_x2apic_info.display();
                    madt.entries
                        .push_back(MadtEntry::LocalX2Apic(local_x2apic_info));
                },
            }
            offset += unsafe { (*ptr).len as usize };
        }

        Ok(madt)
    }
}
