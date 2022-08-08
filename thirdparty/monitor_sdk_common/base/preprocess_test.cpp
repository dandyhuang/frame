// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 12/14/11
// Description: test for preprocess.h

#include "thirdparty/monitor_sdk_common/base/preprocess.h"
#include "thirdparty/monitor_sdk_common/base/preprocess_test_helper.h"

#include "thirdparty/gtest/gtest.h"

TEST(Preprocess, Stringize)
{
    EXPECT_STREQ("ABC", COMMON_PP_STRINGIZE(ABC));
}

TEST(Preprocess, Join)
{
    EXPECT_EQ(12, COMMON_PP_JOIN(1, 2));
}

TEST(Preprocess, DisallowInHeader)
{
    COMMON_PP_DISALLOW_IN_HEADER_FILE();
}

TEST(Preprocess, VaNargs)
{
    EXPECT_EQ(0, COMMON_PP_N_ARGS());
    EXPECT_EQ(1, COMMON_PP_N_ARGS(a));
    EXPECT_EQ(2, COMMON_PP_N_ARGS(a, b));
    EXPECT_EQ(3, COMMON_PP_N_ARGS(a, b, c));
    EXPECT_EQ(4, COMMON_PP_N_ARGS(a, b, c, d));
    EXPECT_EQ(5, COMMON_PP_N_ARGS(a, b, c, d, e));
    EXPECT_EQ(6, COMMON_PP_N_ARGS(a, b, c, d, e, f));
    EXPECT_EQ(7, COMMON_PP_N_ARGS(a, b, c, d, e, f, g));
    EXPECT_EQ(8, COMMON_PP_N_ARGS(a, b, c, d, e, f, g, h));
    EXPECT_EQ(9, COMMON_PP_N_ARGS(a, b, c, d, e, f, g, h, i));
    EXPECT_EQ(10, COMMON_PP_N_ARGS(a, b, c, d, e, f, g, h, i, j));
    EXPECT_EQ(11, COMMON_PP_N_ARGS(a, b, c, d, e, f, g, h, i, j, k));
    EXPECT_EQ(12, COMMON_PP_N_ARGS(a, b, c, d, e, f, g, h, i, j, k, l));
    EXPECT_EQ(13, COMMON_PP_N_ARGS(a, b, c, d, e, f, g, h, i, j, k, l, m));
    EXPECT_EQ(14, COMMON_PP_N_ARGS(a, b, c, d, e, f, g, h, i, j, k, l, m, n));
    EXPECT_EQ(15, COMMON_PP_N_ARGS(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o));
}

TEST(Preprocess, Varargs)
{
    EXPECT_EQ("a", COMMON_PP_FOR_EACH_ARGS(COMMON_PP_STRINGIZE, a));
    EXPECT_EQ("ab", COMMON_PP_FOR_EACH_ARGS(COMMON_PP_STRINGIZE, a, b));
    EXPECT_EQ("abc", COMMON_PP_FOR_EACH_ARGS(COMMON_PP_STRINGIZE, a, b, c));
    EXPECT_EQ("abcd", COMMON_PP_FOR_EACH_ARGS(COMMON_PP_STRINGIZE, a, b, c, d));
    EXPECT_EQ("abcde",
              COMMON_PP_FOR_EACH_ARGS(COMMON_PP_STRINGIZE, a, b, c, d, e));
    EXPECT_EQ("abcdef",
              COMMON_PP_FOR_EACH_ARGS(COMMON_PP_STRINGIZE, a, b, c, d, e, f));
    EXPECT_EQ("abcdefg",
              COMMON_PP_FOR_EACH_ARGS(COMMON_PP_STRINGIZE, a, b, c, d, e, f, g));
    EXPECT_EQ("abcdefgh",
              COMMON_PP_FOR_EACH_ARGS(
                  COMMON_PP_STRINGIZE, a, b, c, d, e, f, g, h));
    EXPECT_EQ("abcdefghi",
              COMMON_PP_FOR_EACH_ARGS(
                  COMMON_PP_STRINGIZE, a, b, c, d, e, f, g, h, i));
    EXPECT_EQ("abcdefghij",
              COMMON_PP_FOR_EACH_ARGS(
                  COMMON_PP_STRINGIZE, a, b, c, d, e, f, g, h, i, j));
    EXPECT_EQ("abcdefghijk",
              COMMON_PP_FOR_EACH_ARGS(COMMON_PP_STRINGIZE,
                  a, b, c, d, e, f, g, h, i, j, k));
    EXPECT_EQ("abcdefghijkl",
              COMMON_PP_FOR_EACH_ARGS(
                  COMMON_PP_STRINGIZE, a, b, c, d, e, f, g, h, i, j, k, l));
    EXPECT_EQ("abcdefghijklm",
              COMMON_PP_FOR_EACH_ARGS(
                  COMMON_PP_STRINGIZE, a, b, c, d, e, f, g, h, i, j, k, l, m));
    EXPECT_EQ("abcdefghijklmn",
              COMMON_PP_FOR_EACH_ARGS(
                  COMMON_PP_STRINGIZE,
                  a, b, c, d, e, f, g, h, i, j, k, l, m, n));
    EXPECT_EQ("abcdefghijklmno",
              COMMON_PP_FOR_EACH_ARGS(
                  COMMON_PP_STRINGIZE,
                  a, b, c, d, e, f, g, h, i, j, k, l, m, n, o));
}

#define DEFINE_METHOD(cmd, name) (cmd, name)

#define EXPAND_METHOD_(cmd, name) int name() { return cmd; }
#define EXPAND_METHOD(x) EXPAND_METHOD_ x

#define DEFINE_SERVICE(name, ...) \
    class name { \
    public: \
        COMMON_PP_FOR_EACH_ARGS(EXPAND_METHOD, __VA_ARGS__) \
    };

DEFINE_SERVICE(TestService,
    DEFINE_METHOD(1, Echo),
    DEFINE_METHOD(2, Inc)
)

TEST(Preprocess, VaForEach)
{
    TestService service;
    EXPECT_EQ(1, service.Echo());
    EXPECT_EQ(2, service.Inc());
}
