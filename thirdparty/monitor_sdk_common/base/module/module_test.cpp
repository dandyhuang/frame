//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-28 12:12
//  @file:      module_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/module.h"
#include "thirdparty/monitor_sdk_common/base/module/module_a.h"
#include "thirdparty/monitor_sdk_common/base/module/module_b.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(Module, Test)
{
    EXPECT_EQ(1, GetA());
    EXPECT_EQ(2, GetB());
}

} // end of namespace common

int main(int argc, char** argv)
{
    ::common::InitAllModulesAndTest(&argc, &argv);
    ::common::InitAllModules(&argc, &argv); // Test Init twice
    return RUN_ALL_TESTS();
}
