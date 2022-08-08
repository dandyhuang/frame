// Copyright 2011.
#include <iostream>
#include "thirdparty/monitor_sdk_common/collection/prefix_map.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(PrefixMap, FindMatchLength)
{
    PrefixMap<int> map;
    map.InsertInclusive("test", 10);
    int result;
    EXPECT_EQ(4, map.FindMatchLength("test1", &result));
    EXPECT_EQ(10, result);

    result = 0;
    EXPECT_EQ(4, map.FindMatchLength("test", &result));
    EXPECT_EQ(10, result);

    result = 0;
    EXPECT_EQ(-1, map.FindMatchLength("error", &result));
    EXPECT_EQ(0, result);
}

TEST(PrefixMap, Remove)
{
    PrefixMap<std::string*, PrefixMap_PointerCleaner> map;
    std::string* str = new std::string("test");
    map.InsertInclusive("test", str);
    EXPECT_TRUE(map.Remove("test"));
    EXPECT_FALSE(map.Remove("test"));
}

} // namespace common
