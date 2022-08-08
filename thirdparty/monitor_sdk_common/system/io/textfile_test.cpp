//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:48
//  @file:      textfile_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/io/textfile.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(TextFile, LoadToString)
{
    ::std::string s;
    ASSERT_TRUE(io::textfile::LoadToString("test_unix.txt", &s));
    EXPECT_EQ(773U, s.length());

    ASSERT_TRUE(io::textfile::LoadToString("test_dos.txt", &s));
    EXPECT_EQ(794U, s.length());

    //ASSERT_TRUE(io::textfile::LoadToString(
    //        "/dev/shm/sysconfig/config-eth0", &s));

    ASSERT_TRUE(io::textfile::LoadToString("/dev/zero", &s));
    EXPECT_EQ(134217728U, s.length());

    ASSERT_TRUE(io::textfile::LoadToString("/proc/uptime", &s));
}

TEST(TextFile, UnixReadLinesToVector)
{
    ::std::vector< ::std::string> lines;
    ASSERT_TRUE(io::textfile::ReadLines("test_unix.txt", &lines));
    EXPECT_EQ(21U, lines.size());
    EXPECT_EQ("Long, long ago there lived a king.", lines[0]);
}

TEST(TextFile, DosReadLinesToVector)
{
    ::std::vector< ::std::string> lines;
    ASSERT_TRUE(io::textfile::ReadLines("test_dos.txt", &lines));
    EXPECT_EQ(21U, lines.size());
    EXPECT_EQ("Long, long ago there lived a king.", lines[0]);
}

TEST(TextFile, UnixReadLinesToDeque)
{
    ::std::deque< ::std::string> lines;
    ASSERT_TRUE(io::textfile::ReadLines("test_unix.txt", &lines));
    EXPECT_EQ(21U, lines.size());
    EXPECT_EQ("Long, long ago there lived a king.", lines[0]);
}

TEST(TextFile, DosReadLinesToDeque)
{
    ::std::deque< ::std::string> lines;
    ASSERT_TRUE(io::textfile::ReadLines("test_dos.txt", &lines));
    EXPECT_EQ(21U, lines.size());
    EXPECT_EQ("Long, long ago there lived a king.", lines[0]);
}

TEST(TextFile, UnixReadLinesToList)
{
    ::std::list< ::std::string> lines;
    ASSERT_TRUE(io::textfile::ReadLines("test_unix.txt", &lines));
    EXPECT_EQ(21U, lines.size());
    EXPECT_EQ("Long, long ago there lived a king.", lines.front());
}

TEST(TextFile, DosReadLinesToList)
{
    ::std::list< ::std::string> lines;
    ASSERT_TRUE(io::textfile::ReadLines("test_dos.txt", &lines));
    EXPECT_EQ(21U, lines.size());
    EXPECT_EQ("Long, long ago there lived a king.", lines.front());
}

} // end of namespace common
