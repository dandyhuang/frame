/**
 * @file dynamic_library_test.cpp
 * @brief
 * @author chenzheng05@baidu.com
 * @date 2015-06-11
 */

#include "thirdparty/monitor_sdk_common/system/dynamic_library.h"
#include "thirdparty/gtest/gtest.h"

// namespace common {

TEST(DynamicLibrary, LoadAndUnload)
{
    DynamicLibrary dll;
    ASSERT_TRUE(dll.Load("libm.so"));
    ASSERT_TRUE(dll.Load("libm.so"));
    ASSERT_TRUE(dll.Unload());
    ASSERT_FALSE(dll.IsLoaded());
}

TEST(DynamicLibrary, GetSymbol)
{
    DynamicLibrary dll;
    ASSERT_TRUE(dll.Load("libm.so"));
    double (*log10)(double x);
    ASSERT_TRUE(dll.GetSymbol("log10", &log10));
    ASSERT_DOUBLE_EQ(log10(10.0), 1.0);
}

// } // namespace common
