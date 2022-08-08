// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "thirdparty/monitor_sdk_common/encoding/base64.h"
#include "thirdparty/gtest/gtest.h"

const std::string kText = ".<>@???????";
const std::string kBase64Text = "Ljw+QD8/Pz8/Pz8=";
const std::string kWebSafeBase64Text = "Ljw-QD8_Pz8_Pz8=";

namespace common {

TEST(Base64Test, Encode)
{
    std::string result;
    EXPECT_TRUE(Base64::Encode(kText, &result));
    EXPECT_EQ(kBase64Text, result);
}

TEST(Base64Test, Decode)
{
    std::string result;
    EXPECT_FALSE(Base64::WebSafeDecode(kBase64Text, &result));
    EXPECT_TRUE(Base64::Decode(kBase64Text, &result));
    EXPECT_EQ(kText, result);
}

TEST(Base64Test, WebSafeEncode)
{
    std::string result;
    EXPECT_TRUE(Base64::WebSafeEncode(kText, &result));
    EXPECT_EQ(kWebSafeBase64Text, result);
}

TEST(Base64Test, WebSafeDecode)
{
    std::string result;
    EXPECT_FALSE(Base64::Decode(kWebSafeBase64Text, &result));
    EXPECT_TRUE(Base64::WebSafeDecode(kWebSafeBase64Text, &result));
    EXPECT_EQ(kText, result);
}

TEST(Base64Test, Key1)
{
    std::string input = "aGJy14F+3czpGies6gpgDVyZ3YneHRuamarUmf4M/n0=";
    std::string output = "";
    std::string middle = "";
    EXPECT_TRUE(Base64::Decode(input, &middle));
    EXPECT_TRUE(Base64::Encode(middle, &output));
    EXPECT_EQ(input, output);
}

TEST(Base64Test, Key2)
{
    std::string input = "/sd0dbOPB9IHgldqOoiVSdB8LSiIeZhfJKNlv+YCP5jyYGqmisPVYNX+zA==";
    std::string output = "";
    std::string middle = "";
    EXPECT_TRUE(Base64::Decode(input, &middle));
    EXPECT_TRUE(Base64::Encode(middle, &output));
    EXPECT_EQ(input, output);
}

} // namespace common
