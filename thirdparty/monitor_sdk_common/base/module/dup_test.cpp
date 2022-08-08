//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-28 12:05
//  @file:      dup_test.cpp
//  @author:    
//  @brief:     make duplicated module to test
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/module.h"
#include "thirdparty/gtest/gtest.h"


bool Success()
{
    return true;
}

TEST(Module, DupTest)
{
    // register 2 duplicate module, the 2nd shoud be failed and death
    using ::common::internal::ModuleManager;
    ModuleManager::RegisterModuleCtor(__FILE__, __LINE__, "FOO", Success);
    EXPECT_DEATH(ModuleManager::RegisterModuleCtor(__FILE__, __LINE__, "FOO", Success),
                 "Duplicated");
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}

