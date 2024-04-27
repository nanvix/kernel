# `pinfo()`

## Name

`pinfo()` - Gets information about a process.

## Synopsis

```c
#include <nanvix/kcall.h>

int pinfo(int pid, struct process_info *info);
```

## Description

The `pinfo()` function gets information about a process and stores this information in the structure
pointed to by the `info` argument. The `pid` argument specifies which process to get information
about. It has the following semantics:

- If `pid` equals to `PID_SELF`, information about the calling process is retrieved.
- Otherwise, information about the process identified by `pid` is retrieved.

## Return Value

Upon successful completion, `pinfo()` returns zero. Upon failure, a negative error code is returned.

## Errors

- `ENOENT` - No such process.
- `EINVAL` - Invalid storage location.
- `EFAULT` - Bad storage location.

## Disclaimer

Any behavior that is not documented in this manual page is unintentional and should be reported.
