//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:43
//  @file:      path_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/io/path.h"
#include <iostream>
#include <string>
#include <vector>
#include "thirdparty/monitor_sdk_common/system/io/directory.h"
#include "thirdparty/gtest/gtest.h"

using namespace std;
using namespace common::io;

TEST(Path, Ops)
{
    string filepath = "io_test";
    string fullpath = directory::GetCurrentDir() + "/" + filepath;
    EXPECT_EQ(fullpath, path::GetFullPath(filepath));

    EXPECT_EQ(filepath, path::GetBaseName(filepath));
    EXPECT_EQ("", path::GetExtension(filepath));
    EXPECT_EQ("", path::GetDirectory(filepath));

    filepath = "/";
    EXPECT_EQ("", path::GetBaseName(filepath));
    EXPECT_EQ("", path::GetExtension(filepath));
    EXPECT_EQ("/", path::GetDirectory(filepath));

    filepath = "////xxx//xx.x";
    EXPECT_EQ("xx.x", path::GetBaseName(filepath));
    EXPECT_EQ(".x", path::GetExtension(filepath));
    EXPECT_EQ("////xxx//", path::GetDirectory(filepath));
}

TEST(Path, Normalize)
{
    EXPECT_EQ(".", path::NormalizePath(""));
    EXPECT_EQ("/", path::NormalizePath("///"));
    EXPECT_EQ("//", path::NormalizePath("//"));
    EXPECT_EQ("//abc", path::NormalizePath("//abc"));
    EXPECT_EQ("/a/b/c", path::NormalizePath("///a//b/c//"));
    EXPECT_EQ("../..", path::NormalizePath("../../"));
    EXPECT_EQ("../../abc", path::NormalizePath("../../abc"));
    EXPECT_EQ("/abc", path::NormalizePath("/data/../abc"));
    EXPECT_EQ("/", path::NormalizePath("/abc/../../../"));
}

TEST(Path, Join)
{
    EXPECT_EQ("a/b", path::Join("a", "b"));
    EXPECT_EQ("a/b/c", path::Join("a", "b", "c"));
    EXPECT_EQ("a/b/c/d", path::Join("a", "b", "c", "d"));
    EXPECT_EQ("a/b/c/d/e", path::Join("a", "b", "c", "d", "e"));
    EXPECT_EQ("a/b/c/d/e/f", path::Join("a", "b/", "c", "d/", "e", "f"));
    EXPECT_EQ("abc/def/", path::Join("abc", "def/"));
    EXPECT_EQ("/abc/def/", path::Join("/abc", "def/"));
    EXPECT_EQ("/abc/def/", path::Join("/abc/", "def/"));
    EXPECT_EQ("/def", path::Join("/abc/", "/def"));
}

TEST(Path, GetFullPath)
{
    std::string path = "/xfs/szsk-processing/home/hotwheels/taskmanager";
    EXPECT_EQ(path, path::GetFullPath(path));
    EXPECT_EQ("/xfs/d", path::GetFullPath("/xfs/abc/../d"));
    EXPECT_EQ(directory::GetCurrentDir() + "/d", path::GetFullPath("abc/../d"));
}
