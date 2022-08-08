//
// Created by 王锦涛 on 2019-11-18.
//
#include <iostream>
#include <string>

#include "thirdparty/monitor_sdk_common/base/string/bitwise.h"
#include "thirdparty/gtest/gtest.h"
#include "thirdparty/gmock/gmock.h"

using namespace std;

namespace common {

TEST(BitWise, AndTest1)
{
    std::string str1 = "hello";
    std::string str2 = "ok";
    unsigned char result[5];
    BitWise bw(str1, str2);
    bw.And(result);
    ASSERT_THAT(result, testing::ElementsAre(0, 0, 0, 108, 107));
}

TEST(BitWise, AndTest2)
{
    std::string str1 = "thanks";
    std::string str2 = "noworries";
    unsigned char result[9];
    BitWise bw(str2, str1);
    bw.And(result);
    ASSERT_THAT(result, testing::ElementsAre(0, 0, 0, 100, 96, 96, 104, 97, 115));
}

TEST(BitWise, OrTest1)
{
    std::string str1 = "hello";
    std::string str2 = "ok";
    unsigned char result[5];
    BitWise bw(str1, str2);
    bw.Or(result);
    ASSERT_THAT(result, testing::ElementsAre(104, 101, 108, 111, 111));
}

TEST(BitWise, OrTest2)
{
    std::string str1 = "thanks";
    std::string str2 = "noworries";
    unsigned char result[9];
    BitWise bw(str2, str1);
    bw.Or(result);
    ASSERT_THAT(result, testing::ElementsAre(110, 111, 119, 127, 122, 115, 111, 111, 115));
}

TEST(BitWise, XorTest1)
{
    std::string str1 = "hello";
    std::string str2 = "ok";
    unsigned char result[5];
    BitWise bw(str1, str2);
    bw.Xor(result);
    ASSERT_THAT(result, testing::ElementsAre(104, 101, 108, 3 , 4));
}

TEST(BitWise, XorTest2)
{
    std::string str1 = "thanks";
    std::string str2 = "noworries";
    unsigned char result[9];
    BitWise bw(str2, str1);
    bw.Xor(result);
    ASSERT_THAT(result, testing::ElementsAre(110, 111, 119, 27, 26, 19, 7, 14, 0));
}

} // namespace common