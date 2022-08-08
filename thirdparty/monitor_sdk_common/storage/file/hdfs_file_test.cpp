// Copyright (c) 2015, Vivo Inc. All rights reserved.
/**
 * @file hdfs_file_test.cpp
 * @brief
 * @author chenzheng05@baidu.com
 * @date 2015-01-21
 */

#include "thirdparty/monitor_sdk_common/storage/file/hdfs_file.h"

#include <time.h>

#include <string>

#include "thirdparty/monitor_sdk_common/base/module.h"
#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/random/pseudo_random.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gmock/gmock.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

std::string g_hdfs_test_dir = std::string("/hdfs/user/stan/hdfs_test_dir");
std::string g_hdfs_test_file = g_hdfs_test_dir + "/hdfs_test_file";
HdfsFile* g_local_file = NULL;

using ::testing::DoAll;
using ::testing::Return;
using ::testing::_;

ACTION_TEMPLATE(SetArrayArgumentByChar,
                HAS_1_TEMPLATE_PARAMS(int, k),
                AND_2_VALUE_PARAMS(first, last)) {
    ::std::copy(first, last, reinterpret_cast<char*>(::std::tr1::get<k>(args)));
}

class MockHdfsFile : public HdfsFile {
public:
    MockHdfsFile() {}

    virtual ~MockHdfsFile() {}

    MOCK_METHOD3(Read, int64_t(void* buf, int64_t buf_size, uint32_t* error_code));
};

class TEST_HdfsFile: public testing::Test
{
protected:
    static void SetUpTestCase()
    {
        EXPECT_EQ(true, File::Init());
        uint32_t error_code;
        const char* file = g_hdfs_test_dir.c_str();
        bool is_recursive = true;
        if (File::CheckExist(file, &error_code))
            EXPECT_EQ(0, File::Remove(file, is_recursive, &error_code));
    }

    static void TearDownTestCase()
    {
        File::CleanUp();
    }
};

TEST_F(TEST_HdfsFile, DISABLED_CheckExist)
{
    uint32_t error_code = 0;
    const char* file_name = "/hdfs/.";
    EXPECT_TRUE(File::CheckExist(file_name, &error_code));
    EXPECT_EQ(static_cast<int32_t>(error_code), ERR_FILE_OK);
}

TEST_F(TEST_HdfsFile, DISABLED_AddDir)
{
    uint32_t error_code = 0;
    const char* dir_name = g_hdfs_test_dir.c_str();
    EXPECT_EQ(0, File::AddDir(dir_name, &error_code));
    EXPECT_EQ(static_cast<int32_t>(error_code), ERR_FILE_OK);
    EXPECT_TRUE(File::CheckExist(dir_name, &error_code));
    EXPECT_EQ(static_cast<int32_t>(error_code), ERR_FILE_OK);
}

TEST_F(TEST_HdfsFile, DISABLED_Write)
{
    uint32_t error_code = 0;
    const char* filename = g_hdfs_test_file.c_str();
    int32_t mode = File::ENUM_FILE_OPEN_MODE_W;
    g_local_file = static_cast<HdfsFile*>
        (HdfsFile::Open(filename, mode, OpenFileOptions(), &error_code));
    EXPECT_TRUE(g_local_file);

    const char* test_string = "stanyu";
    std::string buf_str(test_string);
    const char* buf = buf_str.c_str();
    int64_t test_string_len = strlen(test_string);
    int64_t wirten_len = g_local_file->Write(buf, test_string_len, &error_code);
    EXPECT_EQ(test_string_len, wirten_len);
    EXPECT_EQ(static_cast<int32_t>(error_code), ERR_FILE_OK);
    g_local_file->Flush(&error_code);
    EXPECT_EQ(static_cast<int32_t>(error_code), ERR_FILE_OK);
    g_local_file->Close(&error_code);
    EXPECT_EQ(static_cast<int32_t>(error_code), ERR_FILE_OK);
}

TEST_F(TEST_HdfsFile, DISABLED_Read)
{
    uint32_t error_code = 0;
    const char* filename = g_hdfs_test_file.c_str();
    int32_t mode = File::ENUM_FILE_OPEN_MODE_R;
    g_local_file = static_cast<HdfsFile*>
        (HdfsFile::Open(filename, mode, OpenFileOptions(), &error_code));
    EXPECT_TRUE(g_local_file);

    char buf[6];
    int read_len = g_local_file->Read(buf, sizeof(buf), &error_code);
    EXPECT_EQ(read_len, 6);
    int mem_cmp = memcmp(buf, "stanyu", 6);
    EXPECT_EQ(mem_cmp, 0);
    EXPECT_EQ(static_cast<int32_t>(error_code), ERR_FILE_OK);

    g_local_file->Close(&error_code);
    EXPECT_EQ(static_cast<int32_t>(error_code), ERR_FILE_OK);
}

TEST_F(TEST_HdfsFile, DISABLED_GetFileSize)
{
    uint32_t error_code;
    const char* file_name = g_hdfs_test_file.c_str();
    int64_t ret = File::GetSize(file_name, &error_code);
    EXPECT_EQ(6, ret);
    EXPECT_EQ(static_cast<int32_t>(error_code), ERR_FILE_OK);
    const char* dir_name = g_hdfs_test_dir.c_str();
    ret = File::GetSize(dir_name, &error_code);
    EXPECT_EQ(-1, ret);
    EXPECT_EQ(static_cast<int32_t>(error_code), ERR_FILE_FAIL);
}

TEST_F(TEST_HdfsFile, DISABLED_Rename)
{
    uint32_t error_code;
    std::string new_name = g_hdfs_test_file + ".new";

    EXPECT_EQ(0, File::Rename(g_hdfs_test_file.c_str(), new_name.c_str(), &error_code));
    EXPECT_EQ(error_code, ERR_FILE_OK);
    EXPECT_TRUE(File::CheckExist(new_name.c_str(), &error_code));
    EXPECT_FALSE(File::CheckExist(g_hdfs_test_file.c_str(), &error_code));

    EXPECT_EQ(0, File::Rename(new_name.c_str(), g_hdfs_test_file.c_str(), &error_code));
    EXPECT_EQ(error_code, ERR_FILE_OK);
    EXPECT_FALSE(File::CheckExist(new_name.c_str(), &error_code));
    EXPECT_TRUE(File::CheckExist(g_hdfs_test_file.c_str(), &error_code));
}

TEST_F(TEST_HdfsFile, DISABLED_SeekAndTell)
{
    uint32_t error_code = 0;
    const char* filename = g_hdfs_test_file.c_str();
    int32_t mode = File::ENUM_FILE_OPEN_MODE_R;
    g_local_file = static_cast<HdfsFile*>
        (HdfsFile::Open(filename, mode, OpenFileOptions(), &error_code));
    EXPECT_TRUE(g_local_file);
    EXPECT_EQ(6, g_local_file->Seek(-1, SEEK_END, &error_code));
    EXPECT_EQ(6, g_local_file->Tell(&error_code));
    EXPECT_EQ(error_code, ERR_FILE_OK);

    g_local_file->Close(&error_code);
    EXPECT_EQ(static_cast<int32_t>(error_code), ERR_FILE_OK);
}

TEST_F(TEST_HdfsFile, DISABLED_Move)
{
    uint32_t error_code;
    std::string new_name = g_hdfs_test_file + ".new";
    EXPECT_EQ(0, File::Move(g_hdfs_test_file.c_str(), new_name.c_str(), &error_code));
    EXPECT_EQ(error_code, ERR_FILE_OK);
    EXPECT_TRUE(File::CheckExist(new_name.c_str(), &error_code));
    EXPECT_FALSE(File::CheckExist(g_hdfs_test_file.c_str(), &error_code));

    EXPECT_EQ(0, File::Move(new_name.c_str(), g_hdfs_test_file.c_str(), &error_code));
    EXPECT_EQ(error_code, ERR_FILE_OK);
    EXPECT_FALSE(File::CheckExist(new_name.c_str(), &error_code));
    EXPECT_TRUE(File::CheckExist(g_hdfs_test_file.c_str(), &error_code));
}

TEST_F(TEST_HdfsFile, DISABLED_Copy)
{
    uint32_t error_code = 0;
    std::string new_name  = g_hdfs_test_file + ".new";
    const char* dst = new_name.c_str();
    EXPECT_EQ(0, File::Copy(g_hdfs_test_file.c_str(), dst, &error_code));
    EXPECT_EQ(error_code, ERR_FILE_OK);
}

TEST_F(TEST_HdfsFile, DISABLED_List)
{
    uint32_t error_code;
    std::vector<AttrsInfo> file_info;
    AttrsMask mask;
    mask.file_size = 1;

    ListOptions listOptions(&mask);

    EXPECT_EQ(0, File::List(g_hdfs_test_dir.c_str(), listOptions, &file_info, &error_code));
    EXPECT_EQ(error_code, ERR_FILE_OK);
    EXPECT_EQ(2u, file_info.size());
    EXPECT_EQ(6u, file_info[0].file_size);

    LOG(INFO) << "input list name: " << g_hdfs_test_dir.c_str();
    std::vector<AttrsInfo>::iterator iter;
    for (iter = file_info.begin(); iter != file_info.end(); ++iter) {
        LOG(INFO) << iter->file_name;
    }

    EXPECT_EQ(0, File::List(g_hdfs_test_file.c_str(), listOptions, &file_info, &error_code));
    EXPECT_EQ(error_code, ERR_FILE_OK);
    LOG(INFO) << "input list name: " << g_hdfs_test_file.c_str();
    for (iter = file_info.begin(); iter != file_info.end(); ++iter) {
        LOG(INFO) << iter->file_name;
    }
    // input file type is file.
    EXPECT_EQ(1u, file_info.size());
}

TEST_F(TEST_HdfsFile, ReadLine_ShortLine) {

    scoped_ptr<MockHdfsFile> mock_hdfs_file(new MockHdfsFile());
    // in case that HdfsFile::Read was mocked, just ignore s_fs or m_fd which is NULL
    mock_hdfs_file->m_buffer_cache = new char[HdfsFile::kBufferCacheSize];
    mock_hdfs_file->m_cache_front_idx = 0;
    mock_hdfs_file->m_cache_back_idx = 0;

    const int32_t kBufferSize = 4096;
    scoped_array<char> baseline_buffer(new char[kBufferSize]);
    PseudoRandom random(time(NULL));
    for (int32_t i = 0; i < kBufferSize - 1; ++i) {
        if (i != 0 && i % 127 == 0) {
            baseline_buffer[i] = '\n';
        } else {
            baseline_buffer[i] =
                static_cast<char>(static_cast<uint32_t>('A') +
                                  random.NextUInt32(static_cast<uint32_t>('z' - 'A')));
        }
    }
    baseline_buffer[kBufferSize - 1] = '\0';

    EXPECT_CALL(*(mock_hdfs_file.get()), Read(_, _, _))
                .Times(2)
                .WillOnce(DoAll(SetArrayArgumentByChar<0>(&baseline_buffer[0],
                                                          &baseline_buffer[kBufferSize - 1]),
                                Return(kBufferSize - 1)))
                .WillOnce(DoAll(SetArrayArgumentByChar<0>(&baseline_buffer[0],
                                                          &baseline_buffer[kBufferSize - 1]),
                                Return(kBufferSize - 1)));

    std::string line;
    int32_t start_pos = 0;

    // buffer cache is empty, call HdfsFile::Read once
    mock_hdfs_file->File::ReadLine(&line);
    EXPECT_EQ(128, line.length());  // 127 regular characters and an '\n'

    for (size_t i = 0; i < line.size(); ++i) {
        EXPECT_EQ(baseline_buffer[(start_pos + i) % (kBufferSize - 1)], line[i]);
    }
    start_pos += line.size();
    EXPECT_EQ(128, mock_hdfs_file->m_cache_front_idx);
    EXPECT_EQ(kBufferSize - 1, mock_hdfs_file->m_cache_back_idx);

    // the following loop may read lines from buffer cache, which is not empty
    for (int32_t time = 1; time < (kBufferSize - 1) / 127; ++time) {
        mock_hdfs_file->File::ReadLine(&line);
        EXPECT_EQ(127, line.length());  // 126 regular characters and an '\n'

        for (size_t i = 0; i < line.size(); ++i) {
            EXPECT_EQ(baseline_buffer[(start_pos + i) % (kBufferSize - 1)], line[i]);
        }
        start_pos += line.size();
        EXPECT_EQ(128 + 127 * time, mock_hdfs_file->m_cache_front_idx);
        EXPECT_EQ(kBufferSize - 1, mock_hdfs_file->m_cache_back_idx);
    }

    // buffer cache should has been emptied, call HdfsFile::Read once
    mock_hdfs_file->File::ReadLine(&line);
    // 30 (cached) plus 127 (newly read) regular characters and an '\n'
    EXPECT_EQ(128 + ((kBufferSize - 1) % 127 - 1), line.length());

    for (size_t i = 0; i < line.size(); ++i) {
        EXPECT_EQ(baseline_buffer[(start_pos + i) % (kBufferSize - 1)], line[i]);
    }
    EXPECT_EQ(128, mock_hdfs_file->m_cache_front_idx);
    EXPECT_EQ(kBufferSize - 1, mock_hdfs_file->m_cache_back_idx);

    delete []mock_hdfs_file->m_buffer_cache;
    mock_hdfs_file->m_buffer_cache = NULL;
}

TEST_F(TEST_HdfsFile, ReadLine_LongLine) {

    scoped_ptr<MockHdfsFile> mock_hdfs_file(new MockHdfsFile());
    // in case that HdfsFile::Read was mocked, just ignore s_fs or m_fd which is NULL
    mock_hdfs_file->m_buffer_cache = new char[HdfsFile::kBufferCacheSize];
    mock_hdfs_file->m_cache_front_idx = 0;
    mock_hdfs_file->m_cache_back_idx = 0;

    const int32_t kBufferSize = 4096;
    scoped_array<char> baseline_buffer1(new char[kBufferSize]);
    scoped_array<char> baseline_buffer2(new char[kBufferSize]);
    PseudoRandom random(time(NULL));
    for (int32_t i = 0; i < kBufferSize - 1; ++i) {
        baseline_buffer1[i] =
            static_cast<char>(static_cast<uint32_t>('A') +
                              random.NextUInt32(static_cast<uint32_t>('z' - 'A')));
        baseline_buffer2[i] =
            static_cast<char>(static_cast<uint32_t>('A') +
                              random.NextUInt32(static_cast<uint32_t>('z' - 'A')));
    }
    baseline_buffer1[kBufferSize - 1] = '\0';
    baseline_buffer2[127] = '\n';
    baseline_buffer2[kBufferSize - 4] = '\n';
    baseline_buffer2[kBufferSize - 3] = '\n';
    baseline_buffer2[kBufferSize - 2] = '\n';
    baseline_buffer2[kBufferSize - 1] = '\0';

    EXPECT_CALL(*(mock_hdfs_file.get()), Read(_, _, _))
                .Times(2)
                .WillOnce(DoAll(SetArrayArgumentByChar<0>(&baseline_buffer1[0],
                                                          &baseline_buffer1[kBufferSize - 1]),
                                Return(kBufferSize - 1)))
                .WillOnce(DoAll(SetArrayArgumentByChar<0>(&baseline_buffer2[0],
                                                          &baseline_buffer2[kBufferSize - 1]),
                                Return(kBufferSize - 1)));

    std::string line;
    mock_hdfs_file->File::ReadLine(&line);
    EXPECT_EQ(kBufferSize - 1 + 128, line.length());  // regular characters and an '\n'

    // buffer cache is empty and line is longer than buffer size, call HdfsFile::Read twice
    for (int32_t i = 0; i < kBufferSize - 1; ++i) {
        EXPECT_EQ(baseline_buffer1[i], line[i]);
    }
    for (size_t i = kBufferSize - 1; i < line.size(); ++i) {
        EXPECT_EQ(baseline_buffer2[i - (kBufferSize - 1)], line[i]);
    }
    EXPECT_EQ(128, mock_hdfs_file->m_cache_front_idx);
    EXPECT_EQ(kBufferSize - 1, mock_hdfs_file->m_cache_back_idx);

    // read line from buffer cache, which is not empty
    mock_hdfs_file->File::ReadLine(&line);
    EXPECT_EQ(kBufferSize - 1 - 128 - 2, line.length());  // regular characters and an '\n'
    for (size_t i = 0; i < line.size(); ++i) {
        EXPECT_EQ(baseline_buffer2[i + 128], line[i]);
    }
    EXPECT_EQ(kBufferSize - 1 - 2, mock_hdfs_file->m_cache_front_idx);
    EXPECT_EQ(kBufferSize - 1, mock_hdfs_file->m_cache_back_idx);

    // read an '\n'
    mock_hdfs_file->File::ReadLine(&line);
    EXPECT_EQ(1, line.length()); // an '\n'
    EXPECT_EQ('\n', line[0]);
    EXPECT_EQ(kBufferSize - 1 - 1, mock_hdfs_file->m_cache_front_idx);
    EXPECT_EQ(kBufferSize - 1, mock_hdfs_file->m_cache_back_idx);

    // read an '\n'
    mock_hdfs_file->File::ReadLine(&line);
    EXPECT_EQ(1, line.length()); // an '\n'
    EXPECT_EQ('\n', line[0]);
    EXPECT_EQ(kBufferSize - 1, mock_hdfs_file->m_cache_front_idx);
    EXPECT_EQ(kBufferSize - 1, mock_hdfs_file->m_cache_back_idx);

    delete []mock_hdfs_file->m_buffer_cache;
    mock_hdfs_file->m_buffer_cache = NULL;
}

} // namespace common

int main(int argc, char* argv[])
{
    google::SetCommandLineOption("hdfs_enable_unittest_mode", "true");

    ::common::InitAllModulesAndTest(&argc, &argv);
    return RUN_ALL_TESTS();
}
