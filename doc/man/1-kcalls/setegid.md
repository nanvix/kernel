# `setegid()`

## Name

`setegid()` - Set the effective user group identifier (egid) of the calling process.

## Synopsis

```rust
use nanvix::iam::{setegid, GroupID};

fn setegid(gid: GroupID) -> i32;
```

## Description

`setegid()` sets the effective user group identifier of the calling process to `egid`, if it belongs
to a privileged user.

## Return Value

Upon successful completion, `setegid()` returns zero. Otherwise, it returns a negative error code
instead.

## Errors

- `EPERM` The calling process does not belong to a privileged user, and the effective group ID does
not match the real group ID of the calling process.

## Disclaimer

Any behavior that is not documented in this manual page is unintentional and should be reported.

## See Also

`getegid()`, `geteuid()`, `getgid()`, `getuid()`, `seteuid()`, `setgid()`, `setuid()`.
