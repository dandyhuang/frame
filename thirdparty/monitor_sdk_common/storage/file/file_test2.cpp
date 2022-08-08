// Copyright (c) 2015, Vivo Inc. All rights reserved.
/**
 * @file file_test.cpp
 * @brief
 * @author chenzheng05@baidu.com
 * @date 2015-01-20
 */
//
// test functions of file.cc and local_file.cc

#include <ctype.h>
#include <grp.h>
#include <pwd.h>
#include <sys/types.h>

#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/storage/file/file2.h"
#include "thirdparty/monitor_sdk_common/storage/file/local_file2.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/event.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/thread.h"
#include "thirdparty/monitor_sdk_common/system/io/directory.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

std::string g_test_dir_name = std::string("") + "file_test_dir";
std::string g_test_file_name = g_test_dir_name + File::kPathSeparator + "file_test.txt";
File* g_test_file_obj = NULL;
File* g_test_file_obj_r = NULL;

int64_t g_write_count = 100;

void ReadCallback(AutoResetEvent* sync_event, int64_t size, uint32_t error_code) {
    VLOG(1) << "File test, read callback is called, result size = "
        << size << " error_code = " << error_code;
    if (sync_event != NULL) sync_event->Set();
}

void WriteCallback(AutoResetEvent* sync_event, int64_t size, uint32_t error_code) {
    VLOG(1) << "File test, write callback is called, result size = "
        << size << " error_code = " << error_code;
    if (sync_event != NULL) sync_event->Set();
}

void LongWriteCallback(AutoResetEvent* sync_event,
                       int64_t expect_size,
                       int64_t size,
                       uint32_t error_code) {
    EXPECT_EQ(expect_size, size);
    VLOG(1) << "File test, write callback is called, result size = "
        << size << " error_code = " << error_code;
    if (sync_event != NULL) sync_event->Set();
}

class FileTest : public testing::Test {
protected:
    static void SetUpTestCase() {
        // Should fail if init fails.
        ASSERT_EQ(true, File::Init());

        uint32_t error_code = 0;
        const char* dir_name = g_test_dir_name.c_str();
        if (File::CheckExist(dir_name, &error_code)) {
            uint32_t error_code2 = 0;
            File::Remove(dir_name, true, &error_code2);
            EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code2);
            ASSERT_FALSE(File::CheckExist(dir_name, &error_code));
        }
    }

    static void TearDownTestCase() {
        File::CleanUp();
    }
};

TEST_F(FileTest, KeyValueInfoHelperTest) {
    std::string options;
    std::map<std::string, std::string> options_map;
    KeyValueInfoHelper::ParseKeyValueInfo(options, &options_map);
    EXPECT_EQ(0u, options_map.size());

    // ignore options without ":" or '='.
    bool rst = false;
    options = "a";
    rst = KeyValueInfoHelper::ParseKeyValueInfo(options, &options_map);
    EXPECT_EQ(0u, options_map.size());
    EXPECT_EQ(rst, false);

    options = "a=";
    rst = KeyValueInfoHelper::ParseKeyValueInfo(options, &options_map);
    EXPECT_EQ(0u, options_map.size());
    EXPECT_EQ(rst, false);

    options = "r=1";
    rst = KeyValueInfoHelper::ParseKeyValueInfo(options, &options_map);
    EXPECT_EQ(1u, options_map.size());
    EXPECT_EQ(true, rst);
    std::string newOptions;
    rst = KeyValueInfoHelper::CreateKeyValueInfo(options_map, &newOptions);
    EXPECT_EQ(options, newOptions);
    EXPECT_EQ(true, rst);

    options = "fid=123456:r=5";
    rst = KeyValueInfoHelper::ParseKeyValueInfo(options, &options_map);
    EXPECT_EQ(2u, options_map.size());
    EXPECT_TRUE(options_map.find("r") != options_map.end());
    EXPECT_EQ(true, rst);

    std::string replica = options_map["r"];
    int r = atoi(replica.c_str());
    VLOG(1) << "parsed replica: --" << r << "--";
    EXPECT_EQ(5, r);
    rst = KeyValueInfoHelper::CreateKeyValueInfo(options_map, &newOptions);
    EXPECT_EQ(options, newOptions);
    EXPECT_EQ(true, rst);

    std::string key("fid");
    std::string value("33311124");
    std::string info;

    rst = KeyValueInfoHelper::AppendKeyValueInfo(key, value, NULL);
    EXPECT_EQ(rst, false);
    rst = KeyValueInfoHelper::AppendKeyValueInfo("", value, NULL);
    EXPECT_EQ(rst, false);
    rst = KeyValueInfoHelper::AppendKeyValueInfo(key, "", NULL);
    EXPECT_EQ(rst, false);

    rst = KeyValueInfoHelper::AppendKeyValueInfo(key, value, &info);
    EXPECT_EQ(rst, true);
    EXPECT_EQ(info, "fid=33311124");

    rst = KeyValueInfoHelper::AppendKeyValueInfo(key, value, &info);
    EXPECT_EQ(rst, true);
    EXPECT_EQ(info, "fid=33311124:fid=33311124");
}

// @brief:   测试GetFileTypeDesc
TEST_F(FileTest, GetFileTypeDesc) {
    EXPECT_STREQ("FILE_TYPE_UNKNOWNTYPE", GetFileTypeDesc(FILE_TYPE_UNKNOWNTYPE));
    EXPECT_STREQ("FILE_TYPE_NORMAL", GetFileTypeDesc(FILE_TYPE_NORMAL));
    EXPECT_STREQ("FILE_TYPE_DIR", GetFileTypeDesc(FILE_TYPE_DIR));
    EXPECT_STREQ("FILE_TYPE_RA", GetFileTypeDesc(FILE_TYPE_RA));
}

// @brief:   测试GetFileErrorCodeStr
TEST_F(FileTest, GetFileErrorCodeStr) {
    EXPECT_STREQ("ERR_FILE_FAIL", GetFileErrorCodeStr(ERR_FILE_FAIL));
    EXPECT_STREQ("ERR_FILE_OK", GetFileErrorCodeStr(static_cast<uint32_t>(ERR_FILE_OK)));
    EXPECT_STREQ("ERR_FILE_RETRY", GetFileErrorCodeStr(ERR_FILE_RETRY));
    EXPECT_STREQ("ERR_FILE_REOPEN_FOR_WRITE", GetFileErrorCodeStr(ERR_FILE_REOPEN_FOR_WRITE));
    EXPECT_STREQ("ERR_FILE_ENTRY_EXIST", GetFileErrorCodeStr(ERR_FILE_ENTRY_EXIST));
    EXPECT_STREQ("ERR_FILE_ENTRY_NOT_EXIST",
                 GetFileErrorCodeStr(static_cast<uint32_t>(ERR_FILE_ENTRY_NOT_EXIST)));
    EXPECT_STREQ("ERR_FILE_NOT_INIT", GetFileErrorCodeStr(ERR_FILE_NOT_INIT));
    EXPECT_STREQ("ERR_FILE_CLOSED", GetFileErrorCodeStr(ERR_FILE_CLOSED));
    EXPECT_STREQ("ERR_FILE_OPENMODE",
                 GetFileErrorCodeStr(ERR_FILE_OPENMODE));
    EXPECT_STREQ("ERR_FILE_PARAMETER",
                 GetFileErrorCodeStr(static_cast<uint32_t>(ERR_FILE_PARAMETER)));
    EXPECT_STREQ("ERR_FILE_PERMISSION_DENIED",
                 GetFileErrorCodeStr(static_cast<uint32_t>(ERR_FILE_PERMISSION_DENIED)));
    EXPECT_STREQ("ERR_FILE_NOT_EMPTY_DIRECTORY",
                 GetFileErrorCodeStr(ERR_FILE_NOT_EMPTY_DIRECTORY));
    EXPECT_STREQ("ERR_FILE_QUOTA_EXCEED", GetFileErrorCodeStr(ERR_FILE_QUOTA_EXCEED));
    EXPECT_STREQ("ERR_FILE_TOO_MANY_ENTRIES", GetFileErrorCodeStr(ERR_FILE_TOO_MANY_ENTRIES));
    EXPECT_STREQ("ERR_FILE_INPROGRESS", GetFileErrorCodeStr(ERR_FILE_INPROGRESS));
    EXPECT_STREQ("ERR_FILE_ENTRY_EXIST", GetFileErrorCodeStr(ERR_FILE_ENTRY_EXIST));
    EXPECT_STREQ("ERR_FILE_PATH_TOO_LONG", GetFileErrorCodeStr(ERR_FILE_PATH_TOO_LONG));
    EXPECT_STREQ("ERR_FILE_EXPECTED", GetFileErrorCodeStr(ERR_FILE_EXPECTED));
    EXPECT_STREQ("ERR_FILE_REMOVED", GetFileErrorCodeStr(ERR_FILE_REMOVED));
    EXPECT_STREQ("ERR_FILE_CORRUPT", GetFileErrorCodeStr(ERR_FILE_CORRUPT));
    EXPECT_STREQ("ERR_FILE_OUT_OF_RANGE", GetFileErrorCodeStr(ERR_FILE_OUT_OF_RANGE));
    EXPECT_STREQ("ERR_FILE_OVERWRITE_NON_DIRECTORY_WITH_DIRECTORY",
                 GetFileErrorCodeStr(ERR_FILE_OVERWRITE_NON_DIRECTORY_WITH_DIRECTORY));
    EXPECT_STREQ("ERR_FILE_OVERWRITE_DIRECTORY_WITH_NON_DIRECTORY",
                 GetFileErrorCodeStr(ERR_FILE_OVERWRITE_DIRECTORY_WITH_NON_DIRECTORY));
    EXPECT_STREQ("ERR_FILE_INVALID_ROLE_NAME", GetFileErrorCodeStr(ERR_FILE_INVALID_ROLE_NAME));
    EXPECT_STREQ("ERR_FILE_EMPTY", GetFileErrorCodeStr(ERR_FILE_EMPTY));
    EXPECT_STREQ("ERR_FILE_ANCESTOR_NOT_DIR", GetFileErrorCodeStr(ERR_FILE_ANCESTOR_NOT_DIR));
    EXPECT_STREQ("ERR_FILE_INVALID_PERMISSION_VALUE",
                 GetFileErrorCodeStr(ERR_FILE_INVALID_PERMISSION_VALUE));
    EXPECT_STREQ("ERR_FILE_EXCEED_MAX_DEPTH", GetFileErrorCodeStr(ERR_FILE_EXCEED_MAX_DEPTH));
    EXPECT_STREQ("ERR_FILE_VERSION_INCOMPATIBLE",
                 GetFileErrorCodeStr(ERR_FILE_VERSION_INCOMPATIBLE));
    EXPECT_STREQ("ERR_FILE_DIR_EXPECTED", GetFileErrorCodeStr(ERR_FILE_DIR_EXPECTED));
    EXPECT_STREQ("ERR_FILE_NOSPACE", GetFileErrorCodeStr(ERR_FILE_NOSPACE));
    EXPECT_STREQ("ERR_FILE_UNKNOWN", GetFileErrorCodeStr(ERR_FILE_FAIL + 1000));
}

TEST_F(FileTest, FilePathParserTest) {
    uint32_t file_back_factor = 1;
    uint32_t cache_buf_len = 5;
    uint32_t cache_data_interval = 10;

    std::string file_name("/xfs/sz/dir1/my_file.txt");
    std::string temp_full_path = file_name + ":r=5:bufsz=100000:bufit=1000";
    std::string full_path = temp_full_path;

    // Parse additional options in file path.
    std::map<std::string, std::string> options_map;
    size_t optionIdx = temp_full_path.find(":");
    EXPECT_TRUE(optionIdx != std::string::npos);

    if (optionIdx != std::string::npos) {
        // has additional options in file path.
        bool parse_ok = KeyValueInfoHelper::ParseKeyValueInfo(
                                temp_full_path.substr(optionIdx + 1),
                                &options_map);
        EXPECT_EQ(parse_ok, true);

        // Overwrite options in OpenFileOption.
        if (options_map.find("r") != options_map.end()) {
            const std::string& r_str = options_map["r"];
            file_back_factor = atoi(r_str.c_str());
        }
        EXPECT_EQ(file_back_factor, 5u);
        if (options_map.find("bufsz") != options_map.end()) {
            const std::string& sz_str = options_map["bufsz"];
            cache_buf_len = atoi(sz_str.c_str());
        }
        EXPECT_EQ(cache_buf_len, 100000u);
        if (options_map.find("bufit") != options_map.end()) {
            const std::string& it_str = options_map["bufit"];
            cache_data_interval = atoi(it_str.c_str());
        }
        EXPECT_EQ(cache_data_interval, 1000u);

        // The real path should skip the options.
        full_path = temp_full_path.substr(0, optionIdx);
        EXPECT_EQ(full_path, file_name);
        VLOG(1) << "input path:" << temp_full_path;
        VLOG(1) << "parsed path:" << full_path;
    }
}

TEST_F(FileTest, AddDir) {
    uint32_t error_code = 0;
    const char* dir_name = g_test_dir_name.c_str();
    EXPECT_EQ(0, File::AddDir(dir_name, &error_code));
    EXPECT_EQ(error_code, static_cast<uint32_t>(ERR_FILE_OK));

    EXPECT_EQ(true, File::CheckExist(dir_name, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);

    EXPECT_EQ(true, File::CheckExist(".", &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    // add again.
    EXPECT_EQ(-1, File::AddDir(dir_name, &error_code));
    EXPECT_EQ(error_code, static_cast<uint32_t>(ERR_FILE_ENTRY_EXIST));
}

TEST_F(FileTest, AddDirRecursively) {
    uint32_t error_code = 0;
    std::string test_dir = "file_test_dir2";
    std::string file_path = test_dir + "/1/2/3/4/";
    ASSERT_TRUE(File::AddDirRecursively(file_path, &error_code));
    EXPECT_EQ(error_code, static_cast<uint32_t>(ERR_FILE_OK));

    file_path = test_dir + "/a/b/c//";
    ASSERT_TRUE(File::AddDirRecursively(file_path, &error_code));
    EXPECT_TRUE(File::CheckExist(file_path.c_str(), &error_code));
    EXPECT_EQ(error_code, static_cast<uint32_t>(ERR_FILE_OK));

    file_path = test_dir + "1/2";
    ASSERT_TRUE(File::AddDirRecursively(file_path, &error_code));

    file_path = "1";
    ASSERT_TRUE(File::AddDirRecursively(file_path, &error_code));
    EXPECT_TRUE(File::CheckExist(file_path.c_str(), &error_code));
    EXPECT_EQ(error_code, static_cast<uint32_t>(ERR_FILE_OK));

    file_path = "";
    ASSERT_FALSE(File::AddDirRecursively(file_path, &error_code));

    // Add absolute path
    file_path = io::directory::GetCurrentDir() + "/absolute_path/a/b";
    ASSERT_TRUE(File::AddDirRecursively(file_path, &error_code));

    EXPECT_EQ(0, File::Remove(test_dir.c_str(), true));
}

TEST_F(FileTest, Open) {
    int open_mode = File::ENUM_FILE_OPEN_MODE_W;

    uint32_t error_code = 0;
    // First test local file.
    const char* file_name = g_test_file_name.c_str();

    OpenFileOptions options;
    options.backup_factor = 3; // back_factor.
    EXPECT_EQ(NULL, File::Open("", open_mode, options, &error_code));
    g_test_file_obj = File::Open(file_name, open_mode, options, &error_code);
    ASSERT_TRUE(g_test_file_obj != NULL);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    EXPECT_EQ(g_test_file_obj->GetFileImplName(), common::LOCAL_FILE_PREFIX);
    EXPECT_TRUE(File::CheckExist(file_name, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
}

// @brief:   测试local_file.cc的WriteFile
TEST_F(FileTest, WriteFile) {
    uint32_t error_code = 0;
    const char* test_string = "this is a text file,hello world";
    std::string buf_str(test_string);
    const char* buf = buf_str.c_str();
    const int64_t test_string_len = strlen(test_string);
    int64_t wirten_len = g_test_file_obj->Write(buf,
                                                test_string_len,
                                                &error_code);
    VLOG(1) << "write " << test_string_len << " bytes data finish";
    EXPECT_EQ(test_string_len, wirten_len);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    g_test_file_obj->Flush(&error_code);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
}


// @brief:   测试local_file.cc的SeekFile
TEST_F(FileTest, SeekFile) {
    int open_mode = File::ENUM_FILE_OPEN_MODE_R;
    uint32_t error_code = 0;
    // First test local file.
    const char* file_name = g_test_file_name.c_str();

    OpenFileOptions options;
    g_test_file_obj_r = File::Open(file_name, open_mode, options, &error_code);
    ASSERT_TRUE(g_test_file_obj_r != NULL);

    int64_t offset = 5;
    int64_t offset_ret = 0;
    offset_ret = g_test_file_obj_r->Seek(5, SEEK_SET, &error_code);
    EXPECT_EQ(offset, offset_ret);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
}

// @brief:   测试local_file.cc的ReadFile
TEST_F(FileTest, ReadFile) {
    uint32_t error_code = 0;
    char buf[5];

    // after seek.
    int64_t readn_len = g_test_file_obj_r->Read(buf, sizeof(buf) - 1, &error_code);
    EXPECT_EQ(4, readn_len);
    int cmp_result = memcmp(buf, "is a", 4);
    EXPECT_EQ(0, cmp_result);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
}

// @brief:   测试AsyncWriteFile
TEST_F(FileTest, AsyncWriteFile) {
    const char* test_string = "this is a text file,hello world";
    std::string buf_str(test_string);
    const char* buf = buf_str.c_str();

    const int64_t test_string_len = strlen(test_string);

    AutoResetEvent sync_event;
    Closure<void, int64_t, unsigned int> *callback =
        NewClosure(WriteCallback, &sync_event);

    uint32_t error_code;
    g_test_file_obj->Seek(0, SEEK_END, &error_code);
    if (g_test_file_obj->SupportAsync()) {
        EXPECT_EQ(0,
            g_test_file_obj->AsyncWrite(buf,
                test_string_len, callback, 1, &error_code));
        LOG(INFO) << "AsyncWriteFile returned for " << test_string_len << " bytes";
    }
    LOG(INFO) << "to wait async write operation callback";
    sync_event.Wait();

    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
}

// @brief:   测试AsyncReadFrom
TEST_F(FileTest, AsyncReadFrom) {
    AutoResetEvent sync_event;
    char buf[128] = {0};

    uint32_t error_code = 0;
    EXPECT_EQ(0, g_test_file_obj_r->Seek(0, SEEK_SET, &error_code));

    std::string content = "this is a text file,hello worldthis is a text file,hello world";
    if (g_test_file_obj_r->SupportAsync()) {
        Closure<void, int64_t, unsigned int> *callback =
            NewClosure(ReadCallback, &sync_event);

        int64_t start_position = 5;
        EXPECT_EQ(0,
            g_test_file_obj_r->AsyncReadFrom(
                buf, sizeof(buf) - 1, start_position, callback, 1, &error_code));
        VLOG(1) << "AsyncReadFrom returned.";

        VLOG(1) << "to wait async operation callback";
        sync_event.Wait();

        EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
        EXPECT_EQ(content.size() - start_position, strlen(buf));
        VLOG(1) << "read result buf: " << buf;
        EXPECT_EQ(content.substr(start_position), buf);
    }

    if (g_test_file_obj_r->SupportAsync()) {
        Closure<void, int64_t, unsigned int> *callback =
            NewClosure(ReadCallback, &sync_event);
        memset(buf, 0, sizeof(buf));

        // To test read the last byte
        EXPECT_EQ(0,
            g_test_file_obj_r->AsyncReadFrom(
                buf, sizeof(buf) - 1, content.size() - 1, callback, 1, &error_code));
        VLOG(1) << "AsyncReadFrom last byte returned.";

        VLOG(1) << "to wait async operation for last byte callback";
        sync_event.Wait();

        EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
        EXPECT_EQ(1u, strlen(buf));
        EXPECT_EQ(content[content.size() - 1], buf[0]);
    }

    if (g_test_file_obj_r->SupportAsync()) {
        Closure<void, int64_t, unsigned int> *callback =
            NewClosure(ReadCallback, &sync_event);
        memset(buf, 0, sizeof(buf));

        // To test read after the last byte
        EXPECT_EQ(0,
            g_test_file_obj_r->AsyncReadFrom(
                buf, sizeof(buf) - 1, content.size(), callback, 1, &error_code));
        VLOG(1) << "to wait async operation after last byte callback";
        sync_event.Wait();

        EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
        EXPECT_EQ(0u, strlen(buf));
    }
}

TEST_F(FileTest, BinaryWriteTest) {
    uint64_t data = 6124895493223874560ULL;
    uint32_t error_code;
    g_test_file_obj->Seek(0, SEEK_END, &error_code);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);

    VLOG(1) << "to write data " << data;
    int32_t size = g_test_file_obj->Write(
        reinterpret_cast<void*>(&data), sizeof(data), &error_code);
    EXPECT_EQ(static_cast<size_t>(size), sizeof(data));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);

    AutoResetEvent sync_event;
    Closure<void, int64_t, unsigned int> *callback =
        NewClosure(WriteCallback, &sync_event);

    if (g_test_file_obj->SupportAsync()) {
        EXPECT_EQ(0,
            g_test_file_obj->AsyncWrite(reinterpret_cast<void*>(&data),
                sizeof(data), callback, 1, &error_code));
        VLOG(1) << "AsyncWriteFile binary data returned for " << sizeof(data) << " bytes";
    }
    VLOG(1) << "to wait async write operation callback";
    sync_event.Wait();

    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);

    int64_t seek_off = -2 * static_cast<int64_t>(sizeof(data));
    // Not use of static_cast, will cause failure for 32bit.
    // int64_t seek_off = -2 * sizeof(data);
    VLOG(1) << "seek offset " << seek_off;
    g_test_file_obj_r->Seek(seek_off, SEEK_END, &error_code);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    VLOG(1) << "now in pos " << g_test_file_obj_r->Tell();

    uint64_t read_data = 0;
    size = g_test_file_obj_r->Read(
        reinterpret_cast<void*>(&read_data), sizeof(read_data), &error_code);
    VLOG(1) << "read binary data is " << read_data;
    EXPECT_EQ(static_cast<size_t>(size), sizeof(data));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    EXPECT_EQ(data, read_data);

    VLOG(1) << "now in pos " << g_test_file_obj_r->Tell();
    read_data = 0;
    size = g_test_file_obj_r->Read(
        reinterpret_cast<void*>(&read_data), sizeof(read_data), &error_code);
    VLOG(1) << "second time read binary data is " << read_data;
    EXPECT_EQ(static_cast<size_t>(size), sizeof(data));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    EXPECT_EQ(data, read_data);
}

TEST_F(FileTest, LongTextWriteTest) {
    const int64_t kTestStringLen = 512 * 1024;
    char* buf = new char[kTestStringLen + 1];
    for (int i = 0; i < kTestStringLen; ++i) {
        buf[i] = 'a';
    }
    buf[kTestStringLen] = 0;

    for (int i = 0; i < g_write_count; ++i) {
        AutoResetEvent sync_event;
        Closure<void, int64_t, unsigned int> *callback =
            NewClosure(LongWriteCallback, &sync_event, kTestStringLen);

        uint32_t error_code;
        // Don't seek to end before we AsyncWrite.
        // Data will be writen to the next position just as append mode.
        // g_test_file_obj->Seek(0, SEEK_END, &error_code);
        if (g_test_file_obj->SupportAsync()) {
            EXPECT_EQ(0,
                g_test_file_obj->AsyncWrite(buf,
                    kTestStringLen, callback, 1, &error_code));
            VLOG(1) << "AsyncWriteFile returned for " << kTestStringLen << " bytes";
        }
        VLOG(1) << "to wait async write operation callback";
        sync_event.Wait();

        EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    }

    g_test_file_obj_r->Seek(0, SEEK_END);
    int64_t total_size = g_test_file_obj_r->Tell();
    LOG(INFO) << "long text write total size " << total_size;
    EXPECT_TRUE(total_size > g_write_count * kTestStringLen);

    delete []buf;
}

TEST_F(FileTest, AsyncReadFromWrong) {
    AutoResetEvent sync_event;
    char buf[128] = {0};
    Closure<void, int64_t, unsigned int> *callback =
        NewClosure(ReadCallback, &sync_event);

    uint32_t error_code = 0;
    EXPECT_EQ(0, g_test_file_obj_r->Seek(0, SEEK_SET, &error_code));

    if (g_test_file_obj_r->SupportAsync()) {
        EXPECT_EQ(0,
            g_test_file_obj_r->AsyncReadFrom(buf,
                sizeof(buf) - 1, 500000000, callback, 1, &error_code));
        LOG(INFO) << "AsyncReadFrom returned.";
    }

    LOG(INFO) << "to wait async operation callback";
    if (error_code == static_cast<uint32_t>(ERR_FILE_OK)) {
        sync_event.Wait();
    }

    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
}

// @brief:   测试local_file.cc的TellFile
TEST_F(FileTest, TellFile) {
    uint32_t error_code = 0;

    // 先seek到文件中一个位置，再来验证Tell
    EXPECT_EQ(5, g_test_file_obj->Seek(5, SEEK_SET, &error_code));
    EXPECT_EQ(5, g_test_file_obj->Tell(&error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
}

TEST_F(FileTest, WriteWithRetry) {
    uint32_t error_code = 0;
    const int64_t kTestStringLen = 512 * 1024;
    char* buf = new char[kTestStringLen + 1];
    char* read_buf = new char[kTestStringLen + 1];
    for (int i = 0; i < kTestStringLen; ++i) {
        buf[i] = 'a';
        read_buf[i] = 'b';
    }
    buf[kTestStringLen] = 0;
    read_buf[kTestStringLen] = 0;

    g_test_file_obj->Seek(0, SEEK_END, &error_code);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);

    // OK read
    int64_t pos = g_test_file_obj->Tell();
    int64_t writen_size = g_test_file_obj->Write(buf, kTestStringLen, &error_code);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    EXPECT_EQ(writen_size, kTestStringLen);

    g_test_file_obj_r->Seek(pos, SEEK_SET, &error_code);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    g_test_file_obj_r->Read(read_buf, kTestStringLen, &error_code);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);

    EXPECT_EQ(strcmp(buf, read_buf), 0);

    // Wrong read
    writen_size = g_test_file_obj->Read(buf, 0, &error_code);
    EXPECT_EQ(static_cast<uint32_t>(static_cast<uint32_t>(ERR_FILE_PARAMETER)), error_code);
    EXPECT_EQ(-1, writen_size);

    delete []buf;
    delete []read_buf;
}

// @brief:   测试local_file.cc的Truncate
TEST_F(FileTest, Truncate) {
    uint32_t error_code;
    // 之前的数据都还在缓冲里，得flush到文件中
    g_test_file_obj->Flush(&error_code);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);

    int64_t file_size = File::GetSize(g_test_file_name.c_str(), &error_code);
    int64_t new_size;
    if (file_size > 5) {
        new_size = 5;
    } else {
        new_size = 0;
    }
    EXPECT_EQ(0, g_test_file_obj->Truncate(new_size, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);

    file_size = File::GetSize(g_test_file_name.c_str(), &error_code);
    EXPECT_EQ(new_size, file_size);
    g_test_file_obj->Flush(&error_code);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
}

TEST_F(FileTest, Close) {
    uint32_t error_code = 0;

    EXPECT_EQ(0, g_test_file_obj->Close(&error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);

    EXPECT_EQ(0, g_test_file_obj_r->Close(&error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    // Must delete the created File object.
    delete g_test_file_obj;
    delete g_test_file_obj_r;
    g_test_file_obj = NULL;
    g_test_file_obj_r = NULL;
}

// @brief:   测试local_file.cc的CheckExist
TEST_F(FileTest, CheckExist) {
    uint32_t error_code = 0;
    const char* file_name = g_test_file_name.c_str();

    EXPECT_TRUE(File::CheckExist(file_name, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
}

// @brief:   测试local_file.cc的List
TEST_F(FileTest, List) {
    uint32_t error_code;

    std::vector<AttrsInfo> file_info;
    ListOptions list_option;
    list_option.mask.file_size = 1;
    EXPECT_EQ(0, File::List(g_test_dir_name.c_str(), list_option, &file_info, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    EXPECT_EQ(1u, file_info.size());
    EXPECT_EQ("file_test.txt", file_info[0].file_name);
    EXPECT_EQ(5U, file_info[0].file_size);

    VLOG(1) << "input list name: " << g_test_dir_name;
    std::vector<AttrsInfo>::iterator iter;
    for (iter = file_info.begin(); iter != file_info.end(); ++iter) {
        VLOG(1) << iter->file_name;
    }

    EXPECT_EQ(0, File::List(g_test_file_name.c_str(), list_option, &file_info, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    VLOG(1) << "input list name: " << g_test_file_name;
    for (iter = file_info.begin(); iter != file_info.end(); ++iter) {
        VLOG(1) << iter->file_name;
    }
    EXPECT_EQ(1u, file_info.size());
    EXPECT_EQ("file_test.txt", file_info[0].file_name);
}

TEST_F(FileTest, ListEntryItself) {
    uint32_t error_code;

    std::vector<AttrsInfo> file_info;
    ListOptions list_option;
    list_option.mask.file_size = 1;
    list_option.mask.stat_info = 1; // list dir itself.
    EXPECT_EQ(0, File::List(g_test_dir_name.c_str(), list_option, &file_info, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    EXPECT_EQ(1u, file_info.size());
    EXPECT_EQ(g_test_dir_name, file_info[0].file_name);

    VLOG(1) << "input list name: " << g_test_dir_name;
    std::vector<AttrsInfo>::iterator iter;
    for (iter = file_info.begin(); iter != file_info.end(); ++iter) {
        LOG(INFO) << iter->file_name;
    }

    EXPECT_EQ(0, File::List(g_test_file_name.c_str(), list_option, &file_info, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    VLOG(1) << "input list name: " << g_test_file_name;
    for (iter = file_info.begin(); iter != file_info.end(); ++iter) {
        VLOG(1) << iter->file_name;
    }
    EXPECT_EQ(1u, file_info.size());
    EXPECT_EQ(g_test_file_name, file_info[0].file_name);
}

// @brief:   测试local_file.cc的List
TEST_F(FileTest, Du) {
    uint32_t error_code;
    int64_t ret = File::Du(g_test_file_name.c_str(), &error_code);
    EXPECT_EQ(5, ret);
}

// @brief:   测试local_file.cc的Rename
TEST_F(FileTest, Rename) {
    uint32_t error_code;

    // rename it.
    std::string new_name = g_test_file_name + ".new";
    EXPECT_EQ(0, File::Rename(g_test_file_name.c_str(), new_name.c_str(), &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    EXPECT_TRUE(File::CheckExist(new_name.c_str(), &error_code));
    EXPECT_FALSE(File::CheckExist(g_test_file_name.c_str(), &error_code));

    EXPECT_EQ(0, File::Rename(new_name.c_str(), g_test_file_name.c_str(), &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    EXPECT_FALSE(File::CheckExist(new_name.c_str(), &error_code));
    EXPECT_TRUE(File::CheckExist(g_test_file_name.c_str(), &error_code));

    EXPECT_EQ(-1, File::Rename(g_test_file_name.c_str(), g_test_file_name.c_str(), &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_PARAMETER), error_code);
}

TEST_F(FileTest, CopyFiles) {
    uint32_t error_code;
    ASSERT_EQ(-1, File::Copy("", "", &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_PARAMETER), error_code);
    ASSERT_EQ(-1, File::Copy(g_test_file_name.c_str(), g_test_file_name.c_str(), &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_PARAMETER), error_code);
}

TEST_F(FileTest, ListWithPattern) {
    uint32_t error_code;
    bool is_recursive = true;
    std::string work_dir("./tmp/");
    File::Remove(work_dir.c_str(), is_recursive, &error_code);

    ASSERT_EQ(0, File::AddDir(work_dir.c_str(), &error_code));
    ASSERT_EQ(0,
        File::Copy(g_test_file_name.c_str(), (work_dir + "file1.txt").c_str(), &error_code));
    ASSERT_EQ(0,
        File::Copy(g_test_file_name.c_str(), (work_dir + "file2.txt").c_str(), &error_code));
    ASSERT_EQ(0,
        File::Copy(g_test_file_name.c_str(), (work_dir + "file3.txt").c_str(), &error_code));

    std::vector<AttrsInfo> files;
    ListOptions list_option;
    EXPECT_EQ(0, File::List("./tmp/file*.txt", list_option, &files, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    EXPECT_EQ(3u, files.size());
    LOG(INFO) << "matched file count " << files.size();

    EXPECT_EQ(0, File::List("./tmp/file1.txt", list_option, &files, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    EXPECT_EQ(1u, files.size());

    EXPECT_EQ(0, File::List("./tmp/file1*.txt", list_option, &files, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    EXPECT_EQ(1u, files.size());

    EXPECT_EQ(0, File::List("./tmp/*", list_option, &files, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    EXPECT_EQ(3u, files.size());

    EXPECT_EQ(0, File::List("*", list_option, &files, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    LOG(INFO) << "matched file count for *: " << files.size();
    size_t count = files.size();

    EXPECT_EQ(0, File::List(".", list_option, &files, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    LOG(INFO) << "matched file count for .: " << files.size();
    EXPECT_EQ(count, files.size());

    EXPECT_EQ(-1, File::List("./tmp*/file1.txt", list_option, &files, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_PARAMETER), error_code);

    // clean up
    EXPECT_EQ(0, File::Remove(work_dir.c_str(), is_recursive, &error_code));
}

TEST_F(FileTest, GetMatchingFiles) {
    uint32_t error_code;
    bool is_recursive = true;
    std::string work_dir("./tmp/");
    File::Remove(work_dir.c_str(), is_recursive, &error_code);

    ASSERT_EQ(0, File::AddDir(work_dir.c_str(), &error_code));
    ASSERT_EQ(0,
        File::Copy(g_test_file_name.c_str(), (work_dir + "file1.txt").c_str(), &error_code));
    ASSERT_EQ(0,
        File::Copy(g_test_file_name.c_str(), (work_dir + "file2.txt").c_str(), &error_code));
    ASSERT_EQ(0,
        File::Copy(g_test_file_name.c_str(), (work_dir + "file3.txt").c_str(), &error_code));

    const char* pattern = "./tmp/fil?[123].*";
    std::vector<std::string> files;
    EXPECT_EQ(0, File::GetMatchingFiles(pattern, &files));
    EXPECT_EQ(3u, files.size());

    // clean up
    EXPECT_EQ(0, File::Remove(work_dir.c_str(), is_recursive, &error_code));
}

TEST_F(FileTest, Move) {
    uint32_t error_code;
    // Move: file to file.
    LOG(INFO) << "test move files to files";
    EXPECT_TRUE(File::CheckExist(g_test_file_name.c_str(), &error_code));
    const std::string new_test_file = g_test_file_name + ".move";
    EXPECT_EQ(0, File::Move(g_test_file_name.c_str(), new_test_file.c_str(), &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    EXPECT_FALSE(File::CheckExist(g_test_file_name.c_str(), &error_code));
    // move back
    EXPECT_EQ(0, File::Move(new_test_file.c_str(), g_test_file_name.c_str(), &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    EXPECT_TRUE(File::CheckExist(g_test_file_name.c_str(), &error_code));
    // move non-exist files
    EXPECT_EQ(-1, File::Move(new_test_file.c_str(), g_test_file_name.c_str(), &error_code));

    // Move: file to folder.
    LOG(INFO) << "test move files to folders";
    const std::string new_dir = g_test_dir_name + "_new";
    if (!File::CheckExist(new_dir.c_str())) {
        EXPECT_EQ(0, File::AddDir(new_dir.c_str()));
    }
    const std::string new_file = new_dir + File::kPathSeparator + "newfile";
    EXPECT_EQ(0, File::Copy(g_test_file_name.c_str(), new_file.c_str()));
    EXPECT_TRUE(File::CheckExist(new_file.c_str()));

    LOG(INFO) << "to move file " << new_file << " to dir " << g_test_dir_name;
    int ret = File::Move(new_file.c_str(), g_test_dir_name.c_str(), &error_code);
    EXPECT_EQ(0, ret);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    const std::string new_file_moved = g_test_dir_name + File::kPathSeparator + "newfile";
    EXPECT_TRUE(File::CheckExist(new_file_moved.c_str()));

    // Move: folder to folder.
    LOG(INFO) << "move from folder to folder";
    const std::string new_file2 = new_dir + File::kPathSeparator + "newfile2";
    EXPECT_EQ(0, File::Copy(g_test_file_name.c_str(), new_file2.c_str()));
    EXPECT_TRUE(File::CheckExist(new_file2.c_str()));

    const std::string new_file3 = new_dir + File::kPathSeparator + "newfile3";
    EXPECT_EQ(0, File::Copy(g_test_file_name.c_str(), new_file3.c_str()));
    EXPECT_TRUE(File::CheckExist(new_file3.c_str()));

    const std::string sub_dir = new_dir + File::kPathSeparator + "dir";
    if (!File::CheckExist(sub_dir.c_str())) {
        EXPECT_EQ(0, File::AddDir(sub_dir.c_str()));
    }
    const std::string new_file4 = sub_dir + File::kPathSeparator + "newfile4";
    EXPECT_EQ(0, File::Copy(g_test_file_name.c_str(), new_file4.c_str()));
    EXPECT_TRUE(File::CheckExist(new_file4.c_str()));

    // Move the whole folders recursively.
    EXPECT_EQ(0, File::Move(new_dir.c_str(), g_test_dir_name.c_str(), &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);

    // Check move results.
    const std::string moved_file2 = g_test_dir_name + File::kPathSeparator + "newfile2";
    EXPECT_TRUE(File::CheckExist(moved_file2.c_str()));
    EXPECT_FALSE(File::CheckExist(new_file2.c_str()));

    const std::string moved_file3 = g_test_dir_name + File::kPathSeparator + "newfile3";
    EXPECT_TRUE(File::CheckExist(moved_file3.c_str()));
    EXPECT_FALSE(File::CheckExist(new_file3.c_str()));

    const std::string moved_file4 =
        g_test_dir_name + File::kPathSeparator + "dir" + File::kPathSeparator + "newfile4";
    EXPECT_TRUE(File::CheckExist(moved_file4.c_str()));
    EXPECT_FALSE(File::CheckExist(new_file4.c_str()));

    // Move file to allow overwrite an exist file
    EXPECT_EQ(0, File::Move(moved_file3.c_str(), moved_file4.c_str(), &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    EXPECT_TRUE(File::CheckExist(moved_file4.c_str()));
    EXPECT_FALSE(File::CheckExist(moved_file3.c_str()));
}

// @brief:   测试local_file.cc的Remove

TEST_F(FileTest, Remove) {
    uint32_t error_code;
    EXPECT_TRUE(File::CheckExist(g_test_file_name.c_str(), &error_code));
    EXPECT_EQ(0, File::Remove(g_test_file_name.c_str(), false, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    EXPECT_FALSE(File::CheckExist(g_test_file_name.c_str(), &error_code));

    bool is_recursive = true;
    // 删除前
    EXPECT_TRUE(File::CheckExist(g_test_dir_name.c_str(), &error_code));
    EXPECT_EQ(0, File::Remove(g_test_dir_name.c_str(), is_recursive, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    // 删除后
    EXPECT_FALSE(File::CheckExist(g_test_dir_name.c_str(), &error_code));
}


TEST_F(FileTest, AsyncAppendFile) {
    int open_mode = File::ENUM_FILE_OPEN_MODE_A;

    const char* file_name = "test_append";
    // first remove, so that to setup a clean env.
    File::Remove(file_name, false, NULL);

    uint32_t error_code = 0;
    scoped_ptr<File> file(File::Open(file_name, open_mode, OpenFileOptions(), &error_code));
    EXPECT_TRUE(file.get() != NULL);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);

    open_mode = File::ENUM_FILE_OPEN_MODE_R;
    scoped_ptr<File> file_r(File::Open(file_name, open_mode, OpenFileOptions(), &error_code));
    EXPECT_TRUE(file_r != NULL);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);

    const char *test_string = "this is a text file,hello world";
    std::string buf_str(test_string);
    const char* buf = buf_str.c_str();
    const int64_t test_string_len = strlen(test_string) + 1;

    AutoResetEvent sync_event_write1;
    AutoResetEvent sync_event_write2;
    Closure<void, int64_t, unsigned int> *callback1 =
        NewPermanentClosure(WriteCallback, &sync_event_write1);
    Closure<void, int64_t, unsigned int> *callback2 =
        NewPermanentClosure(WriteCallback, &sync_event_write2);
    if (file->SupportAsync()) {
        EXPECT_EQ(0,
            file->AsyncWrite(buf, test_string_len, callback1, 1, &error_code));
        EXPECT_EQ(0,
            file->AsyncWrite(buf, test_string_len, callback2, 1, &error_code));
    }
    LOG(INFO) << "sleep to wait async operation callback";
    sync_event_write1.Wait();
    sync_event_write2.Wait();

    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    delete callback1;
    delete callback2;

    LOG(INFO) << "to read after append";
    AutoResetEvent sync_event;
    Closure<void, int64_t, unsigned int> *read_callback =
        NewPermanentClosure(ReadCallback, &sync_event);

    // Read data to verify
    char* read_buf = new char[test_string_len * 2];
    if (file_r->SupportAsync()) {
        EXPECT_EQ(0,
            file_r->AsyncReadFrom(
                read_buf, test_string_len * 2, 0, read_callback, 1, &error_code));
    }
    LOG(INFO) << "to wait async operation callback";
    sync_event.Wait();
    LOG(INFO) << "to async operation callback OK";

    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);

    delete read_callback;
    delete []read_buf;

    File::Remove(file_name, false, NULL);
}

TEST_F(FileTest, GetPrefix) {
    std::string file_name = "/xfs/sz/test4Options.dat:r=5:bufsz=1024";
    std::string prefix = File::GetFilePrefix(file_name.c_str());
    EXPECT_EQ("/xfs/", prefix);
}

TEST_F(FileTest, TestOpen) {
    std::string file_name = "./test_open.dat";
    std::string test_str1 = "0123456789";
    std::string test_str2 = "987";
    int64_t total_size = test_str1.size() + test_str2.size();
    char buffer[512] = {0};
    CHECK_GT(sizeof(buffer), static_cast<size_t>(total_size));

    scoped_ptr<File> file_obj(File::Open(file_name, "w"));
    file_obj->Write(test_str1.c_str(), test_str1.size());
    file_obj->Close(); // elegant usage: matched open and close.

    file_obj.reset(File::Open(file_name, "r"));
    EXPECT_EQ(static_cast<int64_t>(test_str1.size()), file_obj->Read(buffer, sizeof(buffer)));
    // no close, should close in dtor.

    file_obj.reset(File::Open(file_name, "a"));
    file_obj->Write("987", test_str2.size());

    file_obj.reset(File::Open(file_name, "r"));
    EXPECT_EQ(total_size, file_obj->Read(buffer, sizeof(buffer)));
    file_obj->Close();
    file_obj->Close(); // allow call twice.

    file_obj.reset(File::Open(file_name, "r"));
    EXPECT_EQ(total_size, file_obj->Read(buffer, sizeof(buffer)));
}

TEST_F(FileTest, TestReadLine) {
    std::string file_name = "./test_open.dat";
    // TODO(phongchen): It introduces dependence to reuse file between test cases.
    // Need to initialize file in test fixture.
    File* file_obj = File::Open(file_name, "a");

    file_obj->Write("\n", 1); // end for line 1
    file_obj->Write("line2\n", 6); // line 2
    file_obj->Write("\n", 1); // empty line 3
    file_obj->Write("line4", 5); // line 4, no \n at the end.

    file_obj->Close();
    delete file_obj;

    file_obj = File::Open(file_name, "r");
    const int32_t max_size = 30;
    char* buf = new char[max_size];

    int32_t read_size = 0;
    read_size = file_obj->ReadLine(buf, 4);
    EXPECT_EQ(3, read_size); // less than a line.
    EXPECT_STREQ("012", buf);

    file_obj->Seek(0, SEEK_SET);
    read_size = file_obj->ReadLine(buf, max_size);
    LOG(INFO) << "readed: " << buf;
    EXPECT_EQ(13 + 1, read_size); // line with \n
    EXPECT_STREQ("0123456789987\n", buf);

    read_size = file_obj->ReadLine(buf, max_size);
    EXPECT_EQ(6, read_size); // line with \n
    EXPECT_STREQ("line2\n", buf);

    read_size = file_obj->ReadLine(buf, max_size);
    EXPECT_EQ(1, read_size); // empty line with \n
    EXPECT_STREQ("\n", buf);

    read_size = file_obj->ReadLine(buf, max_size);
    EXPECT_EQ(5, read_size); // no \n
    EXPECT_STREQ("line4", buf);

    read_size = file_obj->ReadLine(buf, max_size);
    EXPECT_EQ(0, read_size); // EOF

    file_obj->Close();
    delete file_obj;

    delete[] buf;
    buf = NULL;
}

TEST_F(FileTest, TestReadLineForString) {
    std::string file_name = "./test_open.dat";
    File* file_obj = File::Open(file_name, "r");
    std::string buf;

    int32_t read_size = 0;
    file_obj->ReadLine(&buf);
    EXPECT_EQ("0123456789987\n", buf);

    file_obj->ReadLine(&buf);
    EXPECT_EQ("line2\n", buf);

    file_obj->ReadLine(&buf);
    EXPECT_EQ("\n", buf);

    file_obj->ReadLine(&buf);
    EXPECT_EQ("line4", buf);

    read_size = file_obj->ReadLine(&buf);
    EXPECT_EQ(0, read_size); // EOF

    file_obj->Close();
    delete file_obj;
}

TEST_F(FileTest, TestReadLineForStringOnEmptyFile) {
    std::string file_name = "./empty.dat";
    File* file_obj = File::Open(file_name, "w");
    file_obj->Close();
    delete file_obj;

    file_obj = File::Open(file_name, "r");
    std::string buf;

    int32_t read_size = 0;

    read_size = file_obj->ReadLine(&buf);
    EXPECT_EQ(0, read_size); // EOF

    file_obj->Close();
    delete file_obj;
}

TEST_F(FileTest, TestReadLineForStringOnOneLineFile) {
    std::string file_name = "./oneline.dat";
    File* file_obj = File::Open(file_name, "w");
    file_obj->Write("abc", 3);  // One line without endl
    file_obj->Close();
    delete file_obj;

    file_obj = File::Open(file_name, "r");
    std::string buf;

    int32_t read_size = 0;

    read_size = file_obj->ReadLine(&buf);
    EXPECT_EQ(3, read_size);
    EXPECT_EQ("abc", buf);

    file_obj->Close();
    delete file_obj;
}

TEST_F(FileTest, TestReadLineForStringContainsNullChar) {
    std::string file_name = "./oneline.dat";
    scoped_ptr<File> fp(File::Open(file_name, "w"));

    std::string data("hello\0world\0\n", 13);

    static const int kLoopCount = 10;

    for (int i = 0; i < kLoopCount; ++i) {
        fp->Write(data.data(), data.size());
    }

    fp.reset(File::Open(file_name, "r"));

    std::string buf;

    for (int i = 0; i < kLoopCount; ++i) {
        EXPECT_EQ(static_cast<int32_t>(data.size()), fp->ReadLine(&buf));
        EXPECT_EQ(data, buf);
    }

    EXPECT_EQ(0, fp->ReadLine(&buf));
}

TEST_F(FileTest, TestLoadToString) {
    std::string file_name = "./test_open.dat";
    std::string buf;

    File::LoadToString(file_name, &buf);

    EXPECT_EQ(26u, buf.length());
}

TEST_F(FileTest, GetMode) {
    {
        EXPECT_EQ(0, File::AddDir(g_test_dir_name));
        scoped_ptr<File> file(File::Open(g_test_file_name, "w"));
    }

    uint32_t permission;
    ASSERT_TRUE(File::GetMode(g_test_file_name, &permission));
    EXPECT_NE(0U, permission);

    ASSERT_TRUE(File::Chmod(g_test_file_name, 0777));
    ASSERT_TRUE(File::GetMode(g_test_file_name, &permission));
    EXPECT_EQ(0777U, permission);

    ASSERT_TRUE(File::Chmod(g_test_file_name, 0666));
    ASSERT_TRUE(File::GetMode(g_test_file_name, &permission));
    EXPECT_EQ(0666U, permission);
}

TEST_F(FileTest, GetAttrs) {
    std::string dir_name = "test_get_attrs";
    std::string file_name = "test_get_attrs";
    std::string full_file_name = dir_name + File::kPathSeparator + file_name;

    File::Remove(dir_name, true, NULL);
    EXPECT_EQ(0, File::AddDir(dir_name));
    scoped_ptr<File> file(File::Open(full_file_name, "w"));

    AttrsMask mask;
    mask.file_permission = true;
    mask.file_type = true;
    AttrsInfo attrs_info;
    ASSERT_TRUE(File::GetAttrs(full_file_name, mask, &attrs_info));
    EXPECT_NE(0U, attrs_info.file_permission);
    EXPECT_EQ(FILE_TYPE_NORMAL, attrs_info.file_type);
    EXPECT_EQ(full_file_name, attrs_info.file_name);

    ASSERT_TRUE(File::Chmod(full_file_name, 0666));
    ASSERT_TRUE(File::GetAttrs(full_file_name, mask, &attrs_info));
    EXPECT_EQ(0666U, attrs_info.file_permission);
}

TEST_F(FileTest, Chmod) {
    std::string dir_name = std::string("") + "test_chmod_dir";
    std::string file_name = dir_name + File::kPathSeparator + "test_chmod_file";
    std::string file_not_exist_name = dir_name + File::kPathSeparator + "test_chmod_no_exist_file";
    // first remove, so that to setup a clean env.
    File::Remove(dir_name.c_str(), true, NULL);

    uint32_t error_code = 0;
    int open_mode = File::ENUM_FILE_OPEN_MODE_W;
    ASSERT_EQ(0, File::AddDir(dir_name.c_str(), &error_code));
    ASSERT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    File* file = File::Open(file_name.c_str(), open_mode, OpenFileOptions(), &error_code);
    EXPECT_TRUE(file != NULL);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    EXPECT_EQ(0, file->Close(&error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    delete file;
    file = NULL;

    uint32_t permission = 0700u;
    EXPECT_TRUE(File::Chmod(dir_name.c_str(), permission, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    file = File::Open(file_name.c_str(), open_mode, OpenFileOptions(), &error_code);
    EXPECT_TRUE(file != NULL);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    file->Close();
    delete file;

    permission = 01777u;
    EXPECT_FALSE(File::Chmod(file_name.c_str(), permission, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_PARAMETER), error_code);

    permission = 0400u; // readonly
    EXPECT_TRUE(File::Chmod(file_name.c_str(), permission, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    // Try to write, so it will fail due to permission denied.
    file = File::Open(file_name.c_str(), open_mode, OpenFileOptions(), &error_code);
    EXPECT_TRUE(file == NULL);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_PERMISSION_DENIED), error_code);

    permission = 0777u;
    EXPECT_TRUE(File::Chmod(file_name.c_str(), permission, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    file = File::Open(file_name.c_str(), open_mode, OpenFileOptions(), &error_code);
    EXPECT_TRUE(file != NULL);
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    file->Close();
    delete file;

    EXPECT_FALSE(File::Chmod(file_not_exist_name.c_str(), permission, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_ENTRY_NOT_EXIST), error_code);

    EXPECT_EQ(0, File::Remove(dir_name.c_str(), true, NULL));
}

std::string GetCurrentUserInfo()
{
    char buf[1024];
    struct passwd pwd;
    struct passwd* ppwd = NULL;
    EXPECT_EQ(0, getpwuid_r(getuid(), &pwd, buf, sizeof(buf), &ppwd));
    EXPECT_TRUE(ppwd != NULL);
    struct group group;
    struct group* pgroup = NULL;
    EXPECT_EQ(0, getgrgid_r(getgid(), &group, buf, sizeof(buf), &pgroup));
    EXPECT_TRUE(pgroup != NULL);
    std::string role_name;
    role_name.append(ppwd->pw_name);
    role_name.append(":");
    role_name.append(pgroup->gr_name);
    return role_name;
}

TEST_F(FileTest, ChangeRole) {
    std::string role_name = GetCurrentUserInfo();
    EXPECT_FALSE(File::ChangeRole("./test.dat", role_name.c_str()));
    EXPECT_DEATH(File::ChangeSecondRole("./test.dat", role_name.c_str()), "");
}

TEST_F(FileTest, ChangeSecondRoleDeathTest) {
    std::string role_name = GetCurrentUserInfo();
    EXPECT_DEATH(File::ChangeSecondRole("./test.dat", role_name.c_str()), "");
}

TEST_F(FileTest, TestInitCleanup) {
    File::CleanUp(); // with first Init.

    EXPECT_TRUE(File::Init());
    EXPECT_TRUE(File::Init());
    File::CleanUp();
    File::CleanUp();

    EXPECT_TRUE(File::Init());
    File::CleanUp();

    EXPECT_TRUE(File::Init()); // with final cleanup.
}

TEST_F(FileTest, TestGetDigest) {
    const std::string file_name = "./digest.dat";
    // clean
    File::Remove(file_name.c_str(), false);

    File* file_obj = File::Open(file_name, "w");
    EXPECT_EQ(7, file_obj->Write("abcdefg", 7));
    file_obj->Close();
    delete file_obj;

    uint32_t digest = 0;
    EXPECT_TRUE(File::GetDigest(file_name.c_str(), &digest));
    EXPECT_EQ(19628025u, digest);
}

TEST_F(FileTest, ReadAfterWrite)
{
    uint32_t error_code = 0;
    const std::string file_name = "ReadAfterWrite.txt";
    const std::string line = "hello world\n";
    std::string content;
    const int loop = 3;

    // Write file.
    scoped_ptr<File> file(File::Open(
        file_name.c_str(), File::ENUM_FILE_OPEN_MODE_W, OpenFileOptions(), &error_code));
    for (int i = 0; i < loop; ++i) {
        content += line;
        int64_t ret = file->Write(line.c_str(), line.size(), &error_code);
        ASSERT_GT(ret, 0);
        ASSERT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
        int32_t ret2 = file->Flush(&error_code);
        ASSERT_EQ(ret2, 0);
        ASSERT_EQ(static_cast<uint32_t>(ERR_FILE_OK), error_code);
    }

    // Read file.
    scoped_ptr<File> file_for_read(File::Open(
        file_name.c_str(), File::ENUM_FILE_OPEN_MODE_R, OpenFileOptions(), &error_code));
    std::string result;
    ASSERT_TRUE(File::LoadToString(file_name, &result));

    // Verify.
    EXPECT_EQ(content, result);

    // Clearn up.
    file->Close();
    file_for_read->Close();
}

// TODO(bradzhang): refine this ut when local_file implements the snapshot virtual function
TEST_F(FileTest, Snapshot)
{
    uint32_t error_code = 0;
    const std::string source_file_name = "source_file_name.txt";
    const std::string target_file_name = "source_file_name.txt.snapshot";

    EXPECT_EQ(-1, File::Snapshot(source_file_name.c_str(), target_file_name.c_str(), &error_code));
}

// TODO(bradzhang): refine this ut when local_file implements the freeze virtual function
TEST_F(FileTest, Freeze)
{
    uint32_t error_code = 0;
    std::string file_name = "file_name.txt";

    EXPECT_FALSE(File::Freeze(file_name, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_FUNCTION_NO_IMPLEMENTATION), error_code);

    file_name = "";
    EXPECT_FALSE(File::Freeze(file_name, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_PARAMETER), error_code);
}

TEST_F(FileTest, SetReplication)
{
    uint32_t error_code = 0;
    std::string file_name = "file_name.txt";

    EXPECT_FALSE(File::SetReplication(file_name, 0, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_FUNCTION_NO_IMPLEMENTATION), error_code);

    file_name = "";
    EXPECT_FALSE(File::SetReplication(file_name, 0, &error_code));
    EXPECT_EQ(static_cast<uint32_t>(ERR_FILE_PARAMETER), error_code);
}

TEST_F(FileTest, GetContentSummaryDeathTest) {
    FileContentSummaryMask mask;
    FileContentSummary summary;
    EXPECT_DEATH(File::GetContentSummary("./test.dat", &mask, &summary), "");
}

TEST_F(FileTest, LocateDataDeathTest) {
    int open_mode = File::ENUM_FILE_OPEN_MODE_W;
    uint32_t error_code = 0;
    const char* open_file_name = "./test.dat";
    OpenFileOptions options;
    File* file = File::Open(open_file_name, open_mode, options, &error_code);
    ASSERT_TRUE(file != NULL);
    std::vector<DataLocation> buffer;
    EXPECT_EQ(-1, file->LocateData(0, 64, &buffer));
    delete file;
    File::Remove(open_file_name, false, NULL);
}

TEST_F(FileTest, OpenOrDieDeathTest) {
    const char* file_name = "./test.dat";
    File::Remove(file_name, false, NULL);
    int open_mode = File::ENUM_FILE_OPEN_MODE_R;
    OpenFileOptions options;
    EXPECT_DEATH(File::OpenOrDie(file_name, open_mode, options), "");
}

TEST_F(FileTest, OpenDirAsFile)
{
    uint32_t error_code;
    scoped_ptr<File> fp(File::Open("/bin", "wb", &error_code));
    ASSERT_FALSE(fp);
    ASSERT_EQ(static_cast<uint32_t>(ERR_FILE_EXPECTED), error_code);
}

} // namespace common

