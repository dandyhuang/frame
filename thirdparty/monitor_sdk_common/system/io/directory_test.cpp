//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 10:05
//  @file:      directory_test.cpp
//  @author:
//  @brief:
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/io/directory.h"

#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>

#include "thirdparty/monitor_sdk_common/base/compatible/stdlib.h"
#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/system/io/file.h"
#include "thirdparty/monitor_sdk_common/system/io/path.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

using namespace std;
using namespace common::io;

TEST(DirectoryTest, Ops)
{
    string current_dir = directory::GetCurrentDir();
    ASSERT_EQ(directory::ToUnixFormat(directory::ToWindowsFormat(current_dir)), current_dir);

    string tmp_dir = "_test_dir_";
    ASSERT_FALSE(directory::Exists(tmp_dir));
    directory::Create(tmp_dir);
    ASSERT_TRUE(directory::Exists(tmp_dir));
    ASSERT_TRUE(directory::IsReadable(tmp_dir));
    ASSERT_TRUE(directory::IsWritable(tmp_dir));

    string bad_dir = "/non-existed-dir";
    ASSERT_FALSE(directory::IsReadable(bad_dir));
    ASSERT_FALSE(directory::IsWritable(bad_dir));

    string new_current = current_dir + "/" + tmp_dir;
    ASSERT_FALSE(directory::SetCurrentDir(bad_dir));
    directory::SetCurrentDir(new_current);
    ASSERT_EQ(directory::GetCurrentDir(), new_current);
    directory::SetCurrentDir("..");
    ASSERT_EQ(directory::GetCurrentDir(), current_dir);

    ASSERT_TRUE(directory::Delete(tmp_dir));
    ASSERT_FALSE(directory::Exists(tmp_dir));

    vector<string> files;
    LOG(INFO) << "Files under current dir:\n*****************";
    directory::GetFiles(current_dir, &files);
    for (size_t i = 0; i < files.size(); i++)
    {
        LOG(INFO) <<  files[i];
    }
    LOG(INFO) << "Dirs under current dir:\n*****************";
    directory::GetSubDirs(current_dir, &files);
    for (size_t i = 0; i < files.size(); i++)
    {
        LOG(INFO) << files[i];
    }
    LOG(INFO) << "All files under current dir:\n*****************";
    directory::GetAllFiles(current_dir, &files);
    for (size_t i = 0; i < files.size(); i++)
    {
        LOG(INFO) <<  files[i];
    }
    LOG(INFO) << "All dirs under current dir:\n*****************";
    directory::GetAllSubDirs(current_dir, &files);
    for (size_t i = 0; i < files.size(); i++)
    {
        LOG(INFO) << files[i];
    }

    LOG(INFO) << "All under current dir:\n*****************";
    directory::GetAll(current_dir, &files);
    for (size_t i = 0; i < files.size(); i++)
    {
        LOG(INFO) << files[i];
    }
}

TEST(DirectoryIteratorTest, Ops)
{
    string str = ".";
    DirectoryIterator iter;

    if (!iter.Open(str))
    {
        LOG(INFO) << "open failed.";
        return;
    }

    LOG(INFO) << "All dir and files:\n==============";
    while (!iter.IsEnd())
    {
        LOG(INFO) << iter.Name() << endl;
        LOG(INFO) << "FullName: " << iter.FullPath() << endl;
        iter.Next();
    }
    iter.Close();

    LOG(INFO) << "All files:\n==============";
    if (!iter.Open(str, DirectoryIterator::FILE))
    {
        LOG(INFO) << "open failed.";
        return;
    }
    while (!iter.IsEnd())
    {
        LOG(INFO) << iter.Name() << endl;
        LOG(INFO) << "FullName: " << iter.FullPath();
        iter.Next();
    }
    iter.Close();
    LOG(INFO) << "All cpps:\n==============";

    if (!iter.Open(str, DirectoryIterator::FILE, "*.cpp"))
    {
        LOG(INFO) << "open failed.";
        return;
    }
    while (!iter.IsEnd())
    {
        LOG(INFO) << iter.Name() << endl;
        LOG(INFO) << "FullName: " << iter.FullPath() << endl;
        iter.Next();
    }
    iter.Close();
}

TEST(DirectoryIteratorTest, FilterIterator)
{
    string str = ".";
    DirectoryIterator iter(str.c_str(), DirectoryIterator::FILE, "*.cpp");
    while (!iter.IsEnd())
    {
        LOG(INFO) << iter.Name() << endl;
        LOG(INFO) << "FullName: " << iter.FullPath() << endl;
        iter.Next();
    }
    iter.Close();
}

TEST(DirectoryTest, Time)
{
    string tmp_dir = "_test_dir_";
    ASSERT_FALSE(directory::Exists(tmp_dir));
    directory::Create(tmp_dir);
    ASSERT_TRUE(directory::Exists(tmp_dir));

    time_t now = time(NULL);
    time_t create_time = directory::GetCreateTime(tmp_dir);
    time_t access_time = directory::GetAccessTime(tmp_dir);
    time_t modify_time = directory::GetLastModifyTime(tmp_dir);
    EXPECT_LT(labs(create_time - now), 5);
    EXPECT_LT(labs(access_time - now), 5);
    EXPECT_LT(labs(modify_time - now), 5);

    directory::Delete(tmp_dir);
    ASSERT_FALSE(directory::Exists(tmp_dir));

    ASSERT_EQ(-1, directory::GetCreateTime("/bin/ls"));
    ASSERT_EQ(-1, directory::GetAccessTime("/bin/ls"));
    ASSERT_EQ(-1, directory::GetLastModifyTime("/bin/ls"));
}

TEST(DirectoryTest, RecursiveDelete)
{
    string tmp_dir = "_test_dir_";
    ASSERT_FALSE(directory::Exists(tmp_dir));
    directory::Create(tmp_dir);
    ASSERT_TRUE(directory::Exists(tmp_dir));
    string sub_dir =  tmp_dir + "/test1";
    directory::Create(sub_dir);
    sub_dir =  tmp_dir + "/test2";
    directory::Create(sub_dir);
    ASSERT_TRUE(directory::RecursiveDelete(tmp_dir));
}

TEST(DirectoryTest, RecursiveCreate)
{
    string tmp_dir = "_test_create_dir_";
    ASSERT_FALSE(directory::Exists(tmp_dir));
    string sub_dir =  tmp_dir + "/test1/";
    ASSERT_TRUE(directory::RecursiveCreate(sub_dir));
    ASSERT_TRUE(directory::Exists(sub_dir));
    ASSERT_TRUE(directory::RecursiveDelete(sub_dir));
    ASSERT_FALSE(directory::Exists(sub_dir));

    string sub_dir_1 = tmp_dir + "/test2";
    directory::Create(sub_dir_1, 0755);
    ASSERT_TRUE(directory::Exists(sub_dir_1));
    chmod(tmp_dir.c_str(), 0555);

    // 测试递归创建失败后，不会删除tmp_dir目录中其它内容
    ASSERT_FALSE(directory::RecursiveCreate(sub_dir));
    ASSERT_TRUE(directory::Exists(sub_dir_1));
    chmod(tmp_dir.c_str(), 0777);
    // 创建一个存在的，会失败
    ASSERT_FALSE(directory::RecursiveCreate(sub_dir_1));
}
