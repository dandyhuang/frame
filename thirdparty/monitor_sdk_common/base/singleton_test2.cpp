//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 16:32
//  @file:      singleton_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/singleton.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

// make a singleton class
class TestClass : public SingletonBase<TestClass>
{
private:
    friend class SingletonBase<TestClass>;
    TestClass() {}
public:
    int Test() const
    {
        return 1;
    }
};

TEST(Singleton, SingletonClass)
{
    TestClass& test = TestClass::Instance();
    EXPECT_EQ(1, test.Test());
}

class TestClass2 {};
TEST(Singleton, ClassSingleton)
{
    TestClass2& test = Singleton<TestClass2>::Instance();
    (void) test;
}

class TestClass3 : public SingletonBase<TestClass3>
{
    friend class SingletonBase<TestClass>;
};

TEST(Singleton, IsAlive)
{
    EXPECT_FALSE(TestClass3::IsAlive());
    TestClass3::Instance();
    EXPECT_TRUE(TestClass3::IsAlive());
}

} // end of namespace common
