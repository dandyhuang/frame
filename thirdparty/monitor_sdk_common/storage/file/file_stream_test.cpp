// Copyright (C) 2012, Vivo Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:

#include "thirdparty/monitor_sdk_common/storage/file/file_stream.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

#include "thirdparty/monitor_sdk_common/storage/file/file_types.h"

namespace common {

const ::std::string file_path_read = "./testdata/testdata.dat";
const ::std::string file_path_write = "./testdata/testdata_copy.dat";
const int64_t test_data_size = 18;

class FileStreamTest : public ::testing::Test {
public:
    FileStreamTest() {
        EXPECT_TRUE(m_file_reader.Open(file_path_read, FILE_READ));
        EXPECT_TRUE(m_file_writer.Open(file_path_write, FILE_WRITE));
    }
    ~FileStreamTest() {
        EXPECT_TRUE(m_file_reader.Close());
        EXPECT_TRUE(m_file_writer.Close());
    }

protected:
    FileStream m_file_reader;
    FileStream m_file_writer;
};

TEST_F(FileStreamTest, ReadData) {
    int64_t buffer_size = 100;
    char buffer[100] = {'\0'};
    EXPECT_EQ(test_data_size, m_file_reader.Read(buffer, buffer_size - 1));
    LOG(INFO) << "buffer content is: " << buffer;
}

TEST_F(FileStreamTest, WriteData) {
    int64_t buffer_size = 100;
    char buffer[100] = {'\0'};
    EXPECT_EQ(test_data_size, m_file_reader.Read(buffer, test_data_size));
    LOG(INFO) << "buffer content is: " << buffer;

    EXPECT_EQ(test_data_size, m_file_writer.Write(buffer, test_data_size));
    EXPECT_TRUE(m_file_writer.Flush());

    FileStream temp_reader;
    EXPECT_TRUE(temp_reader.Open(file_path_write, FILE_READ));
    char buffer_2[100];
    EXPECT_EQ(test_data_size, temp_reader.Read(buffer_2, buffer_size - 1));
    LOG(INFO) << "buffer2 content is: " << buffer_2;
}

TEST_F(FileStreamTest, Mkdir_Delete) {
    std::string file_path = "./FileStreamTest_ut";
    m_file_reader.Remove(file_path);
    EXPECT_FALSE(m_file_reader.IsExist(file_path));
    EXPECT_EQ(0, m_file_reader.CreateDir(file_path));
    EXPECT_TRUE(m_file_reader.IsExist(file_path));
    EXPECT_TRUE(m_file_reader.IsDir(file_path));
    EXPECT_EQ(0, m_file_reader.Remove(file_path));
    EXPECT_FALSE(m_file_reader.IsExist(file_path));

    FileStream file;
    file.Open(file_path, FILE_WRITE);
    EXPECT_TRUE(m_file_reader.IsExist(file_path));
    EXPECT_FALSE(m_file_reader.IsDir(file_path));
    EXPECT_EQ(0, m_file_reader.Remove(file_path));
    EXPECT_FALSE(m_file_reader.IsExist(file_path));
    EXPECT_FALSE(m_file_reader.IsDir(file_path));
}

} // end of namespace common

