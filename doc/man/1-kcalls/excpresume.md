# `excpresume()`

## Name

`excpresume()` - Resumes the execution of a faulting process.

## Synopsis

```c
#include <nanvix/kcall.h>

int excpresume(int exnum);
```

## Description

The `excpresume()` function resumes the execution of a faulting process. The `exnum` argument specifies the exception number.

## Return Value

Upon successful completion, `excpresume()` returns zero. Upon failure, a negative error code is returned instead.

## Errors

- `EINVAL` - Invalid exception number.
- `EPERM` - The calling process does not own the exception line.

## Disclaimer

Any behavior that is not documented in this manual page is unintentional and should be reported.

## See Also

`excpctrl()`, `excpwait()`.
