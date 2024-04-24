# `geteuid()`

## Name

`geteuid()` - Get the effective user identifier (euid) of the calling process.

## Synopsis

```rust
use nanvix::iam::{geteuid, UserID};

fn geteuid() -> UserID;
```

## Description

`geteuid()` returns the effective user identifier of the calling process.

## Return Value

`geteuid()` returns the effective user identifier of the calling process.

## Errors

None.

## Disclaimer

Any behavior that is not documented in this manual page is unintentional and should be reported.

## See Also

`getegid()`, `getgid()`, `getuid()`, `setegid()`, `seteuid()`, `setgid()`, `setuid()`.
