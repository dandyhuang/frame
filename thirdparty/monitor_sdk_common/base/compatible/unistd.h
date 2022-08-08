// Copyright (c) 2011, Vivo Inc.
// All rights reserved.
//
// Description: compatible unistd.h

#ifndef COMMON_BASE_COMPATIBLE_UNISTD_H
#define COMMON_BASE_COMPATIBLE_UNISTD_H
#pragma once

#include "thirdparty/monitor_sdk_common/base/compatible/internal.h"

#ifndef __unix__
# ifdef _MSC_VER
#  include <process.h>
#  include <stddef.h>

typedef int pid_t;
typedef intptr_t ssize_t;

COMPATIBLE_INLINE pid_t getpid()
{
    return _getpid();
}

# else
#  error unknown platform
# endif // _MSC_VER
#else
# include <unistd.h>
#endif // __unix__

#endif // COMMON_BASE_COMPATIBLE_UNISTD_H
