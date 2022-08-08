//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:14
//  @file:      time_utils.h
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_TIME_TIME_UTILS_H
#define COMMON_SYSTEM_TIME_TIME_UTILS_H

#include <string.h>
#include <string>
#include "thirdparty/monitor_sdk_common/base/stdint.h"

namespace common {

struct TimeUtils
{
    // Milliseconds always returns milliseconds(1/1000s) since Jan 1, 1970 GMT.
    static int64_t Milliseconds();

    // Milliseconds always returns microseconds(1/1000000s) since Jan 1, 1970 GMT.
    static int64_t Microseconds();

    // Returns the offset in hours between local time and GMT (or UTC) time.
    static int GetGMTOffset();

    static std::string GetCurTime();

    static std::string GetCurMilliTime();
};

} // end of namespace common

#endif // COMMON_SYSTEM_TIME_TIME_UTILS_H

