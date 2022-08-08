// Copyright (c) 2008, Vivo Inc.
// All rights reserved.
//

#include "thirdparty/monitor_sdk_common/crypto/hash/md5_2.h"

#include <stdio.h>
#include <string.h>
#include <string>

#include "thirdparty/monitor_sdk_common/base/byte_order.h"
#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(MD5, Digester)
{
    MD5 md5;
    md5.Update("");
    ASSERT_EQ("d41d8cd98f00b204e9800998ecf8427e", md5.HexFinal());
    md5.Update("");

    md5.Init();
    md5.Update("The quick brown fox jumps over the lazy dog");
    ASSERT_EQ("9e107d9d372bb6826bd81d3542a419d6", md5.HexFinal());
}

TEST(MD5, HexDigest)
{
    ASSERT_EQ("d41d8cd98f00b204e9800998ecf8427e", MD5::HexDigest(""));
    ASSERT_EQ("9e107d9d372bb6826bd81d3542a419d6",
              MD5::HexDigest("The quick brown fox jumps over the lazy dog"));
}

TEST(MD5, Digest64)
{
    ASSERT_EQ(htonll(0xd41d8cd98f00b204ULL), MD5::Digest64(""));
    ASSERT_EQ(htonll(0x0cc175b9c0f1b6a8ULL), MD5::Digest64("a"));
    ASSERT_EQ(htonll(0x900150983cd24fb0ULL), MD5::Digest64("abc"));
    ASSERT_EQ(htonll(0xf96b697d7cb7938dULL), MD5::Digest64("message digest"));
    ASSERT_EQ(htonll(0xc3fcd3d76192e400ULL), MD5::Digest64("abcdefghijklmnopqrstuvwxyz"));
    ASSERT_EQ(htonll(0x9e107d9d372bb682ULL),
              MD5::Digest64("The quick brown fox jumps over the lazy dog"));

    static const char message[] = "message digest";
    static const size_t message_length = strlen(message);
    unsigned long long result = htonll(0xf96b697d7cb7938dULL);

    ASSERT_EQ(result, MD5::Digest64(message));
    ASSERT_EQ(result, MD5::Digest64(message, message_length));
}

TEST(MD5, Digest)
{
    char digest[MD5::kDigestLength];
    MD5::Digest("The quick brown fox jumps over the lazy cog", digest);
    ASSERT_EQ(0x10, digest[0]);
    ASSERT_EQ(0x4b, digest[15]);
}

TEST(MD5, Digest64String)
{
    ASSERT_EQ("17476731383137391888",
              NumberToString(MD5::Digest64("The quick brown fox jumps over the lazy cog")));
}

} // namespace common
