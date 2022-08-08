// Copyright (c) 2015, Vivo Inc. All rights reserved.
/**
 * @file file_mock.cpp
 * @brief
 * @author chenzheng05@baidu.com
 * @date 2015-01-23
 */

#include <string>

#include "thirdparty/monitor_sdk_common/storage/file/file_mock.h"

DEFINE_bool(filemock_use_filemock, false, "mockfile for test!");

DEFINE_int32(filemock_read_mock, 0x7fffffff, "mock for open and read");
DEFINE_int32(filemock_get_size_zero, 0x7fffffff, "mock for get size return ok, but size is zero");
DEFINE_int32(filemock_read_len_zero, 0x7fffffff, "mock for read return ok, but read len is zero");
DEFINE_int32(filemock_write_mock, 0x7fffffff, "mock for write");
DEFINE_int32(filemock_readline_mock, 0x7fffffff, "mock for readline");
DEFINE_int32(filemock_flush_mock, 0x7fffffff, "mock for Flush");
DEFINE_int32(filemock_close_mock, 0x7fffffff, "mock foe close");
DEFINE_int32(filemock_seek_mock, 0x7fffffff, "mock for Seek");
DEFINE_int32(filemock_tell_mock, 0x7fffffff, "mock for tell");
DEFINE_int32(filemock_return_value, -1, "default return value");
DEFINE_int32(filemock_data_read, 0x7fffffff, "mock for data read buf");
DEFINE_int32(filemock_file_exist, 0x7fffffff, "mock for file exist");

namespace common {

REGISTER_FILE_IMPL(MOCK_FILE_PREFIX, FileMock);


int64_t FileMock::Read(void* buffer, int64_t size, uint32_t* error_code)
{
    if (--FLAGS_filemock_read_mock < 0)
    {
        FLAGS_filemock_read_mock = 0x7fffffff;
        SetErrorCode(error_code, static_cast<common::FILE_ERROR_CODE>(FLAGS_filemock_return_value));
        return -1;
    }
    else
    {
        return  LocalFile::Read(buffer, size, error_code);
    }
}

int64_t FileMock::Write(const void* buffer, int64_t size, uint32_t* error_code)
{
    if (--FLAGS_filemock_write_mock < 0)
    {
        FLAGS_filemock_write_mock = 0x7fffffff;
        SetErrorCode(error_code, static_cast<common::FILE_ERROR_CODE>(FLAGS_filemock_return_value));
        return -1;
    }
    else
    {
        return LocalFile::Write(buffer, size, error_code);
    }
}

int32_t FileMock::ReadLine(void* buffer, int32_t max_size)
{
    if (--FLAGS_filemock_readline_mock < 0)
    {
        FLAGS_filemock_readline_mock = 0x7fffffff;
        return -1;
    }
    else
    {
        return LocalFile::ReadLine(buffer, max_size);
    }
}

int32_t FileMock::Flush(uint32_t* error_code)
{
    if (--FLAGS_filemock_flush_mock < 0)
    {
        FLAGS_filemock_flush_mock = 0x7fffffff;
        SetErrorCode(error_code, static_cast<common::FILE_ERROR_CODE>(FLAGS_filemock_return_value));
        return -1;
    }
    else
    {
        return LocalFile::Flush(error_code);
    }
}

int32_t FileMock:: Close(uint32_t* error_code)
{
    if (--FLAGS_filemock_close_mock < 0)
    {
        FLAGS_filemock_close_mock = 0x7fffffff;
        SetErrorCode(error_code, static_cast<common::FILE_ERROR_CODE>(FLAGS_filemock_return_value));
        return -1;
    }
    else
    {
        return  LocalFile::Close(error_code);
    }
}

int64_t FileMock::Seek(int64_t offset, int32_t whence, uint32_t* error_code)
{
    if (--FLAGS_filemock_seek_mock < 0)
    {
        FLAGS_filemock_seek_mock = 0x7fffffff;
        SetErrorCode(error_code, static_cast<common::FILE_ERROR_CODE>(FLAGS_filemock_return_value));
        return -1;
    }
    else
    {
        return   LocalFile::Seek(offset, whence, error_code);
    }
}

int64_t FileMock::Tell(uint32_t* error_code)
{
    if (--FLAGS_filemock_tell_mock < 0)
    {
        FLAGS_filemock_tell_mock = 0x7fffffff;
        SetErrorCode(error_code, static_cast<common::FILE_ERROR_CODE>(FLAGS_filemock_return_value));
        return -1;
    }
    else
    {
        return  LocalFile::Tell(error_code);
    }
}

bool FileMock:: OpenImpl(const char *file_path, uint32_t flags,
                         const OpenFileOptions& options,
                         uint32_t *error_code)
{
    if (--FLAGS_filemock_read_mock < 0)
    {
        FLAGS_filemock_read_mock = 0x7fffffff;
        SetErrorCode(error_code, static_cast<common::FILE_ERROR_CODE>(FLAGS_filemock_return_value));
        return false;
    }
    else
    {
        return  LocalFile::OpenImpl(file_path, flags, options, error_code);
    }
}

int64_t FileMock::GetSizeImpl(const char* file_name,
                              uint32_t*   error_code)
{
    if (--FLAGS_filemock_read_mock < 0)
    {
        FLAGS_filemock_read_mock = 0x7fffffff;
        SetErrorCode(error_code, static_cast<common::FILE_ERROR_CODE>(FLAGS_filemock_return_value));
        return -1;
    }
    else
    {
        if (--FLAGS_filemock_get_size_zero < 0)
        {
            FLAGS_filemock_get_size_zero = 0x7fffffff;
            SetErrorCode(error_code, static_cast<common::FILE_ERROR_CODE>(ERR_FILE_OK));
            return 0;
        }
        return  LocalFile::GetSizeImpl(file_name, error_code);
    }
}

int32_t FileMock::AsyncReadFrom(void* buffer, int64_t size,
                                int64_t start_position,
                                Closure<void, int64_t, uint32_t>* callback,
                                uint32_t  timeout, uint32_t* error_code)
{
    m_read_buffer = buffer;
    m_read_buffer_len = size;
    if (--FLAGS_filemock_read_mock < 0)
    {
        FLAGS_filemock_read_mock = 0x7fffffff;
        SetErrorCode(error_code, static_cast<common::FILE_ERROR_CODE>(FLAGS_filemock_return_value));
        return -1;
    }
    else
    {
        Closure<void, int64_t, uint32_t>* callback_mock = NewClosure(
                this, &FileMock::ReadCompleteCallBackMock, callback);
        return  LocalFile::AsyncReadFrom(buffer, size, start_position, callback_mock,
                            timeout, error_code);
    }
}

int32_t FileMock::AsyncWrite(const void* buf, int64_t buf_size,
                            Closure<void, int64_t, uint32_t>* callback,
                            uint32_t time_out, uint32_t* error_code)
{
    if (--FLAGS_filemock_write_mock < 0)
    {
        FLAGS_filemock_write_mock = 0x7fffffff;
        SetErrorCode(error_code, static_cast<common::FILE_ERROR_CODE>(FLAGS_filemock_return_value));
        return -1;
    }
    else
    {
        Closure<void, int64_t, uint32_t>* callback_mock = NewClosure(
                this, &FileMock::WriteCompleteCallBackMock, callback);
        return LocalFile::AsyncWrite(buf, buf_size, callback_mock, time_out, error_code);
    }
}

void FileMock::ReadCompleteCallBackMock(
                            Closure<void, int64_t, uint32_t>* out_callback,
                            int64_t readed_len,
                            uint32_t error_code)
{
    if (--FLAGS_filemock_read_mock < 0)
    {
        FLAGS_filemock_read_mock = 0x7fffffff;
        out_callback->Run(-1, FLAGS_filemock_return_value);
    }
    else if (--FLAGS_filemock_read_len_zero < 0)
    {
        FLAGS_filemock_read_len_zero = 0x7fffffff;
        out_callback->Run(0, static_cast<common::FILE_ERROR_CODE>(ERR_FILE_OK));
    }
    else
    {
        if (--FLAGS_filemock_data_read < 0)
        {
            FLAGS_filemock_data_read = 0x7fffffff;
            ::memset(m_read_buffer, 0, m_read_buffer_len);
        }
        out_callback->Run(readed_len, error_code);
    }
}

void FileMock::WriteCompleteCallBackMock(
                                Closure<void, int64_t, uint32_t>* out_callback,
                                int64_t writen_len,
                                uint32_t error_code)
{
    if (--FLAGS_filemock_write_mock < 0)
    {
        FLAGS_filemock_write_mock = 0x7fffffff;
        out_callback->Run(-1, FLAGS_filemock_return_value);
    }
    else
    {
        out_callback->Run(writen_len, error_code);
    }
}

bool FileMock::CheckExistImpl(const char* path_name,
                        uint32_t* error_code)
{
    if (--FLAGS_filemock_file_exist < 0)
    {
        FLAGS_filemock_file_exist = 0x7fffffff;
        SetErrorCode(error_code, static_cast<common::FILE_ERROR_CODE>(FLAGS_filemock_return_value));
        return false;
    }
    else
    {
        return LocalFile::CheckExistImpl(path_name, error_code);
    }
}

} // namespace common

