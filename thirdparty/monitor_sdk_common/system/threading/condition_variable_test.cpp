// Copyright (c) 2011, The Toft Authors. All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>

#include "thirdparty/monitor_sdk_common/system/threading/condition_variable.h"

#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(ConditionVariable, Init)
{
    ConditionVariable cond;
}

TEST(ConditionVariable, Wait)
{
    ConditionVariable cond;
    cond.Signal();
}

TEST(ConditionVariable, Release)
{
}

} // namespace common
