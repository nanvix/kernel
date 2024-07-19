// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Imports
//==================================================================================================

use crate::{
    error::{
        Error,
        ErrorCode,
    },
    hal::{
        arch::{
            ContextInformation,
            ExceptionInformation,
            InterruptNumber,
        },
        mem::VirtualAddress,
        Hal,
    },
    pm::{
        sync::condvar::Condvar,
        ProcessManager,
    },
};
use ::alloc::{
    collections::LinkedList,
    rc::Rc,
};
use ::core::{
    cell::{
        RefCell,
        RefMut,
    },
    mem,
};
use ::kcall::{
    Capability,
    Event,
    EventDescriptor,
    EventInformation,
    ExceptionEvent,
    ProcessIdentifier,
};

//==================================================================================================
// Structures
//==================================================================================================

static mut DISPATCHER: EventManager = unsafe { mem::zeroed() };

struct ExceptionEventInformation {
    pid: ProcessIdentifier,
    info: ExceptionInformation,
}

struct EventManagerInner {
    nevents: usize,
    wait: Option<Rc<Condvar>>,
    pending_interrupts: [LinkedList<EventDescriptor>; usize::BITS as usize],
    pending_exceptions: [LinkedList<(EventDescriptor, ExceptionEventInformation, Rc<Condvar>)>;
        usize::BITS as usize],
}

impl EventManagerInner {
    const NUMBER_EVENTS: usize = 2;

    pub fn try_wait(
        &mut self,
        info: *mut EventInformation,
        interrupts: usize,
        exceptions: usize,
    ) -> Result<bool, Error> {
        for i in 0..Self::NUMBER_EVENTS {
            // Check if any interrupts were triggered.
            if ((self.nevents + i) % Self::NUMBER_EVENTS) == 0 {
                // FIXME: starvation.
                for i in 0..usize::BITS {
                    if (interrupts & (1 << i)) != 0 {
                        let idx: usize = i as usize;
                        if let Some(_event) = self.pending_interrupts[idx].pop_front() {
                            return Ok(true);
                        }
                    }
                }
            }

            // Check if any exceptions were triggered.
            if ((self.nevents + i) % Self::NUMBER_EVENTS) == 1 {
                // FIXME: starvation.
                for i in 0..usize::BITS {
                    if (exceptions & (1 << i)) != 0 {
                        let idx: usize = i as usize;
                        if let Some(entry) = self.pending_exceptions[idx].pop_front() {
                            let dest: VirtualAddress =
                                VirtualAddress::new(info as *mut EventInformation as usize);
                            let size: usize = core::mem::size_of::<EventInformation>();

                            let mut src: EventInformation = EventInformation::default();
                            src.id = entry.0.clone();
                            src.pid = entry.1.pid;
                            src.number = Some(entry.1.info.num() as usize);
                            src.code = Some(entry.1.info.code() as usize);
                            src.address = Some(entry.1.info.addr() as usize);
                            src.instruction = Some(entry.1.info.instruction() as usize);

                            let src: *const EventInformation = &src as *const EventInformation;
                            let src: VirtualAddress = VirtualAddress::new(src as usize);

                            ProcessManager::vmcopy_to_user(dest, src, size)?;

                            self.pending_exceptions[idx].push_back(entry);

                            return Ok(true);
                        }
                    }
                }
            }
        }

        Ok(false)
    }

    fn resume_exception(&mut self, ev: ExceptionEvent) -> Result<(), Error> {
        let idx: usize = usize::from(ev);

        let is_pending_exception = |evdesc: &EventDescriptor, ev: &ExceptionEvent| -> bool {
            match evdesc.event() {
                Event::Exception(ev2) => &ev2 == ev,
                _ => false,
            }
        };

        // Search and remove event from pending exceptions.
        if let Some(entry) = self.pending_exceptions[idx]
            .iter()
            .position(|(evdesc, _info, _resume)| is_pending_exception(evdesc, &ev))
        {
            let (_enventinfo, _excpinfo, resume) = self.pending_exceptions[idx].remove(entry);

            if let Err(e) = resume.notify_all() {
                warn!("failed to notify all: {:?}", e);
                unimplemented!("terminate process")
            }
        }

        Ok(())
    }

    fn wakeup_interrupt(&mut self, interrupts: usize) -> Result<(), kcall::Error> {
        self.nevents += 1;
        let idx: usize = interrupts.trailing_zeros() as usize;
        let ev = Event::from(kcall::InterruptEvent::try_from(idx)?);
        let eventid: EventDescriptor = EventDescriptor::new(self.nevents, ev);
        self.pending_interrupts[idx].push_back(eventid);
        self.get_wait().notify_all()
    }

    fn wakeup_exception(
        &mut self,
        exceptions: usize,
        pid: ProcessIdentifier,
        info: &ExceptionInformation,
    ) -> Result<Rc<Condvar>, kcall::Error> {
        trace!("wakeup_exception(): exceptions={:#x}, pid={:?}, info={:?}", exceptions, pid, info);
        self.nevents += 1;
        let idx: usize = exceptions.trailing_zeros() as usize;
        let ev = Event::from(kcall::ExceptionEvent::try_from(idx)?);
        let eventid: EventDescriptor = EventDescriptor::new(self.nevents, ev);
        let resume: Rc<Condvar> = Rc::new(Condvar::new());
        self.pending_exceptions[idx].push_back((
            eventid,
            ExceptionEventInformation {
                pid,
                info: info.clone(),
            },
            resume.clone(),
        ));
        self.get_wait().notify_all()?;

        Ok(resume)
    }

    fn get_wait(&self) -> &Rc<Condvar> {
        // NOTE: it is safe to unwrap because the wait field is always Some.
        self.wait.as_ref().unwrap()
    }
}

//==================================================================================================
// Event Manager
//==================================================================================================

pub struct EventManager(RefCell<EventManagerInner>);

impl EventManager {
    pub fn resume(evdesc: EventDescriptor) -> Result<(), Error> {
        trace!("do_resume(): evdesc={:?}", evdesc);
        match evdesc.event() {
            Event::Interrupt(_ev) => {
                // No further action is required for interrupts.
                Ok(())
            },
            Event::Exception(ev) => {
                return EventManager::get().try_borrow_mut()?.resume_exception(ev);
            },
        }
    }

    pub fn wait(
        info: *mut EventInformation,
        interrupts: usize,
        exceptions: usize,
    ) -> Result<(), Error> {
        trace!(
            "do_wait(): info={:?}, interrupts={:#x}, exceptions={:#x}",
            info,
            interrupts,
            exceptions
        );
        // Ensure that the process has the required capabilities.
        if interrupts != 0 && !ProcessManager::has_capability(Capability::InterruptControl)? {
            let reason: &str = "process does not have interrupt control capability";
            error!("do_wait(): reason={:?}", reason);
            return Err(Error::new(ErrorCode::PermissionDenied, &reason));
        }
        if exceptions != 0 && !ProcessManager::has_capability(Capability::ExceptionControl)? {
            let reason: &str = "process does not have exception control capability";
            error!("do_wait(): reason={:?}", reason);
            return Err(Error::new(ErrorCode::PermissionDenied, &reason));
        }

        let wait: Rc<Condvar> = EventManager::get().try_borrow_mut()?.get_wait().clone();

        loop {
            trace!("do_wait(): checking for events");
            let event_received: bool = EventManager::get()
                .try_borrow_mut()?
                .try_wait(info, interrupts, exceptions)?;

            if event_received {
                break Ok(());
            }

            trace!("do_wait(): waiting for event");
            wait.wait()?;
            trace!("do_wait(): event received");
        }
    }

    fn try_borrow_mut(&self) -> Result<RefMut<EventManagerInner>, Error> {
        trace!("try_borrow_mut()");
        match self.0.try_borrow_mut() {
            Ok(em) => Ok(em),
            Err(e) => {
                let reason: &str = "failed to borrow event manager";
                error!("try_borrow_mut(): {:?} (error={:?})", reason, e);
                Err(Error::new(ErrorCode::PermissionDenied, &reason))
            },
        }
    }

    fn get() -> &'static EventManager {
        unsafe { &DISPATCHER }
    }
}

//==================================================================================================
// Standalone Functions
//==================================================================================================

fn interrupt_handler(intnum: InterruptNumber) {
    trace!("interrupt_handler(): intnum={:?}", intnum);
    if let Ok(mut dispatcher) = EventManager::get().try_borrow_mut() {
        match dispatcher.wakeup_interrupt(1 << intnum as usize) {
            Ok(_) => {},
            Err(e) => error!("failed to wake up dispatcher: {:?}", e),
        }
    }
}

fn exception_handler(info: &ExceptionInformation, _ctx: &ContextInformation) {
    trace!("exception_handler(): info={:?}", info);
    let pid: ProcessIdentifier = match ProcessManager::get_pid() {
        Ok(pid) => pid,
        Err(e) => {
            error!("failed to get process identifier: {:?}", e);
            return;
        },
    };

    let resume: Rc<Condvar> = match EventManager::get().try_borrow_mut() {
        Ok(mut dispatcher) => {
            match dispatcher.wakeup_exception(1 << info.num() as usize, pid, info) {
                Ok(resume) => resume,
                Err(e) => {
                    error!("failed to wake up dispatcher: {:?}", e);
                    return;
                },
            }
        },
        Err(e) => {
            error!("failed to borrow event manager: {:?}", e);
            return;
        },
    };

    if let Err(_) = resume.wait() {
        if let Err(e) = ProcessManager::exit(-1) {
            unreachable!("failed to terminate process (error={:?})", e);
        }
    }
}

pub fn init(hal: &mut Hal) {
    let mut pending_interrupts: [LinkedList<EventDescriptor>; usize::BITS as usize] =
        unsafe { mem::zeroed() };
    for list in pending_interrupts.iter_mut() {
        *list = LinkedList::default();
    }

    let mut pending_exceptions: [LinkedList<(
        EventDescriptor,
        ExceptionEventInformation,
        Rc<Condvar>,
    )>; usize::BITS as usize] = unsafe { mem::zeroed() };
    for list in pending_exceptions.iter_mut() {
        *list = LinkedList::default();
    }

    let dispatcher: RefCell<EventManagerInner> = RefCell::new(EventManagerInner {
        nevents: 0,
        pending_interrupts,
        pending_exceptions,
        wait: Some(Rc::new(Condvar::new())),
    });

    unsafe {
        DISPATCHER = EventManager(dispatcher);
    }

    hal.excpman.register_handler(exception_handler);

    for intnum in InterruptNumber::VALUES {
        if intnum == InterruptNumber::Timer {
            continue;
        }
        match hal.intman.register_handler(intnum, interrupt_handler) {
            Ok(()) => {
                if let Err(e) = hal.intman.unmask(intnum) {
                    warn!("failed to mask interrupt: {:?}", e);
                }
            },
            Err(e) => warn!("failed to register interrupt handler: {:?}", e),
        }
    }
}
