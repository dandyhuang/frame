// Copyright (C) 2014, Vivo Inc.
// Author: Zhang ShuXin (zhangshuxin@baidu.com)
//
// Description:

#ifndef COMMON_FILE_HDFS_FILE_STREAM_H
#define COMMON_FILE_HDFS_FILE_STREAM_H

#include <fstream>
#include <string>
#include "thirdparty/monitor_sdk_common/storage/file/file_stream.h"
#include "thirdparty/monitor_sdk_common/storage/file/hdfs/hdfs_client.h"

namespace common {

class HDFSFileStream : public FileStream {
public:
    HDFSFileStream(
        std::string host = "default",
        int16_t port = 0,
        int32_t read_buffer_size = kBufferSize);

    virtual ~HDFSFileStream();

    virtual bool Open(const ::std::string& file_path,
                      FileOpenMode flag,
                      FileErrorCode* error_code = NULL);

    virtual bool Close(FileErrorCode* error_code = NULL);

    virtual int64_t Write(const void* buffer, int64_t buffer_size, FileErrorCode* error_code = NULL);

    virtual int64_t Read(void* buffer, int64_t buffer_size, FileErrorCode* error_code = NULL);

    virtual bool Flush();

    virtual int64_t Seek(int64_t offset, int32_t origin, FileErrorCode* error_code = NULL);

    virtual int64_t Tell(FileErrorCode* error_code = NULL);

    virtual int64_t GetSize(const ::std::string& file_path, FileErrorCode* error_code = NULL);

    virtual int32_t ReadLine(void* buffer, int32_t max_size);

    virtual int32_t ReadLine(::std::string* result);

    virtual int32_t CreateDir(const ::std::string& file_path, FileErrorCode* error_code = NULL);

    virtual bool IsExist(const ::std::string& file_path, FileErrorCode* error_code = NULL);

    virtual bool IsDir(const ::std::string& file_path, FileErrorCode* error_code = NULL);

    virtual int32_t Remove(const ::std::string& file_path, FileErrorCode* error_code = NULL);

    virtual int32_t Rename(const ::std::string& src_file_path,
                           const ::std::string& dest_file_path,
                           FileErrorCode* error_code = NULL);

private:
    void Reset();

private:
    HDFSClient* m_hdfs_client;
    HDFSFile*   m_hdfs_file;
    HDFSClient::Options m_hdfs_client_options;

    FileOpenMode m_open_mode;

    // TODO(zhangshuxin) 测试一下小数据读操作，确实是否需要添加一层cache
    char*        m_buffer;      // 内部读缓冲区，为了合并小的读写请求
    int32_t      m_buffer_size; // 内部读缓冲区的长度
    int32_t      m_valid_pos;   // buffer中数据的有效起始位置
    int32_t      m_valid_len;   // buffer中数据的有效长度
    static const int32_t kBufferSize = 1024 * 512; // 512k的内部缓冲区
};

} // end of namespace common

#endif // COMMON_FILE_HDFS_FILE_STREAM_H
