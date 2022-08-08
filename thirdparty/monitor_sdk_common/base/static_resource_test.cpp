// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 09/30/11
// Description: static resource test

#include "thirdparty/monitor_sdk_common/base/static_resource.h"
#include "thirdparty/monitor_sdk_common/base/static_resource_test_data.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

static const char RESOURCE_test[] = "hello";

TEST(StaticResource, Test)
{
    StringPiece resource = COMMON_STATIC_RESOURCE(test);
    EXPECT_STREQ(RESOURCE_test, resource.data());
}

TEST(StaticResource, Package)
{
    const StaticResourcePackage& package =
        COMMON_STATIC_RESOURCE_PACKAGE(common_base_static_resource_test_data);
    StringPiece data;
    ASSERT_TRUE(package.Find("static_resource.testdata", &data));
    EXPECT_EQ("hello, blade!\n", data);
    ASSERT_FALSE(package.Find("wtf", &data));
}

} // namespace common
