# `getegid()`

## Name

`getegid()` - Get the effective user group identifier (egid) of the calling process.

## Synopsis

```rust
use nanvix::iam::{getegid, GroupID};

fn getegid() -> GroupID;
```

## Description

`getegid()` returns the effective user group identifier of the calling process.

## Return Value

`getegid()` returns the effective user group identifier of the calling process.

## Errors

None.

## Disclaimer

Any behavior that is not documented in this manual page is unintentional and should be reported.

## See Also

`geteuid()`, `getgid()`, `getuid()`, `setegid()`, `seteuid()`, `setgid()`, `setuid()`.
