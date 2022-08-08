// Copyright (c) 2013, Vivo Inc.
// All rights reserved.
//
// Created: 2013-03-22

#include "thirdparty/monitor_sdk_common/crypto/hash/md4.h"
#include "thirdparty/gtest/gtest.h"

namespace common{

TEST(MD4, HexDigest)
{
    // Standard test vectors, see http://en.wikipedia.org/wiki/MD4
    EXPECT_EQ("31d6cfe0d16ae931b73c59d7e0c089c0", MD4::HexDigest(""));
    EXPECT_EQ("bde52cb31de33e46245e05fbdbd6fb24", MD4::HexDigest("a"));
    EXPECT_EQ("a448017aaf21d8525fc10ae87aa6729d", MD4::HexDigest("abc"));
    EXPECT_EQ("d9130a8164549fe818874806e1c7014b",
              MD4::HexDigest("message digest"));
    EXPECT_EQ("d79e1c308aa5bbcdeea8ed63df412da9",
              MD4::HexDigest("abcdefghijklmnopqrstuvwxyz"));
    EXPECT_EQ("043f8582f241db351ce627e153e7f0e4",
              MD4::HexDigest("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmn"
                             "opqrstuvwxyz0123456789"));
    EXPECT_EQ("e33b4ddc9c38f2199c3e7b164fcc0536",
              MD4::HexDigest("1234567890123456789012345678901234567890"
                             "1234567890123456789012345678901234567890"));
}

} // namespace common
