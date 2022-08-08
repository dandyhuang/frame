//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 16:11
//  @file:      stdlib.h
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_COMPATIBLE_STDLIB_H
#define COMMON_BASE_COMPATIBLE_STDLIB_H

#include <stdlib.h>
#include <float.h>
#include <errno.h>
#include "thirdparty/monitor_sdk_common/base/compatible/internal.h"

#ifdef _MSC_VER
COMPATIBLE_INLINE float strtof(const char *nptr, char **endptr)
{
    double d = strtod(nptr, endptr);
    if (d < FLT_MIN || d > FLT_MAX)
        errno = ERANGE;
    return static_cast<float>(d);
}

// MSVC: double and long double share same binary representation
COMPATIBLE_INLINE long double strtold(const char *nptr, char **endptr)
{
    return strtod(nptr, endptr);
}

COMPATIBLE_INLINE
long long int strtoll(const char *nptr, char **endptr, int base)
{
    return _strtoi64(nptr, endptr, base);
}

COMPATIBLE_INLINE
unsigned long long int strtoull(const char *nptr, char **endptr, int base)
{
    return _strtoui64(nptr, endptr, base);
}

COMPATIBLE_INLINE
long long int llabs(long long int n)
{
    return _abs64(n);
}

#endif

#endif // COMMON_BASE_COMPATIBLE_STDLIB_H

