//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-28 12:04
//  @file:      dependency_map_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/module/dependency_map.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(Dependency, Two)
{
    DependencyMap m;
    m["A"].insert("B");
    m["B"];
    std::vector<std::string> result;
    ASSERT_TRUE(TopologicalSort(m, &result));
    ASSERT_EQ("A", result[0]);
    ASSERT_EQ("B", result[1]);
}

TEST(Dependency, Three)
{
    DependencyMap m;
    m["A"].insert("B");
    m["B"].insert("C");
    m["C"];
    std::vector<std::string> result;
    ASSERT_TRUE(TopologicalSort(m, &result));
    ASSERT_EQ("A", result[0]);
    ASSERT_EQ("B", result[1]);
    ASSERT_EQ("C", result[2]);
}

} // end of namespace common

