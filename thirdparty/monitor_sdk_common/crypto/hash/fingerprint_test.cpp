// Copyright (c) 2012, Vivo Inc.
// All rights reserved.
//
// Created: 07/09/2012

#include "thirdparty/monitor_sdk_common/crypto/hash/fingerprint.h"
#include "thirdparty/gtest/gtest.h"

namespace common{

TEST(FingerPrint, Buffer)
{
    EXPECT_EQ(0UL, FingerPrint("", 0));
    EXPECT_EQ(279172874305UL, FingerPrint("A", 1));
    EXPECT_EQ(545833254715463890UL, FingerPrint("hello", 5));
}

TEST(FingerPrint, CString)
{
    EXPECT_EQ(0UL, FingerPrint(""));
    EXPECT_EQ(279172874305UL, FingerPrint("A"));
    EXPECT_EQ(545833254715463890UL, FingerPrint("hello"));
}

} // namespace common
