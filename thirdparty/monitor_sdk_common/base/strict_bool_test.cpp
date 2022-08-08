//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:55
//  @file:      strict_bool_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/strict_bool.h"
#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/base/scoped_refptr.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(StrictBool, CompileTest)
{
    strict_bool b;
    strict_bool b2(true);
    strict_bool b3(false);
    strict_bool b4 = true;
    strict_bool b5 = false;
    bool bb = b;
    bb = true;
    EXPECT_FALSE(b == b2);
    EXPECT_TRUE(b != b2);

    EXPECT_FALSE(b == true);
    EXPECT_TRUE(b == false);
    EXPECT_TRUE(b != true);
    EXPECT_FALSE(b != false);

    b = true;
    EXPECT_TRUE(true == b);
    EXPECT_FALSE(false == b);

    b = false;
    EXPECT_TRUE(false == b);
    EXPECT_FALSE(true == b);

    EXPECT_TRUE(true != b);
    EXPECT_FALSE(false != b);

    if (b)
        EXPECT_TRUE((bool)b);
    else
        EXPECT_FALSE((bool)b);
}

strict_bool Return()
{
    strict_bool b;
    return true;
    return false;
    return b;
}

TEST(StrictBool, Return)
{
    strict_bool b = Return();
}

// all following code should not compile
TEST(StrictBool, CtorNoCompileTest)
{
#if 0
    strict_bool b0(0);
    strict_bool b1(1);
#endif
}

TEST(StrictBool, InitNoCompileTest)
{
#if 0
    strict_bool b3 = 0;
    strict_bool b4 = 1;
#endif
}

TEST(StrictBool, AssignNoCompileTest)
{
#if 0
    strict_bool b;
    b = 1;
    b = 0;
    int a = b;
#endif
}

TEST(StrictBool, CompareNoCompileTest)
{
#if 0
    strict_bool b;
    b == 1;
    b != 1;
    1 == b;
    1 != b;
#endif
}

TEST(StrictBool, CompareWithZeroNoCompileTest)
{
#if 0
    strict_bool b;
    b == 0;
    b != 0;
    0 == b;
    0 != b;
#endif
}

class RpcChannel;

class Xxx : public RefCountedBase<Xxx> {};

TEST(StrictBool, Overloading)
{
    RpcChannel* channel = NULL;
    CHECK(channel == NULL) << "Channel found.";
    CHECK(NULL == channel) << "Channel found.";
    scoped_refptr<Xxx> p;
    CHECK(p == NULL);
    CHECK(NULL == p);
    scoped_ptr<int> pi;
    CHECK(pi == NULL);
}

} // end of namespace common
