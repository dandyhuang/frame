// Copyright 2010, Vivo Inc.
//
// Defines checksum functions base on CRC algorithms.
// Refer to: http://en.wikipedia.org/wiki/Cyclic_redundancy_check

#include "thirdparty/monitor_sdk_common/crypto/hash/crc.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

// ≤‚ ‘º”√‹Ω‚√‹∫Ø ˝
TEST(CRC32, BaseOperation)
{
    const char *string = "/home/ivanhuang/data/test";
    size_t size = strlen(string);

    unsigned int now_crc = 0;
    unsigned int old_crc = 12345;

    for (int i = 0; i < 50; ++i)
    {
        now_crc = UpdateCRC32(string, size, old_crc);

        ASSERT_NE(now_crc, old_crc);

        old_crc = now_crc;
    }
}

TEST(CRC16, BaseOperation16)
{
    EXPECT_EQ(0x62U, CRC16Hash8("hello", 5));
}

} // namespace common
