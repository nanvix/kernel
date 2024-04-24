# `seteuid()`

## Name

`seteuid()` - Set the effective user identifier (euid) of the calling process.

## Synopsis

```rust
use nanvix::iam::{seteuid, UserID};

fn seteuid(uid: UserID) -> i32;
```

## Description

`seteuid()` sets the effective user identifier of the calling process to `euid`, if it belongs to a
privileged user.

## Return Value

Upon successful completion, `seteuid()` returns zero. Otherwise, it returns a negative error code
instead.

## Errors

- `EPERM` The calling process does not belong to a privileged user.

## Disclaimer

Any behavior that is not documented in this manual page is unintentional and should be reported.

## See Also

`getegid()`, `geteuid()`, `getgid()`, `getuid()`, `setegid()`, `setgid()`, `setuid()`.
