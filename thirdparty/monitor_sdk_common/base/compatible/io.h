// Copyright 2010, Vivo Inc.

#ifndef COMMON_BASE_COMPATIBLE_IO_H
#define COMMON_BASE_COMPATIBLE_IO_H

#if defined __GNUC__
#include <unistd.h>
#elif defined _MSC_VER
#include <io.h>
#else
#error Unknown compiler
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined _MSC_VER
// It's not defined as inline because the implementation is not trivial and we
// don't want to introduce <windows.h> which might contaminate the global
// namespace.
int fsync(int fd);
#endif // _MSC_VER

#ifdef __cplusplus
} // extern "C"
#endif

#endif // COMMON_BASE_COMPATIBLE_IO_H
