//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 15:55
//  @file:      format_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/string/format.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

static unsigned long lu = 99;

TEST(StringFormat, StringFormat)
{
    ASSERT_EQ("sx100hehe,99\n", StringFormat("sx%d%s%lu\n", 100, "hehe,", lu));
}

TEST(StringFormat, StringFormatTo)
{
    ::std::string str;
    size_t length = StringFormatTo(&str, "sx%d%s%lu\n", 100, "hehe,", lu);
    ASSERT_EQ("sx100hehe,99\n", str);
    ASSERT_EQ(str.length(), length);
}

TEST(StringFormat, StringFormatAppend)
{
    ::std::string str = "hello";
    size_t org_length = str.length();
    size_t append_length = StringFormatAppend(&str, "sx%d%s%lu\n", 100, "hehe,", lu);
    ASSERT_EQ("hellosx100hehe,99\n", str);
    ASSERT_EQ(str.length() - org_length, append_length);
}

TEST(StringFormat, LongString)
{
    ::std::string a(1024, 'A');
    ::std::string b(1024, 'A');
    ASSERT_EQ(a + b, StringFormat("%s%s", a.c_str(), b.c_str()));
}

} // end of namespace common
