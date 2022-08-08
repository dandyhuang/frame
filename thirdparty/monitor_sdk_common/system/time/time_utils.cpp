//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:17
//  @file:      time_utils.cpp
//  @brief:     
//
//********************************************************************



#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "thirdparty/monitor_sdk_common/system/time/time_utils.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifdef __unix__
#include <sys/time.h>
#elif defined _WIN32
#include "thirdparty/monitor_sdk_common/base/common_windows.h"
#endif

#if _WIN32

namespace common {

/*
 * Number of 100 nanosecond units from 1/1/1601 to 1/1/1970
 */
#define EPOCH_BIAS  116444736000000000i64

int64_t TimeUtils::Milliseconds()
{
    ULARGE_INTEGER uli;
    GetSystemTimeAsFileTime(reinterpret_cast<FILETIME*>(&uli));
    return (uli.QuadPart - EPOCH_BIAS) / 10000;
}

int64_t TimeUtils::Microseconds()
{
    ULARGE_INTEGER uli;
    GetSystemTimeAsFileTime(reinterpret_cast<FILETIME*>(&uli));
    return (uli.QuadPart - EPOCH_BIAS) / 10;
}

int TimeUtils::GetGMTOffset()
{
    TIME_ZONE_INFORMATION tzInfo;
    DWORD theErr = ::GetTimeZoneInformation(&tzInfo);
    if (theErr == TIME_ZONE_ID_INVALID)
        return 0;

    return ((tzInfo.Bias / 60) * -1);
}

} // end of namespace common

#endif // _WIN32

#ifdef __unix__

#include "thirdparty/monitor_sdk_common/system/time/timestamp.h"

namespace common {

int64_t TimeUtils::Milliseconds()
{
    return GetTimeStampInMs();
}

int64_t TimeUtils::Microseconds()
{
    return GetTimeStampInUs();
}

int TimeUtils::GetGMTOffset()
{
    struct timeval tv;
    struct timezone tz;

    int err = ::gettimeofday(&tv, &tz);
    if (err != 0)
        return 0;

    return ((tz.tz_minuteswest / 60) * -1); // return hours before or after GMT
}

} // end of namespace common

#endif // __unix__

namespace common {

std::string TimeUtils::GetCurTime()
{
    std::string retStr;
    char s[50];
    struct tm curr;

    time_t  iCurTime;

    time(&iCurTime);

    curr = *localtime(&iCurTime);

    if (curr.tm_year > 50)
    {
        sprintf(s, "%04d%02d%02d%02d%02d%02d",
                curr.tm_year+1900, curr.tm_mon+1, curr.tm_mday,
                curr.tm_hour, curr.tm_min, curr.tm_sec);
    }
    else
    {
        sprintf(s, "%04d%02d%02d%02d%02d%02d",
                curr.tm_year+2000, curr.tm_mon+1, curr.tm_mday,
                curr.tm_hour, curr.tm_min, curr.tm_sec);
    }
    retStr =s;
    return retStr;
}

std::string TimeUtils::GetCurMilliTime()
{
    std::string retStr;
    char s[50];
    struct tm curr;

    int64_t nowMilliSecs = TimeUtils::Milliseconds();
    time_t  iCurTime = nowMilliSecs/1000;

    curr = *localtime(&iCurTime);

    if (curr.tm_year > 50)
    {
        sprintf(s, "%04d%02d%02d%02d%02d%02d%03d",
                curr.tm_year+1900, curr.tm_mon+1, curr.tm_mday,
                curr.tm_hour, curr.tm_min, curr.tm_sec, (int)(nowMilliSecs%1000));
    }
    else
    {
        sprintf(s, "%04d%02d%02d%02d%02d%02d%03d",
                curr.tm_year+2000, curr.tm_mon+1, curr.tm_mday,
                curr.tm_hour, curr.tm_min, curr.tm_sec, (int)(nowMilliSecs%1000));
    }
    retStr = s;
    return retStr;
}

} // end of namespace common
