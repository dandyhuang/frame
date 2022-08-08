//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-28 11:53
//  @file:      cycle_test.cpp
//  @author:    
//  @brief:     test cycle dependancy
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/module.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

int g_argc;
char** g_argv;

TEST(Module, CycleTest)
{
    EXPECT_DEATH(InitAllModules(&g_argc, &g_argv), "mod1");
}

} // end of namespace common


int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    common::g_argc = argc;
    common::g_argv = argv;
    return RUN_ALL_TESTS();
}
