// Copyright(c) The Maintainers of Nanvix.
// Licensed under the MIT License.

//==================================================================================================
// Structures
//==================================================================================================

///
/// # Description
///
/// Error code for various adverse conditions.
///
/// # Notes
///
/// The values in this enumeration intentionally match the error codes defined in the Linux kernel.
///
#[derive(Copy, Clone, PartialEq, Eq, Debug)]
pub enum ErrorCode {
    /// Operation not permitted (EPERM).
    OperationNotPermitted = 1,
    /// No such file or directory (ENOENT).
    NoSuchEntry = 2,
    /// No such process (ESRCH).
    NoSuchProcess = 3,
    /// Interrupted system call (EINTR).
    Interrupted = 4,
    /// I/O error (EIO).
    IoErr = 5,
    /// No such device or address (ENXIO).
    NoSuchDeviceOrAddress = 6,
    /// Argument list too long (E2BIG).
    TooBig = 7,
    /// Executable format error (ENOEXE).
    InvalidExecutableFormat = 8,
    /// Bad file number (EBADF).
    BadFile = 9,
    /// No child processes (ECHILD).
    NoChildProcess = 10,
    /// Try again (EAGAIN).
    TryAgain = 11,
    /// Out of memory (ENOMEM).
    OutOfMemory = 12,
    /// Permission denied (EACCES).
    PermissionDenied = 13,
    /// Bad address (EFAULT).
    BadAddress = 14,
    /// Not a device required (ENOTBLK).
    NotBlockDevice = 15,
    /// Device or resource busy (EBUSY).
    ResourceBusy = 16,
    /// Entry Exists (EEXIST).
    EntryExists = 17,
    /// Cross-device link (EXDEV).
    CrossDeviceLink = 18,
    /// No such device (ENODEV).
    NoSuchDevice = 19,
    /// Not a directory (ENOTDIR).
    InvalidDirectory = 20,
    /// Is a directory (EISDIR).
    IsDirectory = 21,
    /// Invalid argument (EINVAL).
    InvalidArgument = 22,
    /// File table overflow (ENFILE).
    FileTableOVerflow = 23,
    /// Too many open files (EMFILE).
    TooManyOpenFiles = 24,
    /// Not a typewriter (ENOTTY).
    InvalidTerminalOperation = 25,
    /// Text file busy (ETXTBSY).
    TextFileBusy = 26,
    /// File too large (EFBIG).
    FileTooLarge = 27,
    /// No space left on device (ENOSPC).
    NoSpaceOnDevice = 28,
    /// Illegal seek (ESPIPE).
    IllegalSeek = 29,
    /// Read-only file system (EROFS).
    ReadOnlyFileSystem = 30,
    /// Too many links (EMLINK).
    TooManyLinks = 31,
    /// Broken pipe (EPIPE).
    BrokenPipe = 32,
    /// Math argument out of domain of function (EDOM).
    MathArgDomainErr = 33,
    /// Math result not representable (ERANGE).
    ValueOutOfRange = 34,
    /// Resource deadlock would occur (EDEADLK).
    Deadlock = 35,
    /// File name too long (ENAMETOOLONG).
    NameTooLong = 36,
    /// No record locks available (ENOLCK).
    LockNotAvailable = 37,
    /// Invalid system call number (ENOSYS).
    InvalidSysCall = 38,
    /// Directory not empty (ENOTEMPTY).
    DirectoryNotEmpty = 39,
    /// Too many symbolic links encountered (ELOOP).
    SymbolicLinkLoop = 40,
    /// Operation would block (EWOULDBLOCK).
    OperationWouldBlock = 41,
    /// No message of desired type (ENOMSG).
    NoMessageAvailable = 42,
    /// Identifier removed (EIDRM).
    IdentifierRemoved = 43,
    /// Channel number out of range (ECHRNG).
    OutOfRangeChannel = 44,
    /// Level 2 not synchronized (EL2NSYNC).
    Level2NotSynchronized = 45,
    /// Level 3 halted (EL3HLT).
    Level3Halted = 46,
    /// Level 3 reset (EL3RST).
    Level3Reset = 47,
    /// Link number out of range (ELNRNG).
    InvalidLinkNumber = 48,
    /// Protocol driver not attached (EUNATCH).
    InvalidProtocolDriver = 49,
    /// No CSI structure available (ENOCSI).
    NoStructAvailable = 50,
    /// Level 2 halted (EL2HLT).
    Level2Halted = 51,
    /// Invalid exchange (EBADE).
    InvalidExchange = 52,
    /// Invalid request descriptor (EBADR).
    InvalidRequestDescriptor = 53,
    /// Exchange full (EXFULL).
    ExchangeFull = 54,
    /// No anode (ENOANO).
    InvalidAnode = 55,
    /// Invalid request code (EBADRQC).
    InvalidRequestCode = 56,
    /// Invalid slot (EBADSLT).
    InvalidSlot = 57,
    /// Resource deadlock would occur (EDEADLOCK).
    DeadlockWouldOccur = 58,
    /// Bad font file format (ENOSTR).
    BadFontFormat = 59,
    /// Device not a stream (ENOSTR).
    NoStreamDeviceAvailable = 60,
    /// No data available (ENODATA).
    NoDataAvailable = 61,
    /// Timer expired (ETIME).
    TimerExpired = 62,
    /// Out of streams resources (ENOSR).
    NoStreamResources = 63,
    /// Machine is not on the network (ENONET).
    NoNetwork = 64,
    /// Package not installed (ENOPKG).
    MissingPackage = 65,
    /// Object is remote (EREMOTE).
    RemoteObject = 66,
    /// Link has been severed (ENOLINK).
    NoLink = 67,
    /// Advertise error (EADV).
    AdvertiseErr = 68,
    /// Remote file share system mount error (ESRMNT).
    MountErr = 69,
    /// Communication error on send (ECOMM).
    CommunicationErr = 70,
    /// Protocol error (EPROTO).
    ProtocolErr = 71,
    /// Multi-hop attempted (EMULTIHOP).
    MultipleHopAttemped = 72,
    /// RFS specific error (EDOTDOT).
    RfsErr = 73,
    /// Not a data message (EBADMSG).
    InvalidMessage = 74,
    /// Value too large for defined data type (EOVERFLOW).
    ValueOverflow = 75,
    /// Name not unique on network (ENOTUNIQ).
    NonUniqueName = 76,
    /// File descriptor in bad state (EBADFD).
    InvalidFileDescriptor = 77,
    /// Remote address changed (EREMCHG).
    RemoteAddressChanged = 78,
    /// Can not access a needed shared library (ELIBACC).
    LibraryAccessErr = 79,
    /// Accessing a corrupted shared library (ELIBBAD).
    InvalidLibraryAccess = 80,
    /// .lib section in a.out corrupted (ELIBSCN).
    CorruptedLibSection = 81,
    /// Attempting to link in too many shared libraries (ELIBMAX).
    ExcessiveLibraryLinkCount = 82,
    /// Cannot exec a shared library directly (ELIBEXEC).
    InvalidExecSharedLibrary = 83,
    /// Illegal byte sequence (EILSEQ).
    IllegalByteSequence = 84,
    /// Interrupted system call should be restarted (ERESTART).
    RestartRequired = 85,
    /// Streams pipe error (ESTRPIPE).
    StreamPipeErr = 86,
    /// Too many users (EUSERS).
    TooManyUsers = 87,
    /// Socket operation on non-socket (ENOTSOCK).
    NotSocketFile = 88,
    /// Destination address required (EDESTADDRREQ).
    DestinationAddressRequired = 89,
    /// Message too long (EMSGSIZE).
    MessageTooLong = 90,
    /// Protocol wrong type for socket (EPROTOTYPE).
    BadProtocolType = 91,
    /// Protocol not available (ENOPROTOOPT).
    ProtocolOptionNotAvailable = 92,
    /// Protocol not supported (EPROTONOSUPPORT).
    ProtocolNotSupported = 93,
    /// Socket type not supported (ESOCKTNOSUPPORT).
    SocketTypeNotSupported = 94,
    /// Operation not supported on transport endpoint (EOPNOTSUPP).
    OperationNotSupported = 95,
    /// Protocol family not supported (EPFNOSUPPORT).
    ProtocolFamilyNotSupported = 96,
    /// Address family not supported by protocol (EAFNOSUPPORT).
    AddressFamilyNotSupported = 97,
    /// Address already in use (EADDRINUSE).
    AddressInUse = 98,
    /// Cannot assign requested address (EADDRNOTAVAIL).
    AddressNotAvailable = 99,
    /// Network is down (ENETDOWN).
    NetworkDown = 100,
    /// Network is unreachable (ENETUNREACH).
    NetworkUnreachable = 101,
    /// Network dropped connection because of reset (ENETRESET).
    NetworkReset = 102,
    /// Software caused connection abort (ECONNABORTED).
    ConnectionAborted = 103,
    /// Connection reset by peer (ECONNRESET).
    ConnectionReset = 104,
    /// No buffer space available (ENOBUFS).
    NoBufferSpace = 105,
    /// Transport endpoint is already connected (EISCONN).
    TransportEndpointConnected = 106,
    /// Transport endpoint is not connected (ENOTCONN).
    TransportEndpointNotConnected = 107,
    /// Cannot send after transport endpoint shutdown (ESHUTDOWN).
    TransportEndpointShutdown = 108,
    /// Too many references: cannot splice (ETOOMANYREFS).
    TooManyReferences = 109,
    /// Connection timed out (ETIMEDOUT).
    ConnectionTimeout = 110,
    /// Connection refused (ECONNREFUSED).
    ConnectionRefused = 111,
    /// Host is down (EHOSTDOWN).
    HostDown = 112,
    /// No route to host (EHOSTUNREACH).
    HostUnreachable = 113,
    /// Operation already in progress (EALREADY).
    OperationAlreadyInProgress = 114,
    /// Operation now in progress (EINPROGRESS).
    OperationInProgress = 115,
    /// Stale file handle (ESTALE).
    StaleHandle = 116,
    /// Structure needs cleaning (EUCLEAN).
    UncleanStructure = 117,
    /// Not a XENIX named type file (ENOTNAM).
    NoXenixtNamedTypeFile = 118,
    /// No XENIX semaphores available (ENAVAIL).
    NoXenixSemaphoresAvailable = 119,
    /// Is a named type file (EISNAM).
    IsNamedTypeFile = 120,
    /// Remote I/O error (EREMOTEIO).
    RemoteIOErr = 121,
    /// Quota exceeded (EDQUOT).
    QuotaExceeded = 122,
    /// No medium found (ENOMEDIUM).
    MediumNotFound = 123,
    /// Wrong medium type (EMEDIUMTYPE).
    InvalidMediumType = 124,
    /// Operation Canceled (ECANCELED).
    OperationCanceled = 125,
    /// Required key not available (ENOKEY).
    MissingKey = 126,
    /// Key has expired (EKEYEXPIRED).
    ExpiredKey = 127,
    /// Key has been revoked (EKEYREVOKED).
    KeyRevoked = 128,
    /// Key was rejected by service (EKEYREJECTED).
    KeyRejected = 129,
    /// Owner died (EOWNERDEAD).
    DeadOwner = 130,
    /// State not recoverable (ENOTRECOVERABLE).
    UnrecoverableState = 131,
    /// Operation not possible due to RF-kill (ERFKILL).
    RfKillSwitch = 132,
    /// Memory page has hardware error (EHWPOISON).
    HardwarePoison = 133,
}

impl ErrorCode {
    ///
    /// # Description
    ///
    /// Converts an [`ErrorCode`] into an `errno` value.
    ///
    pub fn into_errno(self) -> i32 {
        -(self as i32)
    }
}

#[derive(Debug)]
pub struct Error {
    pub code: ErrorCode,
    pub reason: &'static str,
}

impl Error {
    pub fn new(code: ErrorCode, reason: &'static str) -> Self {
        Self { code, reason }
    }
}

//==================================================================================================
// Implementations
//==================================================================================================

impl core::error::Error for ErrorCode {}

impl core::fmt::Display for ErrorCode {
    fn fmt(&self, f: &mut core::fmt::Formatter) -> core::fmt::Result {
        write!(f, "error={:?}", self)
    }
}
