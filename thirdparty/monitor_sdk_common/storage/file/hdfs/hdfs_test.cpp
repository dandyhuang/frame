// Copyright (c) 2014, Vivo Inc.
// All rights reserved.
//
// Author: rabbitliu <liubin18@baidu.com>
// Created: 01/12/14
// Description:

#include <string.h>
#include <string>

#include "thirdparty/monitor_sdk_common/storage/file/hdfs/hdfs_client.h"
#include "thirdparty/monitor_sdk_common/storage/file/hdfs/hdfs_def.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gmock/gmock.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

using ::testing::Return;
using ::testing::_;

const std::string g_test_contents = "hi, i am rabbitliu!";
const std::string read_file_path = "./testdata/data.txt";
const std::string write_file_path = "./testdata/data_copy.txt";
const std::string list_dir_path = "./testdata";

/// blade test不能运行该测试，所以DISABLED掉，因为需要设置一些hadoop的jar到环境变量的CLASSPATH里，
/// 因此运行测试,请运行./run_test.sh，这个才是真正的测试，下面那个是为了覆盖率Mock了HDFS的实现
TEST(HDFSClient, DISABLED_ReadTest)
{
    // 默认的配置得到一个HDFSClient，也即从LocalFileSystem
    HDFSClient* client = HDFSClient::GetHDFSClient();
    CHECK_NOTNULL(client);

    HDFSFile* read_file = client->Open(read_file_path, file::OpenModel::kRead);
    CHECK_NOTNULL(read_file);

    const size_t kSize = 100;
    char buffer[kSize] = {'\0'};
    size_t read_length = read_file->Read(buffer, kSize);
    ASSERT_EQ(read_length, g_test_contents.size());
    ASSERT_EQ(g_test_contents, std::string(buffer, read_length));
    LOG(INFO) << "buffer contents: " << std::string(buffer, read_length);

    read_file->Close();
    delete read_file;
    HDFSClient::ReleaseHDFSClient(client);
}

TEST(HDFSClient, DISABLED_WriteTest)
{
    // 默认的配置得到一个HDFSClient，也即从LocalFileSystem
    HDFSClient* client = HDFSClient::GetHDFSClient();
    CHECK_NOTNULL(client);
    // 以读写方式打开文件
    HDFSFile* write_file = client->Open(write_file_path, file::OpenModel::kWrite);
    CHECK_NOTNULL(write_file);

    const char* write_buffer = g_test_contents.c_str();
    size_t write_size = g_test_contents.size();
    size_t write_length = write_file->Write(write_buffer, write_size);
    ASSERT_EQ(write_length, write_size);

    write_file->Close();
    write_file = client->Open(write_file_path, file::OpenModel::kRead);
    const size_t kSize = 100;
    char buffer[kSize] = {'\0'};
    size_t read_length = write_file->Read(buffer, kSize);
    ASSERT_EQ(read_length, g_test_contents.size());
    ASSERT_EQ(g_test_contents, std::string(buffer, read_length));
    LOG(INFO) << "buffer contents: " << std::string(buffer, read_length);

    write_file->Close();

    std::string new_write_file_path = write_file_path + "_new";
    ASSERT_EQ(0, client->Rename(write_file_path, new_write_file_path));
    write_file = client->Open(new_write_file_path, file::OpenModel::kRead);
    memset(buffer, '\0', kSize);
    read_length = write_file->Read(buffer, kSize);
    ASSERT_EQ(read_length, g_test_contents.size());
    ASSERT_EQ(g_test_contents, std::string(buffer, read_length));
    LOG(INFO) << "buffer contents: " << std::string(buffer, read_length);

    delete write_file;
    HDFSClient::ReleaseHDFSClient(client);
}

TEST(HDFSClient, DISABLED_ListTest)
{
    // 默认的配置得到一个HDFSClient，也即从LocalFileSystem
    HDFSClient* client = HDFSClient::GetHDFSClient();
    CHECK_NOTNULL(client);
    std::vector<file::AttrsInfo> attrs;
    ASSERT_TRUE(client->List(list_dir_path, &attrs));
    for (size_t i = 0; i < attrs.size(); ++i) {
        LOG(ERROR) << "attrs[" << i << "].file_name: " << attrs[i].file_name;
    }
    HDFSClient::ReleaseHDFSClient(client);
}

/// 以下测试使用Mock，因此不需要CLASSPATH环境变量就可以执行，所以不用DISTABLE，直接blade test即可
/// 这个是为了测试覆盖率
class MockHDFSClientTest : public ::testing::Test {
public:
    MockHDFSClientTest()
    {
        m_mock_any = 0;
        m_mock_fs = reinterpret_cast<hdfsFS>(&m_mock_any);
        m_mock_file = reinterpret_cast<hdfsFile>(&m_mock_any);

        HDFSClient::SetMockModel();
        m_mock_file_impl = HDFSClient::GetMockImpl();
        CHECK_NOTNULL(m_mock_file_impl);
    }
    ~MockHDFSClientTest() {
    }

    void SetUp()
    {
        EXPECT_CALL(*m_mock_file_impl, hdfsConnect(_, _)).WillRepeatedly(Return(m_mock_fs));
        EXPECT_CALL(*m_mock_file_impl, hdfsDisconnect(_)).WillRepeatedly(Return(0));
        EXPECT_CALL(*m_mock_file_impl, hdfsOpenFile(_, _, _, _, _, _)).
            WillRepeatedly(Return(m_mock_file));
        EXPECT_CALL(*m_mock_file_impl, hdfsCloseFile(_, _)).WillRepeatedly(Return(0));
        EXPECT_CALL(*m_mock_file_impl, hdfsExists(_, _)).WillRepeatedly(Return(0));
        EXPECT_CALL(*m_mock_file_impl, hdfsFlush(_, _)).WillRepeatedly(Return(0));
        EXPECT_CALL(*m_mock_file_impl, hdfsRead(_, _, _, _)).
            WillRepeatedly(Invoke(this, &MockHDFSClientTest::MockhdfsReadSuccess));
        EXPECT_CALL(*m_mock_file_impl, hdfsWrite(_, _, _, _)).
            WillRepeatedly(Return(g_test_contents.size()));
    }

    void TearDown() {}

protected:
    tSize MockhdfsReadSuccess(hdfsFS fs, hdfsFile file, void* buffer, tSize length)
    {
        const char* contents = g_test_contents.c_str();
        int64_t size = g_test_contents.size();

        ::memcpy(buffer, contents, size);
        return size;
    }

protected:
    // HDFS的Mock实现
    MockHDFS* m_mock_file_impl;

    int32_t m_mock_any;
    ::hdfsFS m_mock_fs;
    ::hdfsFile m_mock_file;
};

/// 和上面那个DISTABLE了的测试一模一样，只不过底层实现不是java那个HDFS了而是MOCK
TEST_F(MockHDFSClientTest, DISABLED_ReadTest)
{
    // 默认的配置得到一个HDFSClient，也即从LocalFileSystem
    HDFSClient* client = HDFSClient::GetHDFSClient();
    CHECK_NOTNULL(client);

    HDFSFile* read_file = client->Open(read_file_path, file::OpenModel::kRead);
    CHECK_NOTNULL(read_file);

    const size_t kSize = 100;
    char buffer[kSize] = {'\0'};
    size_t read_length = read_file->Read(buffer, kSize);
    ASSERT_EQ(read_length, g_test_contents.size());
    ASSERT_EQ(g_test_contents, std::string(buffer, read_length));
    LOG(INFO) << "buffer contents: " << std::string(buffer, read_length);

    read_file->Close();
    delete read_file;
    HDFSClient::ReleaseHDFSClient(client);
}

/// 和上面那个DISTABLE了的测试一模一样，只不过底层实现不是java那个HDFS了而是MOCK
TEST_F(MockHDFSClientTest, DISABLED_WriteTest)
{
    // 默认的配置得到一个HDFSClient，也即从LocalFileSystem
    HDFSClient* client = HDFSClient::GetHDFSClient();
    CHECK_NOTNULL(client);
    // 以读写方式打开文件
    HDFSFile* write_file = client->Open(write_file_path, file::OpenModel::kWrite);
    CHECK_NOTNULL(write_file);

    const char* write_buffer = g_test_contents.c_str();
    size_t write_size = g_test_contents.size();
    size_t write_length = write_file->Write(write_buffer, write_size);
    ASSERT_EQ(write_length, write_size);

    write_file->Close();
    write_file = client->Open(write_file_path, file::OpenModel::kRead);
    const size_t kSize = 100;
    char buffer[kSize] = {'\0'};
    size_t read_length = write_file->Read(buffer, kSize);
    ASSERT_EQ(read_length, g_test_contents.size());
    ASSERT_EQ(g_test_contents, std::string(buffer, read_length));
    LOG(INFO) << "buffer contents: " << std::string(buffer, read_length);

    write_file->Close();
    delete write_file;
    HDFSClient::ReleaseHDFSClient(client);
}

} // namespace common
