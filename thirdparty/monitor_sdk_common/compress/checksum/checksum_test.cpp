// Copyright (c) 2012, Vivo Inc.
// All rights reserved.
//
// Created: 01/13/12
// Description:

#include "thirdparty/monitor_sdk_common/compress/checksum/checksum.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(CheckSum, Fletcher32)
{
    EXPECT_EQ(0xFFFFFFFFU, fletcher32("", 0));
    EXPECT_EQ(0x810943d2U, fletcher32("hello", 5));
}

TEST(CheckSum, Fletcher32A)
{
    EXPECT_EQ(0xFFFFFFFFU, fletcher32a("", 0));
    // user must pass 16-bits alligned data into fletcher32a
    // and its length should be bytes in data / 2
    // should re-defined fletcher32a interface and its definition
    EXPECT_EQ(0x3D37D4D1U, fletcher32a("hell", 2));
}

TEST(CheckSum, Crc32)
{
    EXPECT_EQ(0U, crc32("", 0));
    EXPECT_EQ(0x3610a686U, crc32("hello", 5));
}

TEST(CheckSum, Adler32)
{
    EXPECT_EQ(1U, adler32("", 0));
    EXPECT_EQ(0x62C0215U, adler32("hello", 5));
}

} // namespace common
