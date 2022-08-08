// Copyright (C) 2014, Vivo Inc.
// Author: Zhang ShuXin (zhangshuxin@baidu.com)
//
// Description:

#include "thirdparty/monitor_sdk_common/storage/file/hdfs_file_stream.h"

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "thirdparty/monitor_sdk_common/base/stdext/string.h"
#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

namespace common {

using file_stream::SetErrorCode;

HDFSFileStream::HDFSFileStream(std::string host,
                               int16_t port,
                               int32_t read_buffer_size)
    : m_hdfs_client(NULL),
      m_hdfs_file(NULL),
      m_buffer(NULL),
      m_buffer_size(read_buffer_size),
      m_valid_pos(0),
      m_valid_len(0) {
    m_hdfs_client_options.host = host;
    m_hdfs_client_options.port = port;
}

HDFSFileStream::~HDFSFileStream() {
    Close();
}

void HDFSFileStream::Reset() {
    m_valid_pos = 0;
    m_valid_len = 0;
    delete []m_buffer;
    m_buffer = NULL;

    delete m_hdfs_file;
    m_hdfs_file = NULL;

    if (m_hdfs_client) {
        HDFSClient::ReleaseHDFSClient(m_hdfs_client);
        m_hdfs_client = NULL;
    }
}

bool HDFSFileStream::Open(const ::std::string& file_path,
                          FileOpenMode flag,
                          FileErrorCode* error_code) {
    file::OpenModel hdfs_open_mode = file::kRead;
    switch (flag) {
    case FILE_READ:
        hdfs_open_mode = file::kRead;
        break;
    case FILE_WRITE:
        hdfs_open_mode = file::kWrite;
        break;
    case FILE_APPEND:
        hdfs_open_mode = file::kAppend;
        break;
    }

    m_open_mode = flag;
    m_hdfs_client = HDFSClient::GetHDFSClient(m_hdfs_client_options);
    if (m_hdfs_client == NULL) {
        SetErrorCode(error_code, kFileErrOpenFail);
        return false;
    }

    m_hdfs_file = m_hdfs_client->Open(file_path.c_str(), hdfs_open_mode);
    if (m_hdfs_file == NULL) {
        SetErrorCode(error_code, kFileErrOpenFail);
        return false;
    }

    m_buffer = new char[m_buffer_size];
    m_valid_pos = 0;
    m_valid_len = 0;

    SetErrorCode(error_code, kFileSuccess);
    return true;
}

bool HDFSFileStream::Close(FileErrorCode* error_code) {
    if (!m_hdfs_file) {
        SetErrorCode(error_code, kFileErrNotOpen);
        return false;
    }

    if (!Flush() || !m_hdfs_file->Close()) {
        SetErrorCode(error_code, kFileErrClose);
        return false;
    }

    Reset();

    SetErrorCode(error_code, kFileSuccess);
    return true;
}

int64_t HDFSFileStream::Read(void* buffer, int64_t buffer_size, FileErrorCode* error_code) {
    if (!m_hdfs_file) {
        SetErrorCode(error_code, kFileErrNotOpen);
        return -1;
    }

    if (!buffer || buffer_size <= 0) {
        SetErrorCode(error_code, kFileErrParameter);
        return -1;
    }

    char* byte_buffer = static_cast<char*>(buffer);
    int64_t read_bytes = 0;
    // 缓冲区中有数据
    if (m_valid_len > 0) {
        int64_t will_copy_len = m_valid_len;
        if (will_copy_len > buffer_size) {
            will_copy_len = buffer_size;
        }
        memcpy(buffer, m_buffer + m_valid_pos, will_copy_len);
        read_bytes = will_copy_len;
        m_valid_pos += will_copy_len;
        m_valid_len -= will_copy_len;
    }

    // 缓冲区中数据不够
    int64_t left_len = buffer_size - read_bytes;
    if (left_len >= m_buffer_size) {
        // 用户请求数据太大，直接把数据读取到用户buffer
        int64_t cur_read_len = m_hdfs_file->Read(byte_buffer + read_bytes, left_len);
        if (cur_read_len > 0) {
            read_bytes += cur_read_len;
        }
    } else if (left_len > 0) {
        // 先把数据读取到缓冲区，然后拷贝一部分到用户buffer
        int64_t cur_read_len = m_hdfs_file->Read(m_buffer, m_buffer_size);
        m_valid_pos = 0;
        m_valid_len = cur_read_len;
        if (m_valid_len > 0) {
            int64_t will_copy_len = m_valid_len;
            if (will_copy_len > left_len) {
                will_copy_len = left_len;
            }
            memcpy(byte_buffer + read_bytes, m_buffer, will_copy_len);
            read_bytes += will_copy_len;
            m_valid_pos += will_copy_len;
            m_valid_len -= will_copy_len;
        }
    }
    SetErrorCode(error_code, kFileSuccess);
    if (read_bytes != buffer_size) {
        SetErrorCode(error_code, kFileErrRead);
        LOG(INFO) << "hdfs read fail, requesut len: " << buffer_size
            << ", read len: " << read_bytes;
    }

    return read_bytes;
}

int64_t HDFSFileStream::Write(const void* buffer, int64_t buffer_size, FileErrorCode* error_code) {
    if (!m_hdfs_file) {
        SetErrorCode(error_code, kFileErrNotOpen);
        LOG(ERROR) << "fail";
        return -1;
    }

    if (!buffer || buffer_size <= 0) {
        SetErrorCode(error_code, kFileErrParameter);
        return -1;
    }

    SetErrorCode(error_code, kFileSuccess);
    int64_t write_bytes = m_hdfs_file->Write(buffer, buffer_size);

    if (write_bytes != buffer_size) {
        LOG(ERROR) << "hdfs write fail, requesut len: " << buffer_size
            << ", written len: " << write_bytes;
        SetErrorCode(error_code, kFileErrWrite);
    }
    return write_bytes;
}

bool HDFSFileStream::Flush() {
    if (!m_hdfs_file) {
        return false;
    }

    if (m_open_mode == FILE_READ) {
        return true;
    }

    bool flush_ret = m_hdfs_file->Flush();
    if (!flush_ret) {
        LOG(ERROR) << "flush fail";
    }
    return flush_ret;
}

int64_t HDFSFileStream::Seek(int64_t offset, int32_t origin, FileErrorCode* error_code) {
    if (!m_hdfs_file) {
        SetErrorCode(error_code, kFileErrNotOpen);
        return -1;
    }

    // read方式打开的文件才允许seek
    if (m_open_mode != FILE_READ) {
        SetErrorCode(error_code, kFileFail);
        return -1;
    }

    // 清空本地buffer
    m_valid_pos = 0;
    m_valid_len = 0;

    int64_t cur_pos = m_hdfs_file->Seek(offset, origin);
    SetErrorCode(error_code, kFileSuccess);
    if (cur_pos == -1) {
        SetErrorCode(error_code, kFileFail);
    }
    return cur_pos;
}

int64_t HDFSFileStream::Tell(FileErrorCode* error_code) {
    if (!m_hdfs_file) {
        return -1;
    }

    int64_t cur_pos = m_hdfs_file->Tell();
    SetErrorCode(error_code, kFileSuccess);
    if (cur_pos == -1) {
        SetErrorCode(error_code, kFileFail);
    }

    // 修正文件位置
    if (cur_pos >= 0 && m_open_mode == FILE_READ) {
        CHECK_LE(m_valid_len, cur_pos);
        cur_pos -= m_valid_len;
    }

    return cur_pos;
}

int64_t HDFSFileStream::GetSize(const ::std::string& file_path, FileErrorCode* error_code) {
    SetErrorCode(error_code, kFileFail);
    HDFSClient* hdfs_client = HDFSClient::GetHDFSClient(m_hdfs_client_options);
    if (!hdfs_client) {
        return -1;
    }
    int64_t file_size = hdfs_client->GetFileSize(file_path);
    HDFSClient::ReleaseHDFSClient(hdfs_client);

    if (file_size >= 0) {
        SetErrorCode(error_code, kFileSuccess);
    }
    return file_size;
}

int32_t HDFSFileStream::ReadLine(void* buffer, int32_t max_size) {
    if (!m_hdfs_file || !buffer || max_size <= 0) {
        return -1;
    }
    char* byte_buffer = static_cast<char*>(buffer);
    int32_t read_bytes = 0;
    while (read_bytes < max_size) {
        int32_t cur_read_bytes = Read(byte_buffer + read_bytes, 1);
        if (cur_read_bytes == -1) {
            break;
        }
        read_bytes += 1;
        if (*(byte_buffer + read_bytes - 1) == '\n') {
            break;
        }
    }
    return read_bytes;
}

int32_t HDFSFileStream::ReadLine(::std::string* result) {
    result->resize(0);

    while (true) {
        int32_t buffer_size = 4 * 1024;
        ::std::string buffer(buffer_size, 0);
        int32_t bytes = ReadLine(string_as_array(&buffer), m_buffer_size);

        if (bytes < 0) {
            result->resize(0);
            return bytes;
        }
        if (bytes == 0) {
            return result->size();
        }
        if (bytes > 0) {
            buffer.resize(bytes);
            result->append(buffer);
            if (StringEndsWith(*result, "\n")) {
                return result->size();
            }
        }
    }
}

int32_t HDFSFileStream::CreateDir(const ::std::string& file_path, FileErrorCode* error_code) {
    SetErrorCode(error_code, kFileFail);
    HDFSClient* hdfs_client = HDFSClient::GetHDFSClient(m_hdfs_client_options);
    if (!hdfs_client) {
        return -1;
    }
    SetErrorCode(error_code, kFileSuccess);
    int32_t create_ret = hdfs_client->CreateDir(file_path);
    HDFSClient::ReleaseHDFSClient(hdfs_client);

    return create_ret;
}

bool HDFSFileStream::IsExist(const ::std::string& file_path, FileErrorCode* error_code) {
    SetErrorCode(error_code, kFileFail);
    HDFSClient* hdfs_client = HDFSClient::GetHDFSClient(m_hdfs_client_options);
    if (!hdfs_client) {
        return false;
    }
    SetErrorCode(error_code, kFileSuccess);
    bool is_exist = hdfs_client->IsExist(file_path);
    HDFSClient::ReleaseHDFSClient(hdfs_client);

    return is_exist;
}

bool HDFSFileStream::IsDir(const ::std::string& file_path, FileErrorCode* error_code) {
    SetErrorCode(error_code, kFileFail);
    HDFSClient* hdfs_client = HDFSClient::GetHDFSClient(m_hdfs_client_options);
    if (!hdfs_client) {
        return -1;
    }

    int32_t err = 0;
    bool is_dir = hdfs_client->IsDir(file_path, &err);
    HDFSClient::ReleaseHDFSClient(hdfs_client);

    if (err == file::HDFS_OK) {
        SetErrorCode(error_code, kFileSuccess);
    }
    return is_dir;
}

int32_t HDFSFileStream::Remove(const ::std::string& file_path, FileErrorCode* error_code) {
    SetErrorCode(error_code, kFileFail);
    HDFSClient* hdfs_client = HDFSClient::GetHDFSClient(m_hdfs_client_options);
    if (!hdfs_client) {
        return false;
    }
    SetErrorCode(error_code, kFileSuccess);
    int32_t remove_ret = hdfs_client->Remove(file_path);
    HDFSClient::ReleaseHDFSClient(hdfs_client);

    return remove_ret;
}

int32_t HDFSFileStream::Rename(const ::std::string& src_file_path,
                               const ::std::string& dest_file_path,
                               FileErrorCode* error_code) {
    SetErrorCode(error_code, kFileFail);
    HDFSClient* hdfs_client = HDFSClient::GetHDFSClient(m_hdfs_client_options);
    if (!hdfs_client) {
        return false;
    }
    SetErrorCode(error_code, kFileSuccess);
    int32_t rename_ret = hdfs_client->Rename(src_file_path, dest_file_path);
    HDFSClient::ReleaseHDFSClient(hdfs_client);

    return rename_ret;
}
} // end of namespace common

