# `excpwait()`

## Name

`excpwait()` - Waits for an exception to happen.

## Synopsis

```c
#include <nanvix/kcall.h>

int excpwait(int exnum, struct exception *excp);
```

## Description

The `excpwait()` function waits for an exception to happen. The `exnum` argument specifies the exception number, and the `excp` argument specifies the location where exception information should be stored.

## Return Value

Upon successful completion, `excpwait()` returns zero. Upon failure, a negative error code is
returned instead.

## Errors

- `EFAULT` - Bad storage location for exception information.
- `EINVAL` - Invalid storage location for exception information.
- `EPERM` - The calling process does not own an exception line.

## Disclaimer

Any behavior that is not documented in this manual page is unintentional and should be reported.

## See Also

`excpctrl()`, `excpresume()`.
