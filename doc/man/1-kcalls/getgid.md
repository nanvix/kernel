# `getgid()`

## Name

`getgid()` - Get the real user group identifier (gid) of the calling process.

## Synopsis

```rust
use nanvix::iam::{getgid, GroupID};

fn getgid() -> GroupID;
```

## Description

`getgid()` returns the real user group identifier of the calling process.

## Return Value

`getgid()` returns the real user group identifier of the calling process.

## Errors

None.

## Disclaimer

Any behavior that is not documented in this manual page is unintentional and should be reported.

## See Also

`getegid()`, `geteuid()`, `getuid()`, `setegid()`, `seteuid()`, `setgid()`, `setuid()`.
