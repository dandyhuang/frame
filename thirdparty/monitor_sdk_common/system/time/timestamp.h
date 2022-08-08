//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:19
//  @file:      timestamp.h
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_TIME_TIMESTAMP_H
#define COMMON_SYSTEM_TIME_TIMESTAMP_H

#include "thirdparty/monitor_sdk_common/base/deprecate.h"
#include "thirdparty/monitor_sdk_common/base/stdint.h"

namespace common {

/// time stamp in millisecond (1/1000 second)
int64_t GetTimeStampInMs();

DEPRECATED_BY(GetTimeStampInMs)
inline int64_t GetTimeStamp()
{
    return GetTimeStampInMs();
}

/// time stamp in microsecond (1/1000000 second)
int64_t GetTimeStampInUs();

} // end of namespace common

#endif // COMMON_SYSTEM_TIME_TIMESTAMP_H
