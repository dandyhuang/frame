//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:18
//  @file:      time_utils_test.cpp
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/time/time_utils.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(TimeUtils, TestUtils)
{
    LOG(INFO) << "milliseconds(1/1000s) since Jan 1, 1970 GMT: "  << TimeUtils::Milliseconds();
    LOG(INFO) << "milliseconds(1/1000000s) since Jan 1, 1970 GMT: " << TimeUtils::Microseconds();
    LOG(INFO) << "offset in hours between local time and GMT (or UTC) time: "
              << TimeUtils::GetGMTOffset();
    LOG(INFO) << "time of today: " << TimeUtils::GetCurTime();
    LOG(INFO) << "time of today: " << TimeUtils::GetCurMilliTime();
}

} // end of namespace common
