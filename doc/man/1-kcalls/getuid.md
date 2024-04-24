# `getuid()`

## Name

`getuid()` - Get the real user identifier (uid) of the calling process.

## Synopsis

```rust
use nanvix::iam::{getuid, UserID};

fn getuid() -> UserID;
```

## Description

`getuid()` returns the real user identifier of the calling process.

## Return Value

`getuid()` returns the real user identifier of the calling process.

## Errors

None.

## Disclaimer

Any behavior that is not documented in this manual page is unintentional and should be reported.

## See Also

`getegid()`, `geteuid()`, `getgid()`, `getuid()`, `setegid()`, `seteuid()`, `setgid()`, `setuid()`.
