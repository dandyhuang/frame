// Copyright (c) 2015, Vivo Inc. All rights reserved.
/**
 * @file abstract_stream_file.h
 * @brief
 * @author chenzheng05@baidu.com
 * @date 2015-01-20
 */
//
// For developers implementing a new File subclass, there are two type of methods to override:
// 1. Stream File operations for a specified file instance, eg. read, write, seek, close.
// 2. Filesystem operations without a predefined file instance, eg. copy, move, list.
//
// Some developers may only want to override stream file operations, but don't care filesystem
// operations. This AbstractStreamFile is serve as base class to simplify such implementations.

#ifndef COMMON_FILE_ABSTRACT_STREAM_FILE_H
#define COMMON_FILE_ABSTRACT_STREAM_FILE_H
#pragma once

#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/storage/file/file2.h"
#include "thirdparty/monitor_sdk_common/storage/file/file_fwd.h"

namespace common {

class AbstractStreamFile : public File {
public:
    // Following are stream operations with no-sense implementations.
    // All subclass should override these operations.
    virtual int64_t Read(void*     buffer,
                         int64_t   size,
                         uint32_t* error_code = NULL) = 0;

    virtual int32_t ReadLine(void* buffer, int32_t max_size) = 0;

    virtual int32_t AsyncReadFrom(void* buffer,
                                  int64_t size,
                                  int64_t start_position,
                                  Closure<void, int64_t, uint32_t>* callback,
                                  uint32_t  timeout = kDefaultAsyncTimeout,
                                  uint32_t* error_code = NULL) = 0;

    virtual int64_t Write(const void* buffer,
                          int64_t     size,
                          uint32_t*   error_code = NULL) = 0;

    virtual int32_t AsyncWrite(const void* buffer,
                               int64_t     size,
                               Closure<void, int64_t, uint32_t>* callback,
                               uint32_t    timeout = kDefaultAsyncTimeout,
                               uint32_t*   error_code = NULL) = 0;

    virtual bool SupportAsync() = 0;

    virtual int32_t Flush(uint32_t* error_code = NULL) = 0;

    virtual int32_t Close(uint32_t* error_code = NULL) = 0;

    virtual int64_t Seek(int64_t   offset,
                         int32_t   whence,
                         uint32_t* error_code = NULL) = 0;

    virtual int64_t Tell(uint32_t* error_code = NULL) = 0;

    virtual int32_t Truncate(uint64_t length, uint32_t* error_code = NULL) = 0;

    virtual int32_t LocateData(uint64_t      start,
                               uint64_t      end,
                               std::vector<DataLocation>* buffer,
                               uint32_t*     error_code = NULL) = 0;

    virtual ~AbstractStreamFile() { }

    // This is FileSystem operation, but may be called by users. So let it public.
    virtual std::string GetFileImplName() { return "AbstractStreamFile"; }

protected:
    // Following methods are FileSystem operations.
    // Applicatiions should nevel call these methods.

    virtual bool InitImpl();

    virtual bool CleanupImpl();

    virtual bool OpenImpl(const char *file_path, uint32_t flags,
                          const OpenFileOptions& options = OpenFileOptions(),
                          uint32_t *error_code = NULL);

    virtual int32_t MoveImpl(const char* src_name,
                             const char* dst_name,
                             uint32_t* error_code);

    virtual int32_t RenameImpl(const char* old_path_name,
                               const char* new_path_name,
                               uint32_t*   error_code = NULL);

    virtual int32_t RemoveImpl(const char* path_name,
                               bool        is_recursive = false,
                               uint32_t*   error_code = NULL);

    virtual int32_t AddDirImpl(const char* path_name,
                               uint32_t* error_code = NULL);

    virtual int32_t ListImpl(const char* pattern,
                             const ListOptions& list_option,
                             std::vector<AttrsInfo>* buffer,
                             uint32_t* error_code = NULL);

    virtual int64_t DuImpl(const char* path_name,
                           uint32_t* error_code = NULL);

    virtual bool GetContentSummaryImpl(const char* path_name,
                                       FileContentSummaryMask* mask,
                                       FileContentSummary* summary,
                                       uint32_t* error_code = NULL);

    virtual int64_t GetSizeImpl(const char* file_name,
                                uint32_t*   error_code = NULL);

    virtual bool CheckExistImpl(const char* path_name,
                                uint32_t* error_code = NULL);

    virtual bool ChmodImpl(const char* path_name,
                           const uint32_t permission,
                           uint32_t* error_code = NULL);

    virtual bool ChangeRoleImpl(const char* path_name,
                                const char* role_name,
                                uint32_t* error_code = NULL);

    virtual bool ChangeSecondRoleImpl(const char* path_name,
                                      const char* second_role_name,
                                      bool is_recursive = false,
                                      uint32_t* error_code = NULL);

    virtual bool GetDigestImpl(File* file_obj,
                               int64_t   file_size,
                               uint32_t* digest,
                               uint32_t* error_code = NULL);
};

} // namespace common

#endif // COMMON_FILE_ABSTRACT_STREAM_FILE_H
