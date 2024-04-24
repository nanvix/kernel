# `setgid()`

## Name

`setgid()` - Set the real user group identifier (gid) of the calling process.

## Synopsis

```rust
use nanvix::iam::{setgid, GroupID};

fn setgid(gid: GroupID) -> i32;
```

## Description

`setgid()` sets the real user group identifier of the calling process to `gid`. If the calling
process is owned by a privileged user, then the effective user group identifier is also set to
`gid`.

## Return Value

Upon successful completion, `setgid()` returns zero. Otherwise, it returns a negative error code
instead.

## Errors

- `EPERM` The calling process does not belong to a privileged user, and the effective user group
identifier does not match the real group ID of the calling process.

## Disclaimer

Any behavior that is not documented in this manual page is unintentional and should be reported.

## See Also

`getegid()`, `geteuid()`, `getgid()`, `getuid()`, `setegid()`, `seteuid()`, `setuid()`.
