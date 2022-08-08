//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 10:15
//  @file:      file_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/io/file.h"

#include <iostream>
#include <string>
#include <vector>

#include "thirdparty/gtest/gtest.h"
#include "thirdparty/monitor_sdk_common/base/compatible/stdlib.h"

using namespace std;
using namespace common::io;

TEST(FileTest, Ops)
{
    string original_file = "/bin/ls";
    string current_file = "/tmp/ls";
    file::Copy(original_file, current_file);
    ASSERT_TRUE(file::IsRegular(current_file));
    string bak_file = "/tmp/ls.bak";
    file::Copy(current_file, bak_file);
    ASSERT_TRUE(file::IsRegular(bak_file));
    file::Delete(bak_file);
    ASSERT_FALSE(file::IsRegular(bak_file));

    EXPECT_TRUE(file::Touch(bak_file));
    EXPECT_TRUE(file::Copy(current_file, bak_file, true));
    long long size1, size2;
    EXPECT_TRUE(file::GetSize(current_file, &size1));
    EXPECT_TRUE(file::GetSize(bak_file, &size2));
    ASSERT_EQ(size1, size2);
    file::Delete(bak_file);
    ASSERT_FALSE(file::IsRegular(bak_file));

    file::Rename(current_file, bak_file);
    ASSERT_TRUE(file::Exists(bak_file));
    ASSERT_FALSE(file::Exists(current_file));

    file::Rename(bak_file, current_file);
    ASSERT_TRUE(file::IsReadable(current_file));
    ASSERT_TRUE(file::IsWritable(current_file));
}

TEST(FileTest, Time)
{
    string original_file = "/bin/ls";
    string current_file = "/tmp/ls";
    file::Delete(current_file);
    file::Copy(original_file, current_file);
    time_t now = time(NULL);
    file::FileTime ft;
    ASSERT_TRUE(file::GetTime(current_file, &ft));
    EXPECT_LT(labs(ft.create_time - now), 5);
    EXPECT_LT(labs(ft.access_time - now), 5);
    EXPECT_LT(labs(ft.modify_time - now), 5);

    sleep(2);
    file::Touch(current_file);
    now = time(NULL);
    ASSERT_TRUE(file::GetTime(current_file, &ft));
    EXPECT_LT(labs(ft.create_time - now), 5);
    EXPECT_LT(labs(ft.access_time - now), 5);
    EXPECT_LT(labs(ft.modify_time - now), 5);

    ft.create_time = ft.create_time - 100;
    ft.access_time = ft.access_time - 100;
    ft.modify_time = ft.modify_time - 100;
    ASSERT_TRUE(file::SetTime(current_file, ft));
    ASSERT_EQ(ft.access_time, file::GetAccessTime(current_file));
    // Create time will not change.
    ASSERT_EQ(ft.create_time + 100, file::GetCreateTime(current_file));
    ASSERT_EQ(ft.modify_time, file::GetLastModifyTime(current_file));

    file::Delete(current_file);
}

TEST(FileTest, ReadFile)
{
    ::std::string str_content;
    ASSERT_TRUE(file::ReadAll("test_unix.txt", &str_content));
    ::std::vector<char> vec_content;
    ASSERT_TRUE(file::ReadAll("test_unix.txt", &vec_content));
    size_t buffer_size = 1024 * 8;
    char* buffer = new char[buffer_size];
    size_t read_size = 0;
    ASSERT_TRUE(file::ReadAll("test_unix.txt", buffer, buffer_size, &read_size));
    ASSERT_EQ(str_content.size(), vec_content.size());
    ASSERT_EQ(str_content.size(), read_size);
    delete []buffer;
}
