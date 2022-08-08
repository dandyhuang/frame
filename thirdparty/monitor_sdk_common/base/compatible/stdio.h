//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 15:50
//  @file:      stdio.h
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_COMPATIBLE_STDIO_H
#define COMMON_BASE_COMPATIBLE_STDIO_H

#include <stdio.h>
#include <stdarg.h>

#include "thirdparty/monitor_sdk_common/base/compatible/internal.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
/// If buffer is not enough sprintf should return bytes needed,
/// while _snprintf return -1. fix it.
COMPATIBLE_INLINE int snprintf(char *str, size_t size, const char *format, ...)
{
	int n;
    va_list va;
    va_start(va, format);
    n = vsnprintf(str, size, format, va);
    if (n < 0)
        n = _vscprintf(format, va);
    va_end(va);
    return n;
}

/// 64 bit offset type
typedef __int64 off64_t;

/// MSC needn't fopen64, fopen can handle everything
COMPATIBLE_INLINE FILE* fopen64(const char* name, const char* mode)
{
    return fopen(name, mode);
}

# if _MSC_VER >= 1400
COMPATIBLE_INLINE off64_t ftello64(FILE* fp)
{
    return _ftelli64(fp);
}

COMPATIBLE_INLINE int fseeko(FILE *stream, off64_t offset, int whence)
{
    return _fseeki64(stream, offset, whence);
}
# else
// VC 2005- has no _ftelli64 and _fseeki64
#  include <io.h>
COMPATIBLE_INLINE off64_t ftello64(FILE* fp)
{
    return _telli64(fileno(fp));
}

COMPATIBLE_INLINE int fseeko(FILE *stream, off64_t offset, int whence)
{
    return _lseeki64(fileno(fp), offset, whence);
}
# endif // _MSC_VER >= 1400
#endif // _MSC_VER

#ifdef __cplusplus
} // extern "C"
#endif

#endif // COMMON_BASE_COMPATIBLE_STDIO_H
