# `excpctrl()`

## Name

`excpctrl()` - Controls which action to take when an exception happens.

## Synopsis

```c
#include <nanvix/kcall.h>

int excpctrl(int exnum, int action);
```

## Description

`excpctrl()` controls which action to take when an exception happens. The `exnum` argument specifies the exception number, and the `action` argument specifies the action to take. The following actions are supported:

- `EXCP_HANDLE` - Handle the exception.
- `EXCP_DEFER` - Defer the exception handling to the kernel.

## Return Value

Upon successful completion, `excpctrl()` returns zero. Upon failure, a negative error code is returned instead.

## Errors

- `EBUSY` - The calling process requested to handle an exception that is already being handled.
- `EBUSY` - The calling process requested to defer exception handling to the kernel, but the target
exception line is busy.
- `EINVAL` - Invalid exception number.
- `EINVAL` - Invalid action.
- `EPERM` - The calling process does not own the exception line.

## Disclaimer

Any behavior that is not documented in this manual page is unintentional and should be reported.

## See Also

`excpresume()`, `excpwait()`.
