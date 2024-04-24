# `setuid()`

## Name

`setuid()` - Set the real user identifier (uid) of the calling process.

## Synopsis

```rust
use nanvix::iam::{setuid, UserID};

fn setuid(uid: UserID) -> i32;
```

## Description

`setuid()` sets the real user identifier of the calling process to `uid`. If the calling process is
owned by a privileged user, then the effective user identifier is also set to `uid`.

## Return Value

Upon successful completion, `setuid()` returns zero. Otherwise, it returns a negative error code
instead.

## Errors

- `EPERM` The calling process does not belong to a privileged user, and the effective user ID does
not match the real user identifier of the calling process.

## Disclaimer

Any behavior that is not documented in this manual page is unintentional and should be reported.

## See Also

`getegid()`, `geteuid()`, `getgid()`, `getuid()`, `setegid()`, `seteuid()`, `setgid()`.
