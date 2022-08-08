// Copyright (C) 2014, Vivo Inc.
// Author: Zhang ShuXin (zhangshuxin@baidu.com)
//
// Description:

#include "thirdparty/monitor_sdk_common/storage/file/hdfs_file_stream.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(HDFSFileStream, DISABLED_Read_Write) {
    int64_t buffer_size = 100;
    HDFSFileStream file_writer("default", 0, buffer_size);

    char* buffer = new char[buffer_size];
    memset(buffer, 'a', buffer_size);

    std::string file_path = "./hdfs_file_stream_ut_1.dat";

    // write
    EXPECT_TRUE(file_writer.Open(file_path, FILE_WRITE));

    // write round 1
    EXPECT_EQ(buffer_size, file_writer.Write(buffer, buffer_size));

    EXPECT_TRUE(file_writer.Flush());
    EXPECT_TRUE(file_writer.Flush());

    EXPECT_EQ(buffer_size, file_writer.Tell());
    // EXPECT_EQ(buffer_size, file_writer.GetSize(file_path));

    // write round 2
    memset(buffer, 'b', buffer_size);
    buffer[10] = 'B';
    EXPECT_EQ(buffer_size, file_writer.Write(buffer, buffer_size));

    // write round 3
    memset(buffer, 'c', buffer_size);
    buffer[buffer_size - 2] = '\n';
    EXPECT_EQ(buffer_size, file_writer.Write(buffer, buffer_size));
    EXPECT_TRUE(file_writer.Flush());
    // EXPECT_EQ(buffer_size * 3, file_writer.GetSize(file_path));

    EXPECT_EQ(-1, file_writer.Seek(-1, SEEK_END));

    EXPECT_TRUE(file_writer.Close());

    EXPECT_FALSE(file_writer.Flush());
    EXPECT_EQ(-1, file_writer.Tell());
    EXPECT_EQ(-1, file_writer.Seek(-1, SEEK_END));

    // read
    HDFSFileStream file_reader("default", 0, buffer_size);
    EXPECT_TRUE(file_reader.Open(file_path, FILE_READ));
    EXPECT_EQ(buffer_size, file_reader.Read(buffer, buffer_size));
    for (int i = 0; i < buffer_size; ++i) {
        CHECK_EQ('a', buffer[i]) << "i=" << i;
    }

    EXPECT_EQ(9, file_reader.Read(buffer, 9));
    for (int i = 0; i < 9; ++i) {
        CHECK_EQ('b', buffer[i]) << "i=" << i;
    }

    EXPECT_EQ(110, file_reader.Seek(110, SEEK_SET));
    EXPECT_EQ(110, file_reader.Tell());
    EXPECT_EQ(1, file_reader.Read(buffer, 1));
    EXPECT_EQ('B', buffer[0]);

    EXPECT_EQ(buffer_size, file_reader.Read(buffer, buffer_size));
    for (int i = 0; i < 89; ++i) {
        CHECK_EQ('b', buffer[i]) << "i=" << i;
    }
    for (int i = 90; i < 100; ++i) {
        CHECK_EQ('c', buffer[i]) << "i=" << i;
    }

    EXPECT_EQ(88, file_reader.ReadLine(buffer, buffer_size));

    EXPECT_TRUE(file_reader.Close());
    EXPECT_EQ(buffer_size * 3, file_reader.GetSize(file_path));
}

TEST(HDFSFileStream, DISABLED_Mkdir_Delete) {
    std::string file_path = "./HSFSFileStreamTest_ut";
    HDFSFileStream file_writer("default", 0);
    file_writer.Remove(file_path);
    EXPECT_FALSE(file_writer.IsExist(file_path));
    EXPECT_EQ(0, file_writer.CreateDir(file_path));
    EXPECT_TRUE(file_writer.IsExist(file_path));
    EXPECT_TRUE(file_writer.IsDir(file_path));
    EXPECT_EQ(0, file_writer.Remove(file_path));
    EXPECT_FALSE(file_writer.IsExist(file_path));

    file_writer.Open(file_path, FILE_WRITE);
    EXPECT_TRUE(file_writer.IsExist(file_path));
    EXPECT_FALSE(file_writer.IsDir(file_path));
    EXPECT_EQ(0, file_writer.Remove(file_path));
    EXPECT_FALSE(file_writer.IsExist(file_path));
    EXPECT_FALSE(file_writer.IsDir(file_path));
    file_writer.Close();
}



} // end of namespace common

