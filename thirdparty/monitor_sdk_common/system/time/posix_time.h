//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:13
//  @file:      posix_time.h
//  @brief:     
//
//********************************************************************

#ifndef COMMON_SYSTEM_TIME_POSIX_TIME_H
#define COMMON_SYSTEM_TIME_POSIX_TIME_H

#ifdef __unix__

#include "thirdparty/monitor_sdk_common/base/stdint.h"

struct timespec;

namespace common {

// for any timed* functions using absolute timespec
void RelativeTimeInMillSecondsToAbsTimeInTimeSpec(
    int64_t relative_time_in_ms,
    timespec* ts
);

// for any timed* functions using absolute timespec
void RelativeMilliSecondsToAbsolute(
    int64_t relative_time_in_ms,
    timespec* ts
);

} // end of namespace common

#else
#error for POSIX compatible platforms only
#endif


#endif // COMMON_SYSTEM_TIME_POSIX_TIME_H
