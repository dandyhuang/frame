//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 10:17
//  @file:      io_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include <iostream>
#include <string>
#include <vector>

#include "thirdparty/gtest/gtest.h"
#include "thirdparty/monitor_sdk_common/base/compatible/stdlib.h"
#include "thirdparty/monitor_sdk_common/system/io/file.h"
#include "thirdparty/monitor_sdk_common/system/io/path.h"
#include "thirdparty/monitor_sdk_common/system/io/directory.h"

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

    string root_dir = "/root";
    ASSERT_TRUE(directory::IsReadable(tmp_dir));
    ASSERT_FALSE(directory::IsReadable(root_dir));
    ASSERT_TRUE(directory::IsWritable(tmp_dir));
    ASSERT_FALSE(directory::IsWritable(root_dir));

    string new_current = current_dir + "/" + tmp_dir;
    ASSERT_FALSE(directory::SetCurrentDir(root_dir));
    directory::SetCurrentDir(new_current);
    ASSERT_EQ(directory::GetCurrentDir(), new_current);
    directory::SetCurrentDir("..");
    ASSERT_EQ(directory::GetCurrentDir(), current_dir);

    directory::Delete(tmp_dir);
    ASSERT_FALSE(directory::Exists(tmp_dir));

    vector<string> files;
    cout << "Files under current dir:\n*****************\n";
    directory::GetFiles(current_dir, &files);
    for (size_t i = 0; i < files.size(); i++)
    {
        cout <<  files[i] << endl;
    }
    cout << "Dirs under current dir:\n*****************\n";
    directory::GetSubDirs(current_dir, &files);
    for (size_t i = 0; i < files.size(); i++)
    {
        cout << files[i] << endl;
    }
    cout << "All files under current dir:\n*****************\n";
    directory::GetAllFiles(current_dir, &files);
    for (size_t i = 0; i < files.size(); i++)
    {
        cout <<  files[i] << endl;
    }
    cout << "All dirs under current dir:\n*****************\n";
    directory::GetAllSubDirs(current_dir, &files);
    for (size_t i = 0; i < files.size(); i++)
    {
        cout << files[i] << endl;
    }
}

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
    EXPECT_LT(llabs(ft.create_time - now), 2);
    EXPECT_LT(llabs(ft.access_time - now), 2);
    EXPECT_LT(llabs(ft.modify_time - now), 2);

    sleep(2);
    file::Touch(current_file);
    now = time(NULL);
    ASSERT_TRUE(file::GetTime(current_file, &ft));
    EXPECT_LT(llabs(ft.create_time - now), 2);
    EXPECT_LT(llabs(ft.access_time - now), 2);
    EXPECT_LT(llabs(ft.modify_time - now), 2);

    file::Delete(current_file);
}

TEST(PathTest, Ops)
{
    string filepath = "io_test";
    string fullpath = directory::GetCurrentDir() + "/" + filepath;
    ASSERT_EQ(path::GetFullPath(filepath), fullpath);

    ASSERT_EQ(path::GetBaseName(filepath),  filepath);
    ASSERT_EQ(path::GetExtension(filepath), "");
    ASSERT_EQ(path::GetDirectory(filepath), "");

    filepath = "/";
    ASSERT_EQ(path::GetBaseName(filepath),  "");
    ASSERT_EQ(path::GetExtension(filepath), "");
    ASSERT_EQ(path::GetDirectory(filepath), "/");

    filepath = "////xxx//xx.x";
    ASSERT_EQ(path::GetBaseName(filepath),  "xx.x");
    ASSERT_EQ(path::GetExtension(filepath), ".x");
    ASSERT_EQ(path::GetDirectory(filepath), "////xxx//");
}

TEST(DirectoryIteratorTest, Ops)
{
    string str = ".";
    io::DirectoryIterator iter;

    if (!iter.Open(str))
    {
        cout << "open failed.\n";
        return;
    }

    cout << "All dir and files:\n==============\n";
    while (!iter.IsEnd())
    {
        cout << iter.Name() << endl;
        cout << "FullName: " << iter.FullPath() << endl;
        iter.Next();
    }
    iter.Close();

    cout << "All files:\n==============\n";
    if (!iter.Open(str, DirectoryIterator::FILE))
    {
        cout << "open failed.\n";
        return;
    }
    while (!iter.IsEnd())
    {
        cout << iter.Name() << endl;
        cout << "FullName: " << iter.FullPath() << endl;
        iter.Next();
    }
    iter.Close();
    cout << "All cpps:\n==============\n";

    if (!iter.Open(str, DirectoryIterator::FILE, "*.cpp"))
    {
        cout << "open failed.\n";
        return;
    }
    while (!iter.IsEnd())
    {
        cout << iter.Name() << endl;
        cout << "FullName: " << iter.FullPath() << endl;
        iter.Next();
    }
    iter.Close();
    //directory::RecursiveDelete("test");
}


