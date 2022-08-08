// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "thirdparty/monitor_sdk_common/system/memory/barrier.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(MemoryBarrier, Test)
{
    MemoryBarrier();
    MemoryReadBarrier();
    MemoryWriteBarrier();
}

} // namespace common
