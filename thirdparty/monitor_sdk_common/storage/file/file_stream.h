// Copyright (C) 2012, Vivo Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:

#ifndef COMMON_FILE_FILE_STREAM_H
#define COMMON_FILE_FILE_STREAM_H

#include <fstream>
#include <string>
#include "thirdparty/monitor_sdk_common/storage/file/file_types.h"

namespace common {

namespace file_stream {
    void SetErrorCode(FileErrorCode* error_code, FileErrorCode code);
}

class FileStream {
public:
    FileStream();
    virtual ~FileStream();

    virtual bool Open(const ::std::string& file_path,
                      FileOpenMode flag,
                      FileErrorCode* error_code = NULL);

    virtual bool Close(FileErrorCode* error_code = NULL);

    virtual int64_t Write(const void* buffer,
                          int64_t buffer_size,
                          FileErrorCode* error_code = NULL);

    virtual int64_t Read(void* buffer,
                         int64_t buffer_size,
                         FileErrorCode* error_code = NULL);

    virtual bool Flush();

    virtual int64_t Seek(int64_t offset,
                         int32_t origin,
                         FileErrorCode* error_code = NULL);

    virtual int64_t Tell(FileErrorCode* error_code = NULL);

    virtual int64_t GetSize(const ::std::string& file_path,
                            FileErrorCode* error_code = NULL);

    virtual int32_t ReadLine(void* buffer, int32_t max_size);

    virtual int32_t ReadLine(::std::string* result);

    virtual bool IsExist(const ::std::string& file_path,
                         FileErrorCode* error_code = NULL);

    virtual bool IsDir(const ::std::string& path,
                       FileErrorCode* error_code = NULL);

    virtual int32_t CreateDir(const ::std::string& file_path,
                              FileErrorCode* error_code = NULL);

    virtual int32_t Remove(const ::std::string& file_path,
                           FileErrorCode* error_code = NULL);

    virtual int32_t Rename(const ::std::string& src_file_path,
                           const ::std::string& dest_file_path,
                           FileErrorCode* error_code = NULL);

protected:
    ::std::string FileOpenModeToString(uint32_t flag);

private:
    FILE* m_fp;
};

} // end of namespace common

#endif // COMMON_FILE_FILE_STREAM_H
