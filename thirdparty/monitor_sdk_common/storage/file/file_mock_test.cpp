// Copyright (c) 2015, Vivo Inc. All rights reserved.
/**
 * @file file_mock_test.cpp
 * @brief
 * @author chenzheng05@baidu.com
 * @date 2015-01-23
 */
#include <string>

#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/storage/file/file_mock.h"

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/glog/raw_logging.h"
#include "thirdparty/gtest/gtest.h"

DECLARE_bool(filemock_use_filemock);
DECLARE_int32(filemock_read_mock);
DECLARE_int32(filemock_get_size_zero);
DECLARE_int32(filemock_write_mock);
DECLARE_int32(filemock_readline_mock);
DECLARE_int32(filemock_flush_mock);
DECLARE_int32(filemock_close_mock);
DECLARE_int32(filemock_seek_mock);
DECLARE_int32(filemock_tell_mock);
DECLARE_int32(filemock_file_exist);
DECLARE_int32(filemock_return_value);

std::string g_test_file_name = "mock/file_test.txt";

class TestEnvironment : public testing::Environment
{
public:
    virtual void SetUp()
    {
        RAW_LOG(INFO, "TestEnvironment SetUp");
        bool init_ok = File::Init();
        EXPECT_EQ(true, init_ok);
    }


    virtual void TearDown()
    {
        RAW_LOG(INFO, "TestEnvironment TearDown");
        File::CleanUp();
    }
};

class FileMockTest : public testing::Test
{
public:
    void RecoverFlags()
    {
        FLAGS_filemock_read_mock = 0x7fffffff;
        FLAGS_filemock_get_size_zero = 0x7fffffff;
        FLAGS_filemock_readline_mock = 0x7fffffff;
        FLAGS_filemock_write_mock = 0x7fffffff;
        FLAGS_filemock_close_mock = 0x7fffffff;
        FLAGS_filemock_seek_mock = 0x7fffffff;
        FLAGS_filemock_tell_mock = 0x7fffffff;
        FLAGS_filemock_flush_mock = 0x7fffffff;
        FLAGS_filemock_return_value = -1;
    }
    scoped_ptr<common::FileMock> m_file_mock;

private:
    virtual void SetUp()
    {
        RAW_LOG(INFO, "FileMockTest SetUp");
        m_file_mock.reset();
    }

    virtual void TearDown()
    {
        RAW_LOG(INFO, "FileMockTest TearDown");
    }
};

// 验证在设定的次数下面能够返回需要的错误值
TEST_F(FileMockTest, FileMock_Fail)
{
    int open_mode = File::ENUM_FILE_OPEN_MODE_W;
    const char* file_name = g_test_file_name.c_str();
    uint32_t error_code = 0;
    char read_buffer[100];
    char write_buffer[50];
    memset(write_buffer, 0, sizeof(write_buffer));
    snprintf(write_buffer, sizeof(write_buffer), "%s", "write this string into the file!");
    m_file_mock.reset(reinterpret_cast<common::FileMock*>((File::Open(file_name,
                            open_mode, OpenFileOptions(), &error_code))));
    ASSERT_TRUE(m_file_mock.get() != NULL);

    // 验证Exist
    RecoverFlags();
    FLAGS_filemock_file_exist = 1;
    ASSERT_TRUE(m_file_mock->CheckExist(file_name, &error_code));
    ASSERT_FALSE(m_file_mock->CheckExist(file_name, &error_code));
    ASSERT_EQ(static_cast<common::FILE_ERROR_CODE>(FLAGS_filemock_return_value),
              static_cast<int32_t>(error_code));

    RecoverFlags();
    // 验证write mock
    FLAGS_filemock_write_mock = 20;
    for (int64_t cnt = 0; cnt < 20; ++cnt)
    {
        m_file_mock->Write(&write_buffer, 50, &error_code);
        ASSERT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    }
    m_file_mock->Write(&write_buffer, 50, &error_code);
    ASSERT_NE(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    m_file_mock->Close(&error_code);

    RecoverFlags();
    open_mode = File::ENUM_FILE_OPEN_MODE_R;
    m_file_mock.reset(reinterpret_cast<common::FileMock*>(File::Open(file_name,
                            open_mode, OpenFileOptions(), &error_code)));
    EXPECT_TRUE(m_file_mock.get() !=  NULL);

    RecoverFlags();
    // 验证GetSize mock
    FLAGS_filemock_get_size_zero = 2;
    ASSERT_EQ(1000, m_file_mock->GetSize(file_name, &error_code));
    ASSERT_EQ(1000, m_file_mock->GetSize(file_name, &error_code));
    ASSERT_EQ(0, m_file_mock->GetSize(file_name, &error_code));
    ASSERT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);

    RecoverFlags();
    // 验证read mock
    FLAGS_filemock_read_mock = 2;
    m_file_mock->Read(&read_buffer, 50, &error_code);
    ASSERT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    m_file_mock->Read(&read_buffer, 50, &error_code);
    ASSERT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    m_file_mock->Read(&read_buffer, 50, &error_code);
    ASSERT_NE(static_cast<uint32_t>(ERR_FILE_OK), error_code);

    RecoverFlags();
    FLAGS_filemock_readline_mock = 10;
    for (int64_t cnt = 0; cnt < 10; ++cnt)
    {
        ASSERT_NE(-1, m_file_mock->ReadLine(read_buffer, 100));
    }
    ASSERT_EQ(-1, m_file_mock->ReadLine(read_buffer, 100));

    RecoverFlags();
    FLAGS_filemock_seek_mock = 10;
    for (int64_t cnt = 0; cnt < 10; ++cnt)
    {
        ASSERT_NE(-1, m_file_mock->Seek(100, 0, &error_code));
    }
    ASSERT_EQ(-1, m_file_mock->Seek(100, 0, &error_code));

    RecoverFlags();
    int64_t offset = 0;
    ASSERT_NE(-1, (offset = m_file_mock->Seek(100, 0, &error_code)));
    FLAGS_filemock_tell_mock = 10;
    for (int64_t cnt = 0; cnt < 10; ++cnt)
    {
        ASSERT_EQ(offset, m_file_mock->Tell(&error_code)) << cnt;
    }
    ASSERT_EQ(-1, m_file_mock->Tell(&error_code));

    // Flush
    RecoverFlags();
    FLAGS_filemock_flush_mock = 2;
    ASSERT_EQ(0, m_file_mock->Flush(&error_code));
    ASSERT_EQ(0, m_file_mock->Flush(&error_code));
    ASSERT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    ASSERT_EQ(-1, m_file_mock->Flush(&error_code));
    ASSERT_EQ(static_cast<common::FILE_ERROR_CODE>(FLAGS_filemock_return_value),
              static_cast<int32_t>(error_code));

    RecoverFlags();
    FLAGS_filemock_close_mock = 0;
    ASSERT_EQ(-1, m_file_mock->Close(&error_code));
    RecoverFlags();
    ASSERT_EQ(0, m_file_mock->Close(&error_code));
}

int main(int argc, char** argv)
{
    google::InitGoogleLogging(argv[0]);
    testing::InitGoogleTest(&argc, argv);
    testing::GTEST_FLAG(break_on_failure) = 1;
    FLAGS_log_dir = "./";
    FLAGS_stderrthreshold = 3;
    return  RUN_ALL_TESTS();
}
