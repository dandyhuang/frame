//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:13
//  @file:      posix_time.cpp
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/time/posix_time.h"

#ifdef __unix__
#include <stddef.h>
#include <time.h>
#include <sys/time.h>

namespace common {

void RelativeTimeInMillSecondsToAbsTimeInTimeSpec(
    int64_t relative_time_in_ms,
    timespec* ts
    )
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t usec = tv.tv_usec + relative_time_in_ms * 1000LL;
    ts->tv_sec = tv.tv_sec + usec / 1000000;
    ts->tv_nsec = (usec % 1000000) * 1000;
}

void RelativeMilliSecondsToAbsolute(
    int64_t relative_time_in_ms,
    timespec* ts
    )
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t usec = tv.tv_usec + relative_time_in_ms * 1000LL;
    ts->tv_sec = tv.tv_sec + usec / 1000000;
    ts->tv_nsec = (usec % 1000000) * 1000;
}

} // end of namespace common

#endif
