// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "thirdparty/monitor_sdk_common/system/time/clock.h"

#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(RealtimeClock, Test)
{
    int64_t us = RealtimeClock.MicroSeconds();
    ASSERT_GT(us, 0);
}

} // namespace common
