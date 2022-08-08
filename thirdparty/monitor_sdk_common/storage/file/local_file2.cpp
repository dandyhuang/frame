// Copyright (c) 2015, Vivo Inc. All rights reserved.
// LocalFile.cc
// Refactor to use polymorphism to forward to the right subclass.

#ifdef _WIN32
#include <io.h>
#include <sys/stat.h>

// don't use windows defined macros(min, max)
#define NOMINMAX
#include <windows.h>
#else

#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <libaio.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include <stdio.h>
#include <algorithm>
#include <fstream>
#include <vector>

#include "thirdparty/monitor_sdk_common/base/closure2.h"
#include "thirdparty/monitor_sdk_common/base/compatible/stdio.h"
#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/crypto/lite_crc/fast_crc.h"
#include "thirdparty/monitor_sdk_common/storage/file/aioframe/aioframe.h"
#include "thirdparty/monitor_sdk_common/storage/file/local_file2.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/thread_pool.h"
#include "thirdparty/monitor_sdk_common/text/wildcard.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gflags/gflags.h"

DEFINE_string(local_file_identified, "local file identified",
              "make sure local file lib is not included repeatedly");
DEFINE_int32(max_local_file_retry_count, 3, "max retry count for local file write");

#ifdef _WIN32
#ifndef S_ISDIR
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISREG
#define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)
#endif

#if defined(__unix__)
// off_t, fseeko, ftello already defined
#elif defined(_WIN32)
#define fseeko _fseeki64
#define ftello _ftelli64
#else // ISO C
#define fseeko fseek
#define ftello ftell
#endif

#endif

// namespace common {

namespace {
// local thread pool for async operations.
common::ThreadPool* g_local_thread_pool = NULL;
// linux only
#ifndef _WIN32
common::AIOFrame* g_aioframe = NULL;
#endif

// Set cur_attrs according to given stat_buf, name, and mask.
void SetEntryAttributes(
    const struct stat& stat_buf,
    const std::string& name,
    const ListOptions& list_option,
    AttrsInfo* cur_attrs) {
    CHECK_NOTNULL(cur_attrs);

    cur_attrs->file_name = name;

    // TODO(aaronzou): use 64-bit timestamp?
    if (list_option.mask.access_time) {
        cur_attrs->access_time = static_cast<uint32_t>(stat_buf.st_atime);
    }
    if (list_option.mask.create_time) {
        cur_attrs->create_time = static_cast<uint32_t>(stat_buf.st_ctime);
    }
    if (list_option.mask.modify_time) {
        cur_attrs->modify_time = static_cast<uint32_t>(stat_buf.st_mtime);
    }
    if (list_option.mask.file_type) {
        ENUM_FILE_TYPE file_type = FILE_TYPE_NORMAL;
        if (S_ISDIR(stat_buf.st_mode)) {
            file_type = FILE_TYPE_DIR;
        }
        cur_attrs->file_type = file_type;
    }
    if (list_option.mask.file_size) {
        cur_attrs->file_size = static_cast<int64_t>(stat_buf.st_size);
    }
    if (list_option.mask.file_permission) {
        cur_attrs->file_permission = static_cast<uint16_t>(stat_buf.st_mode & 0777);
    }
}


FILE_ERROR_CODE TranslateLocalErrorCode(int32_t local_errno) {
#define CASE_TRANS_LOCAL_ERRORCODE(local_err, file_err) \
    case local_err: \
        error_code = (file_err); \
        break;

    FILE_ERROR_CODE error_code = ERR_FILE_FAIL;

    switch (local_errno) {
    CASE_TRANS_LOCAL_ERRORCODE(EACCES, ERR_FILE_PERMISSION_DENIED);
    CASE_TRANS_LOCAL_ERRORCODE(EEXIST, ERR_FILE_ENTRY_EXIST);
    CASE_TRANS_LOCAL_ERRORCODE(ENOENT, ERR_FILE_ENTRY_NOT_EXIST);
    CASE_TRANS_LOCAL_ERRORCODE(EISDIR, ERR_FILE_EXPECTED);
    CASE_TRANS_LOCAL_ERRORCODE(ENOTDIR, ERR_FILE_DIR_EXPECTED);
    CASE_TRANS_LOCAL_ERRORCODE(ENAMETOOLONG, ERR_FILE_PATH_TOO_LONG);
    CASE_TRANS_LOCAL_ERRORCODE(ENOSPC, ERR_FILE_NOSPACE);
    CASE_TRANS_LOCAL_ERRORCODE(EAGAIN, ERR_FILE_RETRY);
    CASE_TRANS_LOCAL_ERRORCODE(ENOMEM, ERR_FILE_RETRY);
    CASE_TRANS_LOCAL_ERRORCODE(EXDEV,  ERR_FILE_CROSS_DEVICE);
    default:
        error_code = ERR_FILE_FAIL;
    }

    return error_code;
}

} // namespace

namespace common {

// Register LocalFile when this module is load and all global varibles are initialed.
REGISTER_FILE_IMPL(LOCAL_FILE_PREFIX, LocalFile);

#define CHECK_HANDLE(h) \
    if (h == NULL) { \
        SetErrorCode(error_code, ERR_FILE_FAIL); \
        return -1; \
    }

// Define common functions for local file.
#ifdef _WIN32

#define ACCESS      _access

#else

#define ACCESS      access

#endif

void LocalFile::SetErrorCodeByLocal(uint32_t* error_code, uint32_t local_errno) {
    SetErrorCode(error_code, TranslateLocalErrorCode(local_errno));
}

LocalFile::LocalFile() {
    m_fp = NULL;
    m_open_flag = 0;
    m_cipher = NULL;
}

LocalFile::~LocalFile() {
    if (m_fp != NULL) {
        LocalFile::Close();
    }
}

std::string LocalFile::ConnectPathComponent(const char* file_path, const char* file_name) {
    CHECK_NOTNULL(file_path);
    CHECK_NOTNULL(file_name);

    std::string full_path(file_path);

    NormalizePath(&full_path);

    full_path += file_name;

    return full_path;
}

void LocalFile::NormalizePath(std::string* full_path) {
    if (full_path->length() > 0
            && File::kPathSeparator != (*full_path)[full_path->length() - 1]) {
        *full_path += File::kPathSeparator;
    }
}

bool LocalFile::InitImpl() {
    VLOG(1) << "Really init LocalFile";
    g_local_thread_pool = new ThreadPool(0, 1);
#ifndef _WIN32
    g_aioframe = new common::AIOFrame();
#endif
    return true;
}

bool LocalFile::CleanupImpl() {
    VLOG(1) << "Really cleanup LocalFile";

    delete g_local_thread_pool;
    g_local_thread_pool = NULL;
#ifndef _WIN32
    delete g_aioframe;
    g_aioframe = NULL;
#endif
    VLOG(1) << "Success to cleanup LocalFile";

    return true;
}

bool LocalFile::OpenImpl(const char* input_file_path, uint32_t flags,
    const OpenFileOptions& options,
    uint32_t* error_code) {
    // ignore options for local files.
    if (!input_file_path || !CheckOpenModeValid(flags)) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return false;
    }

    const char* file_path = input_file_path;

    // Not allow both read and write. Just as XFS file.
    std::string open_mode;
    if ((flags & ENUM_FILE_OPEN_MODE_R) == ENUM_FILE_OPEN_MODE_R) {
        open_mode += "r";
    } else if ((flags & ENUM_FILE_OPEN_MODE_W) == ENUM_FILE_OPEN_MODE_W) {
        open_mode += "w";
    } else if ((flags & ENUM_FILE_OPEN_MODE_A) == ENUM_FILE_OPEN_MODE_A) {
        open_mode += "a";
    }
    open_mode += "b";

    m_fp = fopen(file_path, open_mode.c_str());

    m_open_flag = flags;

    if (m_fp == NULL) {
        if (error_code == NULL) {
            LOG(ERROR) << "Fail to open local file " << file_path
                << ". Reason: " << strerror(errno);
        }

        SetErrorCodeByLocal(error_code, errno);

        return false;
    } else {
#ifdef _WIN32
        SetFileAttributes(file_path, FILE_ATTRIBUTE_NORMAL);
#endif
        SetErrorCode(error_code, ERR_FILE_OK);

        if (IsCipherFile()) {
            m_cipher = CipherBase::CreateCipher();
            CHECK(m_cipher != NULL)
                << "Do you forget to link the real cipher implement library?";

            if (m_open_flag & ENUM_FILE_OPEN_MODE_W) {
                return m_cipher->Open_Write(file_path,
                            NewPermanentClosure(this, &LocalFile::RealWrite),
                            NewPermanentClosure(this, &LocalFile::RealFlush));
            } else if (m_open_flag & ENUM_FILE_OPEN_MODE_R) {
                return m_cipher->Open_Read(file_path,
                            NewPermanentClosure(this, &LocalFile::RealRead));
            } else {
                LOG(ERROR) << "open flag not supported! open_flag=" << m_open_flag;
                return false;
            }
        }
        return true;
    }
}

int32_t LocalFile::Close(uint32_t* error_code) {
    CHECK_HANDLE(m_fp);

    int32_t ret = 0;
    int32_t flush_ret = Flush(error_code);
    if (flush_ret < 0) {
        LOG(ERROR) << "flush before close failed!";
    }

    if (fclose(m_fp) == 0) {
        SetErrorCode(error_code, ERR_FILE_OK);
    } else {
        SetErrorCodeByLocal(error_code, errno);
        ret = -1;
    }

    // Even with fclose fail, reset pointer to avoid call fclose later.
    // Call fclose twice is undefined behaviour.
    m_fp = NULL;

    if (IsCipherFile()) {
        delete m_cipher;
        m_cipher = NULL;
    }
    return ret;
}

int32_t LocalFile::Flush(uint32_t* error_code) {
    if (IsCipherFile()) {
        return m_cipher->Flush(error_code);
    } else {
        return RealFlush(error_code);
    }
}

int32_t LocalFile::RealFlush(uint32_t* error_code) {
    CHECK_HANDLE(m_fp);

#ifndef _WIN32
    int32_t ret = fflush(m_fp);
    if (ret == 0) {
        SetErrorCode(error_code, ERR_FILE_OK);
    } else {
        SetErrorCodeByLocal(error_code, errno);
    }
    return ret;
#else
    return 0;
#endif
}

int64_t LocalFile::Write(const void* buf, int64_t buf_size, uint32_t* error_code) {
    if (IsCipherFile()) {
        return m_cipher->Write(buf, buf_size, error_code);
    } else {
        return RealWrite(buf, buf_size, error_code);
    }
}

int64_t LocalFile::RealWrite(const void* buf, int64_t buf_size, uint32_t* error_code) {
    CHECK_HANDLE(m_fp);
    if (!buf || buf_size < 0) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }

    int64_t total_size = 0;
    int32_t try_count = 0;
    const char* byte_buf = static_cast<const char*>(buf);

    CHECK_GT(FLAGS_max_local_file_retry_count, 0) << "Must really write local file.";
    while (try_count < FLAGS_max_local_file_retry_count) {
        size_t expect_size = static_cast<size_t>(buf_size - total_size);
        if (expect_size == 0u) break;

        size_t ret_size = fwrite(byte_buf + total_size, 1, expect_size, m_fp);
        ++try_count;

        total_size += static_cast<int64_t>(ret_size); // ret_size always >= 0.

        if (ret_size < expect_size) {
            LOG(ERROR) << "writen not enough bytes and fail"
                << ", expect to write total size " << buf_size
                << ", now total size " << total_size
                << ", this write expect size " << expect_size
                << ", this writen get size " << ret_size
                << ", reason: " << strerror(errno);
            CHECK(ferror(m_fp)) << "Must have error to cause write less than expect size";

            // Only retry when:
            // 1) Retry for EINTR
            // 2) Retry servral times when writen some bytes this time.
            // TODO(aaronzou): another policy is abort now.
            //    Refine policy after run with logs to gather more experiences.
            if (errno != EINTR && ret_size == 0u) break;
        }
    }

    if (total_size == buf_size) {
        SetErrorCode(error_code, ERR_FILE_OK);
    } else {
        SetErrorCodeByLocal(error_code, errno);
    }

    // Return actually writen size rather than -1 when finally failed but writen size > 0.
    // So applications could handle side effect, such as consuming disk and changing file position.
    return total_size >= 0 ? total_size : -1;
}

int32_t LocalFile::ReadLine(void* buffer, int32_t max_size) {
    if (m_fp == NULL) return -1;

    if (!buffer || max_size <= 0) return -1;
    char* read_buffer = static_cast<char*>(buffer);
    off64_t org_offest = ftello(m_fp);
    if (org_offest < 0)
        return -1;
    char* readed_buffer = fgets(read_buffer, max_size, m_fp);
    if (readed_buffer == NULL) {
        if (feof(m_fp)) {
            return 0;
        } else {
            return -1;
        }
    } else {
        off64_t new_offset = ftello(m_fp);
        if (new_offset < 0)
            return -1;
        return new_offset - org_offest;
    }
}

int64_t LocalFile::Read(void* buf, int64_t buf_size, uint32_t* error_code) {
    if (IsCipherFile()) {
        return m_cipher->Read(buf, buf_size, error_code);
    } else {
        return RealRead(buf, buf_size, error_code);
    }
}

int64_t LocalFile::RealRead(void* buf, int64_t buf_size, uint32_t* error_code) {
    CHECK_HANDLE(m_fp);
    if (!buf || buf_size <= 0) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }

    int64_t read_bytes = fread(buf, 1, static_cast<size_t>(buf_size), m_fp);

    bool success = true;
    if (read_bytes != buf_size) {
        if (ferror(m_fp)) success = false;
    }

    if (success) {
        SetErrorCode(error_code, ERR_FILE_OK);
    } else {
        SetErrorCodeByLocal(error_code, errno);
    }
    return success ? read_bytes : -1;
}


void LocalFile::AsyncWriteAIOCallback(
    Closure<void, int64_t, uint32_t>* out_callback,
    int64_t start_position,
    int64_t size,
    uint32_t status_code) {
    uint32_t error_code = ERR_FILE_OK;
    // the wanted write size must not 0. So writen 0 bytes is wrong.
    if (status_code != 0u || size <= 0) {
        // when has AIO errors, status_code may still be 0, so must check size.
        error_code = status_code != 0u ? TranslateLocalErrorCode(status_code) : ERR_FILE_FAIL;
        if (size < 0) {
            LOG(ERROR) << "AsyncWriteAIOCallback with status code " << status_code
                << " size " << size;
        }
    }
    // Important: Must move file pointers to change AsyncWrite semantic as AsyncAppend
    // Becuase XFS only support Append, so use the same semantic.
    if ((m_open_flag & File::ENUM_FILE_OPEN_MODE_A) == 0) {
        // Only move file pointer in write mode.
        // Append mode has no necessary to move here, becase file system ensures the movement.
        uint32_t seek_error_code = ERR_FILE_FAIL;
        LocalFile::Seek(start_position + size, SEEK_SET, &seek_error_code);
        CHECK_EQ(static_cast<uint32_t>(ERR_FILE_OK), seek_error_code)
            << "Can not seek to new position after AsyncWrite. Must has errors."
            << "Multiple threads AsyncWrite concurrently?";
    }

    out_callback->Run(size, error_code);
}

int32_t LocalFile::AsyncWrite(const void* buf, int64_t buf_size,
    Closure<void, int64_t, uint32_t>* callback,
    uint32_t time_out, uint32_t* error_code) {
    CHECK_HANDLE(m_fp);
    if (!buf || buf_size <= 0) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }

#ifdef _WIN32
    Closure<void>* task =
        NewClosure(
            this,
            &LocalFile::DoAsyncWrite,
            buf, buf_size, callback, time_out);
    g_local_thread_pool->AddTask(task);

    SetErrorCode(error_code, ERR_FILE_OK);
    return 0;
#else
    CHECK(g_aioframe != NULL)
        << "Forget to call File::Init? Should Init before use async operations";

    int64_t start_position = LocalFile::Tell();
    common::AIOFrame::StatusCode code;

    Closure<void, int64_t, uint32_t>* aio_callback =
        NewClosure(this, &LocalFile::AsyncWriteAIOCallback, callback, start_position);

    int fd = fileno(m_fp);
    bool success = g_aioframe->AsyncWrite(fd, buf, buf_size, start_position, aio_callback, &code);
    if (error_code != NULL) {
        *error_code = success ? ERR_FILE_OK : TranslateLocalErrorCode(code);
    }

    return success ? 0 : -1;
#endif
}


int64_t LocalFile::Seek(int64_t offset, int32_t origin, uint32_t* error_code) {
    CHECK_HANDLE(m_fp);

    if (fseeko(m_fp, offset, origin) < 0) {
        SetErrorCodeByLocal(error_code, errno);
        return -1;
    }

    return LocalFile::Tell(error_code);
}

int64_t LocalFile::Tell(uint32_t* error_code) {
    CHECK_HANDLE(m_fp);

    int64_t ret = ftello(m_fp);
    if (ret >= 0) {
        SetErrorCode(error_code, ERR_FILE_OK);
    } else {
        SetErrorCodeByLocal(error_code, errno);
    }

    return ret;
}

int32_t LocalFile::Truncate(uint64_t length, uint32_t* error_code) {
    CHECK_HANDLE(m_fp);

    int fd = fileno(m_fp);
#ifdef _WIN32
    if (0 != _chsize_s(fd, static_cast<int64_t>(length))) {
        SetErrorCode(error_code, ERR_FILE_FAIL);
        return -1;
    } else {
        SetErrorCode(error_code, ERR_FILE_OK);
        return 0;
    }
#else
    if (0 != ftruncate(fd, (off_t)length)) {
        SetErrorCodeByLocal(error_code, errno);
        return -1;
    } else {
        SetErrorCode(error_code, ERR_FILE_OK);
        return 0;
    }

#endif
}

int32_t LocalFile::ListImpl(const char* pattern,
                            const ListOptions& list_option,
                            std::vector<AttrsInfo>* attrs_info,  uint32_t* error_code) {
    SetErrorCode(error_code, ERR_FILE_OK);
    if (!pattern || !attrs_info) {
        if (error_code != NULL) {
            LOG(ERROR) << "parameter error, NULL pointers";
        }
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }


    const size_t len = strlen(pattern);
    if (len == 0u) {
        if (error_code != NULL) {
            LOG(ERROR) << "parameter error, empty pattern";
        }
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }

    int32_t path_type = ValidatePathWildcard(pattern);
    if (path_type < 0) {
        if (error_code != NULL) {
            LOG(ERROR) << "parameter error, wrong pattern " << pattern;
        }
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }

    std::string path_prefix(pattern);
    std::string name_pattern; // name part for wildcard
    if (path_type > 0) { // has wildcard, split the last path component
        const char* sep_pos = strrchr(pattern, '/');
        if (sep_pos == NULL) {
            // current directory, for pattern like '*.txt'
            name_pattern = path_prefix;
            path_prefix = ".";
        } else {
            name_pattern.assign(sep_pos + 1);
            path_prefix.assign(pattern, sep_pos + 1);
        }
    }
    const char* name = path_prefix.c_str();

    // clear after check parameter
    attrs_info->clear();

#ifdef _WIN32
    int32_t file_exist = _access(name, 0);
    if (file_exist != 0) {
        SetErrorCode(error_code, ERR_FILE_ENTRY_NOT_EXIST);
        if (!error_code) LOG(ERROR) << "file " << name << " not exists";
        return -1;
    }

    // Use "*" instead of "*.*" because some file may have no file types.
    std::string full_file_name(pattern); // windows support wildcard
    if (full_file_name.length() > 0) {
        char lastCh = full_file_name[full_file_name.length() - 1];

        if (lastCh == File::kPathSeparator || lastCh == ':') { // ":" is for "C:"
            // for folders, to search files in folders.
            full_file_name = ConnectPathComponent(full_file_name.c_str(), "*");
        }
    }

    WIN32_FIND_DATA file_data;
    HANDLE find_handle = ::FindFirstFile(full_file_name.c_str(), &file_data);

    if (find_handle == INVALID_HANDLE_VALUE) {
        SetErrorCode(error_code, ERR_FILE_ENTRY_NOT_EXIST);
        if (!error_code) LOG(ERROR) << "file " << name << " not exists";
        return -1;
    }

    do {
        if (strcmp(file_data.cFileName, ".") == 0 || strcmp(file_data.cFileName, "..") == 0) {
            continue;
        }

        AttrsInfo cur_attrs;

        cur_attrs.file_name = file_data.cFileName;
        if ((file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) { // win32 dir, add '\\'
            NormalizePath(&cur_attrs.file_name);
        }
        if (list_option.mask.access_time) {
            FILETIME ft = file_data.ftLastAccessTime;
            LONGLONG  ll;
            ULARGE_INTEGER ui;
            ui.LowPart =  ft.dwLowDateTime;
            ui.HighPart = ft.dwHighDateTime;
            ll = (static_cast<uint64_t>(ft.dwHighDateTime) << 32)  +  ft.dwLowDateTime;
            time_t at =  ((LONGLONG)(ui.QuadPart  -  116444736000000000)  /  10000000);
            cur_attrs.access_time = static_cast<int32_t>(at);
        }
        if (list_option.mask.create_time) {
            FILETIME ft = file_data.ftCreationTime;
            LONGLONG  ll;
            ULARGE_INTEGER ui;
            ui.LowPart =  ft.dwLowDateTime;
            ui.HighPart = ft.dwHighDateTime;
            ll = (static_cast<uint64_t>(ft.dwHighDateTime) << 32)  +  ft.dwLowDateTime;
            time_t ct =  ((LONGLONG)(ui.QuadPart  -  116444736000000000)  /  10000000);
            cur_attrs.create_time = static_cast<int32_t>(ct);
        }
        if (list_option.mask.modify_time) {
            FILETIME ft = file_data.ftLastWriteTime;
            LONGLONG  ll;
            ULARGE_INTEGER ui;
            ui.LowPart =  ft.dwLowDateTime;
            ui.HighPart = ft.dwHighDateTime;
            ll = (static_cast<uint64_t>(ft.dwHighDateTime) << 32)  +  ft.dwLowDateTime;
            time_t mt =  ((LONGLONG)(ui.QuadPart  -  116444736000000000)  /  10000000);
            cur_attrs.modify_time = static_cast<int32_t>(mt);
        }

        if (list_option.mask.file_type) {
            ENUM_FILE_TYPE fileType = FILE_TYPE_NORMAL;
            if ((file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                fileType = FILE_TYPE_DIR;
            cur_attrs.file_type = fileType;
        }
        if (list_option.mask.file_size) {
            cur_attrs.file_size = file_data.nFileSizeHigh * 1024 * 1024 * 1024 * 4 +
                                  file_data.nFileSizeLow;
        }

        attrs_info->push_back(cur_attrs);
    } while (::FindNextFile(find_handle, &file_data));

    ::FindClose(find_handle);
    return 0;

#else
// Linux
    int32_t file_exist = access(name, F_OK);
    if (file_exist != 0) {
        SetErrorCode(error_code, ERR_FILE_ENTRY_NOT_EXIST);
        if (!error_code) LOG(ERROR) << "file " << name << " not exists";
        return -1;
    }

    DIR * dir_ptr;
    struct dirent * dirent_ptr;
    struct stat stat_buf;

    if (stat(name, &stat_buf) < 0) {
        LOG(ERROR) << "stat error for " << name;
        SetErrorCodeByLocal(error_code, errno);
        return -1;
    }
    if (list_option.mask.stat_info) {
        // Want the infomation of this entry itself.
        AttrsInfo cur_attrs;
        SetEntryAttributes(stat_buf, name, list_option, &cur_attrs);

        cur_attrs.error_code = ERR_FILE_OK;
        attrs_info->push_back(cur_attrs);

        return 0;
    }

    // directory
    if (S_ISDIR(stat_buf.st_mode)) {
        dir_ptr = opendir(name);
        if (dir_ptr == NULL) {
            // Must be a very rare case, LOG this error.
            LOG(WARNING) << "opendir error for " << name;
            SetErrorCodeByLocal(error_code, errno);
            return -1;
        }
        while ((dirent_ptr = readdir(dir_ptr)) != NULL) { // NOLINT
            if (strcmp(dirent_ptr->d_name, ".") == 0 || strcmp(dirent_ptr->d_name, "..") == 0) {
                continue;
            }

            if (path_type > 0 && !Wildcard::Match(name_pattern.c_str(), dirent_ptr->d_name)) {
                continue; // ignore don't match files
            }

            std::string full_file_name = ConnectPathComponent(name, dirent_ptr->d_name);
            if (stat(full_file_name.c_str(), &stat_buf) < 0) {
                LOG(ERROR) << "stat error for " << full_file_name << ", may be deleted, ignore it";
                continue;
            }

            AttrsInfo cur_attrs;

            SetEntryAttributes(stat_buf, dirent_ptr->d_name, list_option, &cur_attrs);

            cur_attrs.error_code = ERR_FILE_OK;
            attrs_info->push_back(cur_attrs);
            // should not recursively list
        }
        closedir(dir_ptr);
    } else {    // normal file.
        AttrsInfo cur_attrs;

        std::string file_name = name;
        size_t idx = file_name.rfind('/');
        if (idx != std::string::npos) {
            file_name = file_name.substr(idx + 1);
        }
        SetEntryAttributes(stat_buf, file_name, list_option, &cur_attrs);
        cur_attrs.error_code = ERR_FILE_OK;
        attrs_info->push_back(cur_attrs);
    }
    return 0;
#endif
}

bool LocalFile::CheckExistImpl(const char* full_file_name, uint32_t* error_code) {
    const char* file_name = full_file_name;

    int32_t ret = ACCESS(file_name, 0);
    // When file not exist, better error_code is ERR_FILE_OK.
    // However, just use same error code with XFS to avoid break existing XFS applications
    if (ret == 0) {
        SetErrorCode(error_code, ERR_FILE_OK);
        return true;
    } else {
        SetErrorCodeByLocal(error_code, errno);
        return false;
    }
}

int64_t LocalFile::DuImpl(const char* file_name, uint32_t* error_code) {
    int64_t ret = -1;
    if (!file_name) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        LOG(ERROR) << "file " << file_name << " not exists";
        return ret;
    }
#ifdef _WIN32
    int32_t file_exist = ACCESS(file_name, 0);
    if (file_exist != 0) {
        SetErrorCode(error_code, ERR_FILE_ENTRY_NOT_EXIST);
        LOG(ERROR) << "file " << file_name << " not exists";
        return ret;
    }

    int64_t len = static_cast<int64_t>(strlen(file_name));

    // Use "*" instead of "*.*" because some file may have no file types.
    std::string full_file_name(file_name);
    if (full_file_name.length() > 0) {
        char lastCh = full_file_name[full_file_name.length() - 1];

        if (lastCh == File::kPathSeparator || lastCh == ':') { // ":" is for "C:"
            // for folders, to search files in folders.
            full_file_name = ConnectPathComponent(full_file_name.c_str(), "*");
        }
    }

    WIN32_FIND_DATA file_data;
    HANDLE find_handle=::FindFirstFile(full_file_name.c_str(), &file_data);

    if (INVALID_HANDLE_VALUE == find_handle) {
        ret = 0;
        // Hidden files in Windows.
        SetErrorCode(error_code, ERR_FILE_OK);
        return ret;
    }

    ret = 0;
    do {
        if (strcmp(file_data.cFileName, ".") == 0 || strcmp(file_data.cFileName, "..") == 0 )
            continue;
        bool is_dir = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        char* cur_file_name = file_data.cFileName;
        if (strcmp(cur_file_name , ".") == 0 || strcmp(cur_file_name, "..") == 0)
            continue;

        if (is_dir) {
            std::string new_file_name = ConnectPathComponent(file_name, cur_file_name);
            new_file_name += "/";
            ret += LocalFile::DuImpl(new_file_name.c_str(), error_code);
        } else {
            ret += file_data.nFileSizeHigh * 1024 * 1024 * 1024 * 4 + file_data.nFileSizeLow;
        }
    } while (FindNextFile(find_handle, &file_data));

    ::FindClose(find_handle);
    if (-1 != ret) {
        SetErrorCode(error_code, ERR_FILE_OK);
    } else {
        SetErrorCode(error_code, ERR_FILE_FAIL);
    }
    return ret;

#else
    int32_t file_exist = access(file_name, F_OK);
    if (file_exist != 0) {
        SetErrorCode(error_code, ERR_FILE_ENTRY_NOT_EXIST);
        if (!error_code) LOG(ERROR) << "file " << file_name << " not exists";
        return ret;
    }

    DIR * dir_ptr;
    struct dirent * dirent_ptr;
    struct stat stat_buf;

    if (stat(file_name, &stat_buf) < 0) {
        ret = -1;
        SetErrorCodeByLocal(error_code, errno);
        LOG(ERROR) << "stat error for " << file_name;
        return ret;
    }

    // directory
    if (S_ISDIR(stat_buf.st_mode)) {
        dir_ptr = opendir(file_name);
        ret = 0;
        while ((dirent_ptr = readdir(dir_ptr)) != NULL) { // NOLINT
            if (strcmp(dirent_ptr->d_name, ".") == 0 || strcmp(dirent_ptr->d_name, "..") == 0)
                continue;

            std::string new_file_name = ConnectPathComponent(file_name, dirent_ptr->d_name);
            ret += static_cast<int64_t>(stat_buf.st_size) +
                                        LocalFile::DuImpl(new_file_name.c_str(), error_code);
        }
        closedir(dir_ptr);
    } else {
        // Normal file
        return static_cast<int64_t>(stat_buf.st_size);
    }
    if (-1 != ret) {
        SetErrorCode(error_code, ERR_FILE_OK);
    } else {
        SetErrorCodeByLocal(error_code, errno);
    }
    return ret;
#endif
}

bool LocalFile::GetContentSummaryImpl(
    const char* path_name,
    FileContentSummaryMask* mask,
    FileContentSummary* summary,
    uint32_t* error_code) {
    LOG(FATAL) << "Not implement LocalFile::GetContentSummaryImpl yet.";
    return false;
}

int32_t LocalFile::MoveImpl(const char* src_name, const char* dst_name, uint32_t* error_code) {
    // not implement
    int32_t ret = -1;
    // LocalFile and XFSFile don't allow to move to the same file to avoid mis-operations.
    if (File::IsSameFilePath(src_name, dst_name)) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }

#ifdef _WIN32
    ret = ::MoveFile(src_name, dst_name);
    if (ret != 0) {
        SetErrorCode(error_code, ERR_FILE_FAIL);
    }
    return ret;
#else
    int32_t file_exist = access(src_name, F_OK);
    if (file_exist != 0) {
        SetErrorCode(error_code, ERR_FILE_ENTRY_NOT_EXIST);
        LOG(ERROR) << "file " << src_name << " not exists";
        return ret;
    }

    DIR * dir_ptr = NULL;
    struct dirent * dirent_ptr = NULL;
    struct stat stat_buf;
    struct stat stat_buf_dst;

    if (stat(src_name, &stat_buf) < 0) {
        ret = -1;
        SetErrorCodeByLocal(error_code, errno);
        LOG(ERROR) << "stat error for " << src_name;
        return ret;
    }
    // check existence of destination file/folder.
    bool dst_exist = true;
    if (stat(dst_name, &stat_buf_dst) < 0) {
        dst_exist = false;
    }

    // Directory.
    if (S_ISDIR(stat_buf.st_mode)) {
        // move dir to file, should fail.
        if (dst_exist && !S_ISDIR(stat_buf_dst.st_mode)) {
            ret = -1;
            SetErrorCodeByLocal(error_code, errno);
            return ret;
        }
        // then dest is directory or empty.

        // empty, create a folder.
        if (!dst_exist) {
            ret = LocalFile::AddDirImpl(dst_name, error_code);
            if (ret != 0) {
                // reuse error_code value
                return ret;
            }
        }
        ret = 0;
        dir_ptr = opendir(src_name);
        while ((dirent_ptr = readdir(dir_ptr)) != NULL && ret == 0) { // NOLINT
            if (strcmp(dirent_ptr->d_name, ".") == 0 || strcmp(dirent_ptr->d_name, "..") == 0)
                continue;

            std::string tmp_file_name = ConnectPathComponent(src_name, dirent_ptr->d_name);
            std::string complete_dst_name = ConnectPathComponent(dst_name, dirent_ptr->d_name);
            // for recursively move.
            ret = LocalFile::MoveImpl(tmp_file_name.c_str(), complete_dst_name.c_str(), error_code);
        }
        closedir(dir_ptr);
        // only delete this dir if all children are deleted.
        if (ret == 0) {
            ret = rmdir(src_name);
        }
    } else {
        // Normal file
        std::string complete_dst_name = dst_name;
        if (dst_exist) {
            if (S_ISDIR(stat_buf_dst.st_mode)) {
                const std::string& base_name = GetBaseName(src_name);
                complete_dst_name = ConnectPathComponent(dst_name, base_name.c_str());
                if (stat(complete_dst_name.c_str(), &stat_buf_dst) < 0) {
                    // concated file is not exist
                    dst_exist = false;
                } else if (S_ISDIR(stat_buf_dst.st_mode)) {
                    // after concat to get full dst name, exist a dir, return error.
                    SetErrorCode(error_code, ERR_FILE_ENTRY_EXIST);
                    return -1;
                }
            }

            if (dst_exist) {
                // will overwrite dst file.
                ret = unlink(complete_dst_name.c_str());
                if (ret != 0) {
                    SetErrorCodeByLocal(error_code, errno);
                    return -1;
                }
            }
        }

        // link only support from file to file.
        ret = link(src_name, complete_dst_name.c_str());
        if (ret == 0) {
            ret = unlink(src_name);
        }
    }

    SetErrorCode(error_code, ret == 0 ? ERR_FILE_OK : TranslateLocalErrorCode(errno));

    return ret;
#endif
}

int32_t LocalFile::RenameImpl(const char* old_file_name,
                              const char* new_file_name,
                              uint32_t* error_code) {
    RenameOptions rename_option;
    rename_option.overwrite = true;
    return RenameImplWithOptions(old_file_name, new_file_name, rename_option, error_code);
}

int32_t LocalFile::RenameImplWithOptions(const std::string& old_file_name,
                                         const std::string& new_file_name,
                                         const RenameOptions& rename_option,
                                         uint32_t*   error_code) {
    SetErrorCode(error_code, ERR_FILE_OK);

    // rename library function allow old and new file name are the same.
    // LocalFile and XFSFile don't allow to avoid mis-operations.
    if (File::IsSameFilePath(old_file_name, new_file_name)) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }

    int32_t ret = -1;
    if (!rename_option.overwrite && ACCESS(new_file_name.c_str(), 0) == 0) {
        SetErrorCode(error_code, ERR_FILE_ENTRY_EXIST);
        return ret;
    }
    ret = rename(old_file_name.c_str(), new_file_name.c_str());
    if (ret != 0) {
        SetErrorCodeByLocal(error_code, errno);
    }
    return ret;
}

int32_t LocalFile::RemoveImpl(const char* file_name, bool is_recursive, uint32_t* error_code) {
    SetErrorCode(error_code, ERR_FILE_OK);
    int32_t ret = -1;
    size_t len = strlen(file_name);
    if (!file_name || len <= 0) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }
#ifdef _WIN32
    int32_t file_exist = _access(file_name, 0);
    if (file_exist != 0) {
        SetErrorCode(error_code, ERR_FILE_ENTRY_NOT_EXIST);
        LOG(ERROR) << "file " << file_name << " not exists";
        return ret;
    }

    char complete_file_name[MAX_PATH] = {0};
    bool has_directory = false;
    if (file_name[len - 1] == '/' ||
            file_name[len - 1] == '\\') {
        if (!is_recursive) {
            int32_t is_success = RemoveDirectory(file_name);
            if (is_success == 0) {
                ret = -1;
                SetErrorCode(error_code, ERR_FILE_FAIL);
            } else {
                SetErrorCode(error_code, ERR_FILE_OK);
                ret = 0;
            }
            return ret;
        }
        strcat(complete_file_name, file_name); // NOLINT
        complete_file_name[len - 1] = '\\';
        strcat(complete_file_name, "*.*"); // NOLINT
        has_directory = true;
    } else if (file_name[len - 1] == ':') { // c:, d:
        if (!is_recursive) {
            int32_t is_success = RemoveDirectory(file_name);
            if (is_success == 0) {
                ret = -1;
                SetErrorCode(error_code, ERR_FILE_FAIL);
            } else {
                SetErrorCode(error_code, ERR_FILE_OK);
                ret = 0;
            }
            return ret;
        }
        strcat(complete_file_name, file_name); // NOLINT
        strcat(complete_file_name, "\\*.*"); // NOLINT
        has_directory = true;
    } else {
        SetFileAttributes(file_name, FILE_ATTRIBUTE_NORMAL);
        int32_t delete_ret = DeleteFile(file_name);
        if (delete_ret == 0) {
            SetErrorCode(error_code, ERR_FILE_FAIL);
            return -1;
        } else {
            SetErrorCode(error_code, ERR_FILE_OK);
            return 0;
        }
    }

    WIN32_FIND_DATA file_data;
    HANDLE find_handle=::FindFirstFile(complete_file_name, &file_data);

    if (INVALID_HANDLE_VALUE == find_handle) {
        ret = -1;
        SetErrorCode(error_code, ERR_FILE_FAIL);
        LOG(ERROR) << "file " << file_name << " not exists";
        return ret;
    }

    ret = 0;

    do {
        if (strcmp(file_data.cFileName, ".") == 0 || strcmp(file_data.cFileName, "..") == 0 )
            continue;
        bool is_dir = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        char* fname = file_data.cFileName;
        if (strcmp(fname, ".") == 0 || strcmp(fname, "..") == 0)
            continue;

        std::string new_file_name =
            ConnectPathComponent(file_name, fname);
        if (is_dir) {
            if (!is_recursive) {
                ret = -1;
                SetErrorCode(error_code, ERR_FILE_FAIL);
                break;
            }
            has_directory = true;
            new_file_name += "/";
            ret = Remove(new_file_name.c_str(), is_recursive, error_code);
        } else {
            SetFileAttributes(new_file_name.c_str(), FILE_ATTRIBUTE_NORMAL);
            DeleteFile(new_file_name.c_str());
        }
    } while (FindNextFile(find_handle, &file_data) && ret == 0);

    ::FindClose(find_handle);
    if (has_directory) {
        RemoveDirectory(file_name);
    }

    SetErrorCode(error_code, ret == 0 ? ERR_FILE_OK : ERR_FILE_FAIL);
    return ret;
#else
    int32_t file_exist = access(file_name, F_OK);
    if (file_exist != 0) {
        SetErrorCode(error_code, ERR_FILE_ENTRY_NOT_EXIST);
        LOG(ERROR) << "file " << file_name << " not exists";
        return -1;
    }

    DIR * dir_ptr = NULL;
    struct dirent * dirent_ptr = NULL;
    struct stat stat_buf;

    if (stat(file_name, &stat_buf) < 0) {
        ret = -1;
        SetErrorCodeByLocal(error_code, errno);
        LOG(ERROR) << "stat error for " << file_name;
        return ret;
    }

    // Directory
    if (S_ISDIR(stat_buf.st_mode)) {
        if (!is_recursive) {
            // use rmdir(2) rather than remove(3) to delete empty folders.
            // Because since libc4, remove is the same as unlink and can not delete folder.
            ret = rmdir(file_name);
            if (ret == 0) {
                SetErrorCode(error_code, ERR_FILE_OK);
            } else {
                SetErrorCodeByLocal(error_code, errno);
            }

            return ret;
        }

        ret = 0; // first reset values
        dir_ptr = opendir(file_name);
        while ((dirent_ptr = readdir(dir_ptr)) != NULL && ret == 0) { // NOLINT
            if (strcmp(dirent_ptr->d_name, ".") == 0 || strcmp(dirent_ptr->d_name, "..") == 0)
                continue;

            const std::string& complete_file_name =
                ConnectPathComponent(file_name, dirent_ptr->d_name);
            ret = LocalFile::RemoveImpl(complete_file_name.c_str(), is_recursive, error_code);
        }
        closedir(dir_ptr);
        if (ret != 0) {
            SetErrorCodeByLocal(error_code, errno);
            return ret;
        }
        // Remove the directory itself.
        ret = rmdir(file_name);
    } else {
        // Normal file
        ret = unlink(file_name);
    }

    if (ret == 0) {
        SetErrorCode(error_code, ERR_FILE_OK);
    } else {
        SetErrorCodeByLocal(error_code, errno);
    }

    return ret;
#endif
}

int32_t LocalFile::AddDirImpl(const char* file_name, uint32_t* error_code) {
    int32_t ret = -1;

    bool is_exist = LocalFile::CheckExistImpl(file_name, error_code);
    if (is_exist) {
        SetErrorCode(error_code, ERR_FILE_ENTRY_EXIST);
        return ret;
    }

#ifdef _WIN32
    int32_t is_success = 0;
    is_success = CreateDirectory(file_name, 0);
    if (is_success != 0) {
        SetErrorCode(error_code, ERR_FILE_OK);
        ret = 0;
    } else {
        SetErrorCode(error_code, ERR_FILE_FAIL);
        ret = -1;
    }
    return ret;
#else
    ret = mkdir(file_name, 0777);
    if (ret != 0) {
        SetErrorCodeByLocal(error_code, errno);
    } else {
        SetErrorCode(error_code, ERR_FILE_OK);
    }
    return ret;
#endif
}

int64_t LocalFile::GetSizeImpl(const char* file_name, uint32_t* error_code) {
    int64_t file_size = -1;
#ifdef _WIN32
    if (!file_name) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        if (!error_code) LOG(ERROR) << "input file name is NULL";
        return file_size;
    }

    int32_t file_exist = _access(file_name, 0);
    if (file_exist != 0) {
        SetErrorCode(error_code, ERR_FILE_ENTRY_NOT_EXIST);
        if (error_code == NULL) {
            LOG(ERROR) << "file " << file_name << " not exists";
        }
        return file_size;
    }

    size_t len = strlen(file_name);
    if (len < 1) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return file_size;
    }

    if (file_name[len - 1] == '/' ||
            file_name[len - 1] == '\\'||
            file_name[len - 1] == ':' ) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        if (!error_code) LOG(ERROR) << "input file name " << file_name << " is a directory";
        return file_size;
    }

    WIN32_FIND_DATA file_data;
    HANDLE find_handle=::FindFirstFile(file_name, &file_data);

    if (INVALID_HANDLE_VALUE == find_handle) {
        file_size = 0;
        // Hidden files in Windows
        SetErrorCode(error_code, ERR_FILE_OK);
        return file_size;
    }

    // file size to return
    file_size = static_cast<int64_t>(file_data.nFileSizeHigh) * 1024 * 1024 * 1024 * 4 +
                file_data.nFileSizeLow;

    ::FindClose(find_handle);
    SetErrorCode(error_code, ERR_FILE_OK);
    return file_size;

#else
    int32_t file_exist = access(file_name, F_OK);
    if (file_exist != 0) {
        SetErrorCode(error_code, ERR_FILE_ENTRY_NOT_EXIST);
        if (error_code == NULL) {
            LOG(ERROR) << "file " << file_name << " not exists";
        }
        return file_size;
    }

    struct stat stat_buf;

    if (stat(file_name, &stat_buf) < 0) {
        file_size = -1;
        SetErrorCodeByLocal(error_code, errno);
        LOG(ERROR) << "stat error for " << file_name;
        return file_size;
    }

    if (S_ISDIR(stat_buf.st_mode)) {
        file_size = -1;
        SetErrorCodeByLocal(error_code, errno);
        LOG(ERROR) << "input file name " << file_name << " is a directory";
        return file_size;
    } else {
        SetErrorCode(error_code, ERR_FILE_OK);
        file_size = static_cast<int64_t>(stat_buf.st_size);
    }
    return file_size;
#endif
}

int32_t LocalFile::LocateData(uint64_t start_pos, uint64_t end_pos,
                              std::vector<DataLocation>* buf, uint32_t* error_code) {
    CHECK_HANDLE(m_fp);
    if (!buf || (end_pos < start_pos)) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        LOG(ERROR) << "parameter error";
        return -1;
    }

    // not implement
    SetErrorCode(error_code, ERR_FILE_FAIL);
    return -1;
}


void LocalFile::DoAsyncWrite(
    const void* buf,
    int64_t buf_size,
    Closure<void, int64_t, uint32_t>* callback,
    uint32_t time_out) {
    uint32_t error_code;
    int64_t writen_size = LocalFile::Write(buf, buf_size, &error_code);
    // callback->Run(writen_size, error_code);
    Closure<void>* task =
        NewClosure(
            callback,
            &Closure<void, int64_t, uint32_t>::Run,
            writen_size,
            error_code);
    g_local_thread_pool->AddTask(task);
}

void LocalFile::DoAsyncReadFrom(
    void* buffer,
    int64_t size,
    int64_t start_position,
    Closure<void, int64_t, uint32_t>* callback,
    uint32_t timeout) {
    uint32_t error_code = ERR_FILE_OK;
    int64_t read_size = 0;

    #ifdef _WIN32
    {
        MutexLocker lock(m_mutex);
        if (start_position == LocalFile::Seek(start_position, SEEK_SET, &error_code)) {
            // only read when seek OK.
            read_size = LocalFile::Read(buffer, size, &error_code);
        }
    }
    #else
    int fd = fileno(m_fp);
    read_size = pread64(fd, buffer, size, start_position);
    if (read_size < 0) {
        error_code = TranslateLocalErrorCode(errno);
    }
    #endif

    // callback->Run(read_size, error_code);
    Closure<void>* task =
        NewClosure(
            callback,
            &Closure<void, int64_t, uint32_t>::Run,
            read_size,
            error_code);
    g_local_thread_pool->AddTask(task);
}

void LocalFile::AsyncReadFromAIOCallback(
    Closure<void, int64_t, uint32_t>* out_callback,
    int64_t size,
    uint32_t status_code) {
    uint32_t error_code = ERR_FILE_OK;

    // read size 0 is error.
    // Because we check start_position is less than file size before submitting read request.
    // When reading, the file is truncated so read size 0, return fail.
    if (status_code != 0u || size <= 0) {
        // when has AIO errors, status_code may still be 0, so must check size.
        error_code = status_code != 0u ? TranslateLocalErrorCode(status_code) : ERR_FILE_FAIL;
        if (size < 0) {
            LOG(ERROR) << "AsyncReadFromAIOCallback with status code " << status_code
                << " size " << size;
        }
    }

    out_callback->Run(size, error_code);
}

int32_t LocalFile::AsyncReadFrom(void* buffer,
                                 int64_t size,
                                 int64_t start_position,
                                 Closure<void, int64_t, uint32_t>* callback,
                                 uint32_t timeout,
                                 uint32_t* error_code) {
    CHECK_HANDLE(m_fp);
    if (!buffer || size <= 0 || start_position < 0) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }
#ifdef _WIN32
    Closure<void>* task =
        NewClosure(
            this,
            &LocalFile::DoAsyncReadFrom,
            buffer, size, start_position, callback, timeout);
    g_local_thread_pool->AddTask(task);

    SetErrorCode(error_code, ERR_FILE_OK);
    return 0;
#else
    CHECK(g_aioframe != NULL)
        << "Forget to call File::Init? Should Init before use async operations";

    int64_t fsize = LocalFile::Seek(0, SEEK_END);

    if (start_position >= fsize) { // Reading any position exceeds file size is EOF.
        SetErrorCode(error_code, ERR_FILE_OK);

        Closure<void>* task =
            NewClosure(
                callback,
                &Closure<void, int64_t, uint32_t>::Run,
                static_cast<int64_t>(0),
                static_cast<uint32_t>(ERR_FILE_OK));
        g_local_thread_pool->AddTask(task);

        return 0;
    }

    Closure<void, int64_t, uint32_t>* aio_callback =
        NewClosure(this, &LocalFile::AsyncReadFromAIOCallback, callback);
    common::AIOFrame::StatusCode code;
    int fd = fileno(m_fp);
    bool success = g_aioframe->AsyncRead(fd, buffer, size, start_position, aio_callback, &code);
    if (error_code != NULL) {
        *error_code = ERR_FILE_OK;
        if (!success) {
            *error_code = TranslateLocalErrorCode(code);
        }
    }

    return success ? 0 : -1;
#endif
}

bool LocalFile::CheckOpenModeValid(const uint32_t flags) {
    // Could not both W and A, or both W and R, or both A and R
    uint32_t invalid_mode
        = File::ENUM_FILE_OPEN_MODE_W | File::ENUM_FILE_OPEN_MODE_A;
    bool valid = (flags & invalid_mode) != invalid_mode;
    if (!valid) return false;

    invalid_mode
        = File::ENUM_FILE_OPEN_MODE_W | File::ENUM_FILE_OPEN_MODE_R;
    valid = (flags & invalid_mode) != invalid_mode;
    if (!valid) return false;

    invalid_mode
        = File::ENUM_FILE_OPEN_MODE_A | File::ENUM_FILE_OPEN_MODE_R;
    valid = (flags & invalid_mode) != invalid_mode;
    if (!valid) return false;

    return true;
}

int32_t LocalFile::ValidatePathWildcard(const char* file_path) {
    if (file_path == NULL) return -1;

    bool has_wildcard = false;

    size_t path_len = strlen(file_path);
    for (size_t i = 0; i < path_len; ++i) {
        char ch = file_path[i];
        if (ch == '\\') return false; // forbidden windows style seperator. For windows also use /
        if (ch == '*' || ch == '?' || ch == '[') has_wildcard = true;
        if (ch == '/' && has_wildcard) return -1; // not in last path component
    }

    return has_wildcard ? 1 : 0;
}

bool LocalFile::GetModeImpl(const std::string& path_name,
                            uint32_t* permission,
                            uint32_t* error_code) {

    struct stat stat_buf;
    if (stat(path_name.c_str(), &stat_buf) < 0) {
        LOG(ERROR) << "stat error for " << path_name;
        *permission = 0;
        SetErrorCodeByLocal(error_code, errno);
        return false;
    }
    *permission = stat_buf.st_mode & 0777;
    return true;
}

bool LocalFile::ChmodImpl(const char* path_name,
                          const uint32_t permission,
                          uint32_t* error_code)
{
    if (!path_name || permission > 0777u) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        LOG(ERROR) << "Invalid param";
        return false;
    }
#ifndef _WIN32
    if (chmod(path_name, permission) < 0) {
        SetErrorCodeByLocal(error_code, errno);
        return false;
    }
    SetErrorCode(error_code, ERR_FILE_OK);
    return true;
#endif
    SetErrorCode(error_code, ERR_FILE_FAIL);
    return false;
}

bool LocalFile::ChangeRoleImpl(const char* path_name,
                               const char* role_name,
                               uint32_t* error_code)
{
    if (!path_name || !role_name) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        LOG(ERROR) << "Invalid param";
        return false;
    }

#ifndef _WIN32
    std::string buf(role_name);
    std::vector<std::string> arr;
    SplitString(buf, ":", &arr);
    if (arr.size() != 2) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        LOG(ERROR) << "Invalid role_name format, user_name:group_name";
        return false;
    }
    uid_t uid = 0;
    gid_t gid = 0;

    {
        MutexLocker lock(m_mutex);
        struct passwd* pw = getpwnam(arr[0].c_str()); // NOLINT
        if (!pw) {
            SetErrorCodeByLocal(error_code, errno);
            LOG(ERROR) << "get uid FAIL";
            return false;
        }
        uid = pw->pw_uid;
        struct group* gr = getgrnam(arr[1].c_str()); // NOLINT
        if (!gr) {
            SetErrorCodeByLocal(error_code, errno);
            LOG(ERROR) << "get gid FAIL";
            return false;
        }
        gid = gr->gr_gid;
    }

    if (0 == chown(path_name, uid, gid)) {
        SetErrorCode(error_code, ERR_FILE_OK);
        return true;
    } else {
        SetErrorCodeByLocal(error_code, errno);
        LOG(ERROR) << "Chown fail, error: " << strerror(errno);
        return false;
    }
#endif
    SetErrorCode(error_code, ERR_FILE_FAIL);
    return false;
}

bool LocalFile::ChangeSecondRoleImpl(const char* path_name,
                                     const char* second_role_name,
                                     bool is_recursive,
                                     uint32_t* error_code) {
    LOG(FATAL) << "Not implement LocalFile::ChangeSecondRoleImpl yet.";
    return false;
}

bool LocalFile::GetDigestImpl(File* file_obj,
                              int64_t file_size,
                              uint32_t* digest,
                              uint32_t* error_code)
{
    const uint32_t k4M  = 4 * 1024 * 1024;
    const uint32_t k64M = 16 * k4M;
    uint32_t chunk_num = (file_size / k64M) + ((file_size % k64M) ? 1 : 0);

    unsigned char* buf = new unsigned char[k4M];
    scoped_array<unsigned char> scoped_buf(buf);

    uint32_t total_crc = 0xffffffff;
    for (uint32_t i = 0; i < chunk_num; ++i) {
        uint32_t temp_crc = 0xffffffff;
        uint32_t cur_pos = 0;
        uint32_t end_pos = k64M;
        if (i == (chunk_num - 1)) end_pos = (file_size - i * k64M);
        while (cur_pos < end_pos) {
            int64_t read_len = std::min(end_pos - cur_pos, k4M);
            int64_t ret_len = file_obj->Read(buf, read_len, error_code);
            if (ret_len > 0) {
                temp_crc = lite_crc::FastCRC(temp_crc, buf, ret_len);
            } else {
                // reserve the error_code of Read
                return false;
            }
            cur_pos += ret_len;
        }
        total_crc = lite_crc::FastCRC(total_crc,
            reinterpret_cast<unsigned char*>(&temp_crc),
            sizeof(temp_crc));
    }

    *digest = total_crc;
    SetErrorCode(error_code, ERR_FILE_OK);
    return true;
}

std::string LocalFile::GetBaseName(const std::string& file_path) {
    if (file_path.empty()) return file_path;
    // ignore directory.
    if (file_path[file_path.size() - 1] == File::kPathSeparator) return std::string();

    int32_t sep_pos = file_path.find_last_of(File::kPathSeparator);
    if (static_cast<size_t>(sep_pos) == std::string::npos) sep_pos = -1; // the whole string.

    return file_path.substr(sep_pos + 1); // ignore the kSeperator itself.
}

} // namespace common
