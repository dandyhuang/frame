//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 15:13
//  @file:      condition_variable_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/condition_variable.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(ConditionVariable, Init)
{
    ConditionVariable cond;
}

TEST(ConditionVariable, Wait)
{
    ConditionVariable cond;
    cond.Signal();
}

TEST(ConditionVariable, Release)
{
}

} // end of namespace common
