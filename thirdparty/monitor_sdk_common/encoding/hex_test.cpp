// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "thirdparty/monitor_sdk_common/encoding/hex.h"
#include "thirdparty/monitor_sdk_common/base/array_size.h"

#include "thirdparty/gtest/gtest.h"

namespace common {

const char kTestData[] = { '\x12', '\x34', '\x56', '\xFF' };

TEST(HexEncoding, Generic)
{
    char result[2 * COMMON_ARRAY_SIZE(kTestData) + 1] = {};

    char end_char = result[2 * COMMON_ARRAY_SIZE(kTestData)];
    Hex::Encode(kTestData, kTestData + COMMON_ARRAY_SIZE(kTestData), result, true);
    EXPECT_EQ(end_char, result[2 * COMMON_ARRAY_SIZE(kTestData)]);

    result[2 * COMMON_ARRAY_SIZE(kTestData)] = '\0';
    EXPECT_STREQ("123456FF", result);
}

static bool IsEqual(const std::string& str, const std::vector<char>& v)
{
    return str.size() == v.size() &&
        std::equal(str.begin(), str.end(), v.begin());
}

TEST(HexEncoding, Buffer)
{
    char result[2 * COMMON_ARRAY_SIZE(kTestData) + 1];
    EXPECT_STREQ("123456FF", Hex::EncodeToBuffer(kTestData, COMMON_ARRAY_SIZE(kTestData),
                                                 result, true));
}

TEST(HexEncoding, HexEncodeAppend)
{
    std::string result = "0x";
    EXPECT_EQ("0x123456FF", Hex::EncodeAppend(kTestData, COMMON_ARRAY_SIZE(kTestData),
                                              &result, true));

    std::vector<char> vector_result;
    vector_result.push_back('0');
    vector_result.push_back('x');
    Hex::EncodeAppend(kTestData, COMMON_ARRAY_SIZE(kTestData), &vector_result, true);
    EXPECT_TRUE(IsEqual(result, vector_result));
}

TEST(HexEncoding, HexEncodeTo)
{
    std::string result = "0x";
    EXPECT_EQ("123456FF", Hex::EncodeTo(kTestData, COMMON_ARRAY_SIZE(kTestData),
                                        &result, true));

    std::vector<char> vector_result;
    Hex::EncodeTo(kTestData, COMMON_ARRAY_SIZE(kTestData), &vector_result, true);
    EXPECT_TRUE(IsEqual(result, vector_result));
}

TEST(HexEncoding, Case)
{
    EXPECT_EQ("123456ff", Hex::EncodeAsString(kTestData, COMMON_ARRAY_SIZE(kTestData), false));
    EXPECT_EQ("123456FF", Hex::EncodeAsString(kTestData, COMMON_ARRAY_SIZE(kTestData), true));
}

} // namespace common
