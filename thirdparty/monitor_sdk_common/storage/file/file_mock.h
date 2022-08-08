// Copyright (c) 2015, Vivo Inc. All rights reserved.
/**
 * @file file_mock.h
 * @brief
 * @author chenzheng05@baidu.com
 * @date 2015-01-23
 */
#ifndef COMMON_FILE_FILE_MOCK_H
#define COMMON_FILE_FILE_MOCK_H

#include "thirdparty/monitor_sdk_common/storage/file/local_file2.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/gtest/gtest.h"

// Export the new gmock based file mock2 interface to this file_mock.h, this
// old implement will be deprecated in the future.
#include "thirdparty/monitor_sdk_common/storage/file/file_mock2.h"

DECLARE_bool(filemock_use_filemock);
DECLARE_int32(filemock_read_mock);
DECLARE_int32(filemock_get_size_zero);
DECLARE_int32(filemock_read_len_zero);
DECLARE_int32(filemock_write_mock);
DECLARE_int32(filemock_readline_mock);
DECLARE_int32(filemock_flush_mock);
DECLARE_int32(filemock_close_mock);
DECLARE_int32(filemock_seek_mock);
DECLARE_int32(filemock_tell_mock);
DECLARE_int32(filemock_data_read);
DECLARE_int32(filemock_file_exist);

namespace common {

const char MOCK_FILE_PREFIX[] = "mock/";

class FileMock : public LocalFile
{
    FRIEND_TEST(FileMockTest, FileMock_Fail);
public:
    FileMock() : m_read_buffer(NULL), m_read_buffer_len(0) {}
    virtual int64_t Read(void* buffer, int64_t size, uint32_t* error_code = NULL);
    virtual int64_t Write(const void* buffer, int64_t size, uint32_t* error_code);
    virtual int32_t ReadLine(void* buffer, int32_t max_size);
    virtual int32_t Flush(uint32_t* error_code);
    virtual int32_t Close(uint32_t* error_code);

    virtual int64_t Seek(int64_t offset, int32_t whence, uint32_t* error_code);
    virtual int64_t Tell(uint32_t* error_code);

    virtual int32_t AsyncWrite(const void* buf, int64_t buf_size,
                               Closure<void, int64_t, uint32_t>* callback,
                               uint32_t time_out = kDefaultAsyncTimeout,
                               uint32_t* error_code = NULL);

    virtual int32_t AsyncReadFrom(void* buffer,
                                  int64_t size,
                                  int64_t start_position,
                                  Closure<void, int64_t, uint32_t>* callback,
                                  uint32_t  timeout = kDefaultAsyncTimeout,
                                  uint32_t* error_code = NULL);
    bool InitImpl() {return true;}
    bool CleanupImpl() {return true;}
protected:
    virtual bool OpenImpl(const char *file_path, uint32_t flags,
                          const OpenFileOptions& options,
                          uint32_t *error_code);
    virtual int64_t GetSizeImpl(const char* file_name,
                                uint32_t* error_code);

    virtual bool CheckExistImpl(const char* path_name,
                        uint32_t* error_code);

    void ReadCompleteCallBackMock(Closure<void, int64_t, uint32_t>* out_callback,
                                int64_t readed_len,
                                uint32_t callback_status);

    void WriteCompleteCallBackMock(Closure<void, int64_t, uint32_t>* out_callback,
                                int64_t callback_len,
                                uint32_t callback_status);
private:
    void*      m_read_buffer;
    uint32_t   m_read_buffer_len;
};

} // namespace common


#endif // COMMON_FILE_FILE_MOCK_H
