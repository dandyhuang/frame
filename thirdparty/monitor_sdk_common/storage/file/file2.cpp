// Copyright (c) 2015, Vivo Inc. All rights reserved.
//////////////////////////////////////////////////////////////////////////
// @file:   file.cc
//                      Refactor to use polymorphism to forward to subclasses.
//////////////////////////////////////////////////////////////////////////

#ifndef _WIN32
#include <pthread.h>
#include <sys/un.h>
#endif

#include <stdlib.h>

#include <algorithm>
#include <map>
#include <vector>

#include "thirdparty/monitor_sdk_common/base/module.h"
#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/base/stdext/string.h"
#include "thirdparty/monitor_sdk_common/base/string/format.h"
#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/base/string/concat.h"
#include "thirdparty/monitor_sdk_common/storage/file/file2.h"
#include "thirdparty/monitor_sdk_common/storage/file/local_file2.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"

#include "thirdparty/glog/logging.h"

namespace common {

DEFINE_MODULE(file) {
    return File::Init();
}

DEFINE_MODULE_DTOR(file) {
    File::CleanUp();
}

} // namespace common

namespace {

// mutex to protect local file operations.
common::SimpleMutex g_file_mutex;
// Ref count for Init and Cleanup.
int32_t g_init_count = 0;
// init result, multiple init return it
bool g_init_result = false;

}

namespace common {

#ifndef _WIN32
    // don't define on windows
    const char File::kPathSeparator;
    // 异步操作的默认超时时间（单位:s）
    const uint32_t File::kDefaultAsyncTimeout;
#endif

//////////////////////////////////////////////////////////////////////////
// 成员函数

std::string File::GetFilePrefix(const std::string& file_path) {
    std::string prefix;

    if (file_path.empty()) {
        return prefix;
    }

    // skip the head segment.
    size_t idx = file_path.find(kPathSeparator, 1);
    if (idx == std::string::npos) {
        return prefix;
    }

    return file_path.substr(0, idx + 1);
}

File* File::CreateFileImpl(const std::string& prefix) {
    File* file_impl = CREATE_FILE_IMPL(prefix);
    if (file_impl == NULL) {
        file_impl = CREATE_FILE_IMPL(LOCAL_FILE_PREFIX);
    }
    CHECK(file_impl != NULL)
        << "Do you forget to link the real file implement library?"
        << ", prefix : " << prefix;
    return file_impl;
}

File* File::GetFileImplSingleton(const std::string& prefix) {
    File* file_impl = GET_FILE_IMPL_SINGLETON(prefix);
    if (file_impl == NULL) {
        file_impl = GET_FILE_IMPL_SINGLETON(LOCAL_FILE_PREFIX);
    }
    CHECK(file_impl != NULL)
        << "Do you forget to link the real file implement library singleton?"
        << ", prefix : " << prefix;
    return file_impl;
}

File* File::GetFileImplByPath(const std::string& path) {
    std::string prefix = GetFilePrefix(path);
    File* file_impl = GetFileImplSingleton(prefix);
    return file_impl;
}

bool File::Init() {
    // reference count
    MutexLocker locker(&g_file_mutex);
    ++g_init_count;
    if (g_init_count != 1) {
        // 后续的重复init根据第一次init结果返回
        return g_init_result;
    }

    // init all file impl.
    bool has_error = false;
    std::string success_impls;
    std::string fail_impls;
    for (size_t i = 0; i < FILE_IMPL_COUNT(); ++i) {
        std::string file_impl_name = FILE_IMPL_NAME(i);
        File *file_impl = GetFileImplSingleton(file_impl_name);

        if (file_impl->InitImpl()) {
            success_impls += file_impl_name + ' ';
        } else {
            fail_impls += file_impl_name + ' ';
            has_error = true;
        }
    }

    if (FILE_IMPL_COUNT() == 0U) {
        LOG(WARNING) << " No file implementation is initilized. "
            << "Forget to link any libraries such as xfs_file or local_file.";
    }

    std::string msg;
    StringFormatAppend(&msg,
                       "%s to init all %u file implementations, success: %s %s %s",
                       (has_error ? "Fail" : "Success"),
                       static_cast<uint32_t>(FILE_IMPL_COUNT()),
                       success_impls.c_str(),
                       (has_error ? "Fail:" : ""),
                       fail_impls.c_str());

    (has_error ? LOG(ERROR) : LOG(INFO)) << msg;

    g_init_result = !has_error;
    return g_init_result;
}

void File::CleanUp() {
    MutexLocker locker(&g_file_mutex);
    --g_init_count;

    // 表示cleanup次数多于init
    if (g_init_count < 0) {
        // 置0，保证用户在同一进程中再init时，行为正常
        LOG(ERROR) << "CleanUp is more than init";
        g_init_count = 0;
        return;
    }

    // 表示cleanup次数少于init
    if (g_init_count > 0) {
        return;
    }

    // cleanup all file impl.
    bool has_error = false;
    std::string success_impls;
    std::string fail_impls;
    for (size_t i = 0; i < FILE_IMPL_COUNT(); ++i) {
        std::string file_impl_name = FILE_IMPL_NAME(i);
        File *file_impl = GetFileImplSingleton(file_impl_name);

        if (file_impl->CleanupImpl()) {
            success_impls += file_impl_name + ' ';
        } else {
            fail_impls += file_impl_name + ' ';
            has_error = true;
        }
    }

    std::string msg;
    StringFormatAppend(&msg,
                       "%s to cleanup all %u file implementations, success: %s %s %s",
                       (has_error ? "Fail" : "Success"),
                       static_cast<uint32_t>(FILE_IMPL_COUNT()),
                       success_impls.c_str(),
                       (has_error ? "Fail:" : ""),
                       fail_impls.c_str());

    (has_error ? LOG(ERROR) : LOG(INFO)) << msg;
}

File* File::Open(const std::string& file_path,
                 uint32_t flags,
                 const OpenFileOptions& options,
                 uint32_t* error_code) {
    if (file_path.empty()) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return NULL;
    }
    std::string prefix = GetFilePrefix(file_path);
    // Must create a new File object and return.
    File* file_obj = CreateFileImpl(prefix);

    if (true == file_obj->OpenImpl(file_path.c_str(), flags, options, error_code)) {
        return file_obj;
    } else {
        delete file_obj;
        return NULL;
    }
}

File* File::Open(const std::string& file_path, const char* mode,
                 uint32_t* error_code) {
    uint32_t flags = 0;
    std::string open_mode = RemoveAll(mode, "b");
    if (open_mode == "r") {
        flags = File::ENUM_FILE_OPEN_MODE_R;
    } else if (open_mode == "w") {
        flags = File::ENUM_FILE_OPEN_MODE_W;
    } else if (open_mode == "a") {
        flags = File::ENUM_FILE_OPEN_MODE_A;
    } else {
        SetErrorCode(error_code, ERR_FILE_OPENMODE);
        return NULL;
    }

    return Open(file_path, flags, OpenFileOptions(), error_code);
}


int32_t File::Copy(const std::string&   src_file_path,
                   const std::string&   dest_file_path,
                   uint32_t*     error_code) {
    if (src_file_path.empty() || dest_file_path.empty()) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }
    // Don't allow copy the same file.
    if (File::IsSameFilePath(src_file_path, dest_file_path)) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }

    int32_t   ret = 0;
    //////////////////////////////////////////////////////////////////////////
    // 1.打开原文件
    File* src_file = File::Open(src_file_path,
                                File::ENUM_FILE_OPEN_MODE_R,
                                OpenFileOptions(), error_code);
    if (!src_file) {
        return -1;
    }
    //////////////////////////////////////////////////////////////////////////
    // 2.得到原文件的长度
    std::vector<AttrsInfo> file_info;
    ListOptions list_option;
    list_option.mask.file_size = 1;
    list_option.mask.file_permission = 1;
    int32_t list_ret = File::List(src_file_path, list_option, &file_info, error_code);

    if (list_ret == -1 || file_info.size() != 1u) {
        src_file->Close();
        delete src_file;
        src_file = NULL;
        return -1;
    }
    int64_t src_file_len = file_info[0].file_size;
    uint32_t file_permission = file_info[0].file_permission;
    //////////////////////////////////////////////////////////////////////////
    // 3.打开或创建目标文件
    File* dest_file = File::Open(dest_file_path,
                                 File::ENUM_FILE_OPEN_MODE_W,
                                 OpenFileOptions(), error_code);
    if (!dest_file) {
        src_file->Close();
        delete src_file;
        src_file = NULL;
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////
    // 4.计算copy的range范围
    uint64_t remain_len_in_src = src_file_len;  // 还有多少数据需要从源文件获取

    // 5.移动到读的位置
    src_file->Seek(0, SEEK_SET);
    // 6.计划读取多长数据
    uint64_t plan_read_count = src_file_len;

    uint32_t buff_len = 4 * 1024 * 1024; // 4M
    char*    buff     = new char[buff_len];

    uint64_t all_write_count = 0;
    uint32_t received_errcode = ERR_FILE_OK;
    // 移动到指定位置
    src_file->Seek(0, SEEK_SET, error_code);
    while (remain_len_in_src > 0 && ret != -1) {
        // 同步读取文件
        int64_t max_read_len = (remain_len_in_src > buff_len) ?
                                buff_len : static_cast<uint32_t>(remain_len_in_src);
        int64_t read_count = src_file->Read(buff, max_read_len, &received_errcode);
        if (read_count == -1) {
            ret = -1;
            break;
        }
        else if (read_count > 0) {
            remain_len_in_src -= read_count;
            while (read_count > 0) {
                int64_t write_count = dest_file->Write(buff, read_count, &received_errcode);
                if (write_count <= 0) { // read-count>0, 那么write_count不能小于等于0
                    ret = -1;
                    break;
                }
                else if (write_count > 0) {
                    all_write_count += write_count;
                    read_count -= write_count;
                }
            }
        }
        else if (read_count == 0) { // read over
            break;
        }
    }

    uint32_t code = ERR_FILE_OK;
    bool is_close_success = true;
    if (src_file->Close(&code) != 0) {
        is_close_success = false;
        if (received_errcode == ERR_FILE_OK) {
            received_errcode = code;
        }
    }
    if (dest_file->Close(&code) != 0) {
        is_close_success = false;
        if (received_errcode == ERR_FILE_OK) {
            received_errcode = code;
        }
    }
    if (is_close_success) {
        is_close_success = File::Chmod(dest_file_path, file_permission, &code);
    }
    if (!is_close_success && received_errcode == ERR_FILE_OK) {
        received_errcode = code;
    }

    delete []buff;
    buff = NULL;

    delete src_file;
    src_file = NULL;

    delete dest_file;
    dest_file = NULL;

    if ((all_write_count == plan_read_count) && is_close_success) {
        SetErrorCode(error_code, ERR_FILE_OK);
        ret = 0;
    } else {
        SetErrorCode(error_code, static_cast<FILE_ERROR_CODE>(received_errcode));
        ret = -1;
    }
    return ret;
}

int32_t File::List(const std::string& name,
                   const ListOptions& list_option,
                   std::vector<AttrsInfo>* attrs,
                   uint32_t* error_code) {
    if (name.empty() || !attrs) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }
    // Add list for other fs type here
    File* file_impl = GetFileImplByPath(name);
    return file_impl->ListImpl(name.c_str(), list_option, attrs, error_code);
}

bool File::GetAttrs(const std::string& name,
                    const AttrsMask& mask,
                    AttrsInfo* attrs_info,
                    uint32_t* error_code) {
    if (name.empty() || !attrs_info) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return false;
    }
    if (name.find_first_of("*?") != std::string::npos) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return false;
    }
    File* file_impl = GetFileImplByPath(name);
    return file_impl->GetAttrsImpl(name, mask, attrs_info, error_code);
}

bool File::CheckExist(const std::string& name, uint32_t* error_code) {
    return File::IsExist(name, error_code);
}

bool File::IsExist(const std::string& name, uint32_t* error_code) {
    if (name.empty()) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return false;
    }
    File* file_impl = GetFileImplByPath(name);
    return file_impl->CheckExistImpl(name.c_str(), error_code);
}

int64_t File::Du(const std::string& name, uint32_t* error_code) {
    if (name.empty()) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }
    File* file_impl = GetFileImplByPath(name);
    return file_impl->DuImpl(name.c_str(), error_code);
}

bool File::GetContentSummary(const std::string& path_name,
                             FileContentSummaryMask* mask,
                             FileContentSummary* summary,
                             uint32_t* error_code) {
    if (path_name.empty() || !mask || !summary) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return false;
    }
    File* file_impl = GetFileImplByPath(path_name);
    return file_impl->GetContentSummaryImpl(path_name.c_str(), mask, summary, error_code);
}

int32_t File::Move(const std::string& old_file_name,
                   const std::string& new_file_name,
                   uint32_t* error_code) {
    // Use MoveImpl instead of Rename for polymorphism.
    // return File::Rename(src_name, dest_name, error_code);
    if (old_file_name.empty() || new_file_name.empty()) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }
    // Don't do other checks here. Leave the flexibility to file implementations.

    // TODO(aaronzou): How to handle the different src and dest file type?
    std::string prefix = GetFilePrefix(old_file_name);
    std::string dest_prefix = GetFilePrefix(new_file_name);

    File* file_impl = GetFileImplSingleton(prefix);
    File* dest_file_impl = GetFileImplSingleton(dest_prefix);

    if (file_impl != dest_file_impl) {
        LOG(ERROR) << "file impl mismatch? src prefix " << prefix << " dst prefix " << dest_prefix;
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }
    return file_impl->MoveImpl(old_file_name.c_str(), new_file_name.c_str(), error_code);
}

int32_t File::Rename(const std::string& old_file_name,
                     const std::string& new_file_name,
                     uint32_t* error_code) {
    if (!File::IsRenameParamValid(old_file_name, new_file_name, error_code)) {
        return -1;
    }
    std::string prefix = GetFilePrefix(old_file_name);
    File* file_impl = GetFileImplSingleton(prefix);

    return file_impl->RenameImpl(old_file_name.c_str(), new_file_name.c_str(), error_code);
}

int32_t File::Rename(const std::string& old_file_name,
                     const std::string& new_file_name,
                     const RenameOptions& rename_option,
                     uint32_t* error_code) {
    if (!File::IsRenameParamValid(old_file_name, new_file_name, error_code)) {
        return -1;
    }
    std::string prefix = GetFilePrefix(old_file_name);
    File* file_impl = GetFileImplSingleton(prefix);

    return file_impl->RenameImplWithOptions(old_file_name, new_file_name, rename_option,
                                            error_code);
}

int32_t File::Remove(const std::string& file_path, bool is_recursive, uint32_t* error_code) {
    if (file_path.empty()) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }
    File* file_impl = GetFileImplByPath(file_path);
    return file_impl->RemoveImpl(file_path.c_str(), is_recursive, error_code);
}

int32_t File::AddDir(const std::string& file_path, uint32_t* error_code) {
    if (file_path.empty()) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }

    File* file_impl = GetFileImplByPath(file_path);
    return file_impl->AddDirImpl(file_path.c_str(), error_code);
}

bool File::AddDirRecursively(const std::string& path, uint32_t* error_code) {
    bool ret = true;
    if (path.empty()) {
        LOG(WARNING) << "mkdir: path empty";
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return false;
    }

    bool is_xfs = StringStartsWith(path, "/xfs/");
    std::vector<std::string> segments;
    SplitString(path, "/", &segments);
    if (is_xfs && segments.size() <= 2) {
        LOG(WARNING) << "mkdir: invalid xfs path " << path;
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return false;
    }

    uint32_t pos = 0;
    std::string buf;
    if (is_xfs) {
        buf = StringConcat(File::kPathSeparator, segments[0],
                           File::kPathSeparator, segments[1],
                           File::kPathSeparator);
        pos = 2;
    } else {
        bool is_absolute_path = StringStartsWith(path, "/");
        if (is_absolute_path) {
            buf = "/";
        } else {
            buf = "";
        }
    }

    // Create each directory
    SetErrorCode(error_code, ERR_FILE_OK);
    for (; pos < segments.size(); ++pos) {
        buf += segments[pos] + File::kPathSeparator;
        if (!File::CheckExist(buf)) {
            uint32_t internal_error_code = 0;
            if (File::AddDir(buf, &internal_error_code) != 0) {
                if (internal_error_code != ERR_FILE_ENTRY_EXIST) {
                    SetErrorCode(error_code,
                                 static_cast<FILE_ERROR_CODE>(internal_error_code));
                    ret = false;
                    break;
                }
            }
        }
    }

    return ret;
}

int64_t File::GetSize(const std::string& file_path, uint32_t* error_code) {
    if (file_path.empty()) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }
    File* file_impl = GetFileImplByPath(file_path);
    return file_impl->GetSizeImpl(file_path.c_str(), error_code);
}


File* File::OpenOrDie(const std::string& file_path, uint32_t flags,
    const OpenFileOptions& options) {
    File* fp = File::Open(file_path, flags, options);
    if (fp == NULL) {
        // log and exit.
        LOG(FATAL) << "failed to open " << file_path << " with flags " << flags;
    }
    return fp;
}

int32_t File::GetMatchingFiles(const std::string& pattern,
                               std::vector<std::string>* files,
                               uint32_t* error_code,
                               const ListOptions* options) {
    if (pattern.empty() || !files) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }
    ListOptions list_options;
    if (options) {
        list_options.count = options->count;
        list_options.last_file = options->last_file;
    }
    std::vector<AttrsInfo> file_info;
    int32_t ret = File::List(pattern, list_options, &file_info, error_code);
    if (ret == 0) {
        files->reserve(file_info.size());
        for (std::vector<AttrsInfo>::iterator iter = file_info.begin();
                iter != file_info.end();
                ++iter) {
            files->push_back(iter->file_name);
        }
    }
    return ret;
}

bool File::GetMode(const std::string& path_name,
                   uint32_t* permission,
                   uint32_t* error_code) {
    if (path_name.empty()) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return false;
    }
    File* file_impl = GetFileImplByPath(path_name);
    return file_impl->GetModeImpl(path_name, permission, error_code);
}

bool File::Chmod(const std::string& path_name,
                 const uint32_t permission,
                 uint32_t* error_code) {
    if (path_name.empty()) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return false;
    }
    File* file_impl = GetFileImplByPath(path_name);
    return file_impl->ChmodImpl(path_name.c_str(), permission, error_code);
}

bool File::ChangeRole(const std::string& path_name,
                      const std::string& role_name,
                      uint32_t* error_code) {
    if (path_name.empty() || role_name.empty()) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return false;
    }
    File* file_impl = GetFileImplByPath(path_name);
    return file_impl->ChangeRoleImpl(path_name.c_str(), role_name.c_str(), error_code);
}

bool File::ChangeSecondRole(const std::string& path_name,
                            const std::string& second_role_name,
                            bool is_recursive,
                            uint32_t* error_code) {
    if (path_name.empty()) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return false;
    }
    File* file_impl = GetFileImplByPath(path_name);
    return file_impl->ChangeSecondRoleImpl(path_name.c_str(), second_role_name.c_str(),
                                           is_recursive, error_code);
}

bool File::GetDigest(const std::string& path_name, uint32_t* digest, uint32_t* error_code) {
    if (path_name.empty() || !digest) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return false;
    }

    int64_t fsize = GetSize(path_name, error_code);
    if (fsize < 0) {
        LOG(ERROR) << "Get size:[" << path_name << "] fail.";
        return false;
    } else if (fsize == 0) {
        *digest = 0xffffffff;
        SetErrorCode(error_code, ERR_FILE_OK);
        return true;
    }

    File* file_impl = GetFileImplByPath(path_name);

    uint32_t flag = File::ENUM_FILE_OPEN_MODE_R;
    File* fp = Open(path_name, flag, OpenFileOptions(), error_code);
    if (NULL == fp) {
        return false;
    }
    scoped_ptr<File> scoped_fp(fp);
    bool ret = file_impl->GetDigestImpl(fp, fsize, digest, error_code);
    fp->Close();
    return ret;
}

int32_t File::Snapshot(const std::string& source_path,
                       const std::string& target_path,
                       uint32_t* error_code) {
    if (source_path.empty() || target_path.empty()) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return -1;
    }

    File* file_impl = GetFileImplByPath(source_path);
    return file_impl->SnapshotImpl(source_path.c_str(), target_path.c_str(), error_code);
}

int32_t File::SnapshotImpl(const char* source_path,
                           const char* target_path,
                           uint32_t* error_code) {
    SetErrorCode(error_code, ERR_FILE_FUNCTION_NO_IMPLEMENTATION);
    return -1;
}

bool File::Freeze(const std::string& file_name, uint32_t* error_code) {
    if (file_name.empty()) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return false;
    }
    File* file_impl = GetFileImplByPath(file_name);
    return file_impl->FreezeImpl(file_name, error_code);
}

bool File::FreezeImpl(const std::string& file_name, uint32_t* error_code) {
    SetErrorCode(error_code, ERR_FILE_FUNCTION_NO_IMPLEMENTATION);
    return false;
}

bool File::SetReplication(const std::string& file_name,
                          int32_t new_backup_factor,
                          uint32_t* error_code) {
    if (file_name.empty()) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return false;
    }

    File* file_impl = GetFileImplByPath(file_name);
    return file_impl->SetReplicationImpl(file_name, new_backup_factor, error_code);
}

bool File::SetReplicationImpl(const std::string& file_name,
                              int32_t new_backup_factor,
                              uint32_t* error_code) {
    SetErrorCode(error_code, ERR_FILE_FUNCTION_NO_IMPLEMENTATION);
    return false;
}

int32_t File::RenameImplWithOptions(const std::string& old_path_name,
                                    const std::string& new_file_name,
                                    const RenameOptions& rename_option,
                                    uint32_t* error_code) {
    SetErrorCode(error_code, ERR_FILE_FUNCTION_NO_IMPLEMENTATION);
    return -1;
}

bool File::IsSameFilePath(const std::string& path_name, const std::string& ref_path_name) {
    // TODO(aaronzou): Normalize file path, parse file components and then check.
    //     However, it's hard to check two local files are the same considering links and etc.
    //     Maybe some trick way is use stat and check the inode number.
    //     For XFS file, the implementation should check it.
    //     So, only do simple text check here.

    return path_name == ref_path_name;
}

int32_t File::ReadLine(std::string* result) {
    result->resize(0);

    while (true) {
        const int32_t kBufferSize = 4 * 1024;
        std::string buffer(kBufferSize, 0);
        int32_t bytes = ReadLine(string_as_array(&buffer), kBufferSize);

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

// This is shameless copy of //common/system/io/textfile.cpp
bool File::LoadToString(const std::string& file_path, std::string* result) {
    uint32_t error_code;
    int64_t size;
    size = GetSize(file_path.c_str(), &error_code);
    if (size < 0) {
        return false;
    }

    if (size == 0) {
        result->clear();
        return true;
    }
    // file is larger than memory
    if (static_cast<uint64_t>(size) > SIZE_MAX) {
        return false;
    }
    scoped_ptr<File> fp(File::Open(file_path, "r"));
    if (fp.get() == NULL) {
        return false;
    }

    result->resize(size);
    char* buffer = string_as_array(result);

    const int64_t kMaxReadSize = 64 * 1024 * 1024LL;
    int read_size;
    while ((read_size = fp->Read(buffer, std::min(size, kMaxReadSize))) > 0) {
        buffer += read_size;
        size -= read_size;

        // No more content to read, Read(buffer, 0) will cause verbose logging.
        if (size == 0) {
            break;
        }
    }

    if (size != 0) {
        result->clear();
        fp->Close();
        return false;
    }

    fp->Close();
    return true;
}

// This is a low performance default implementation based on previouis existed
// List interface, you can override to provide a higher performance
// implementation.
bool File::GetModeImpl(const std::string& path_name,
                       uint32_t* permission,
                       uint32_t* error_code) {
    *permission = 0;
    FileListOptions options;
    options.mask.file_permission = true;
    // For xfs, set this to obtain dir info instead sub entries
    options.mask.stat_info = true;
    std::vector<common::AttrsInfo> attrs;
    if (!ListImpl(path_name.c_str(), options, &attrs, error_code) == 0) {
        return false;
    }
    if (attrs.size() != 1) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return false;
    }

    *permission = attrs[0].file_permission;
    return true;
}

// This is a low performance default implementation based on previouis existed
// List interface, you can override to provide a higher performance implementation.
bool File::GetAttrsImpl(const std::string& path_name,
                        const AttrsMask& mask,
                        AttrsInfo* attrs_info,
                        uint32_t* error_code) {
    // For xfs, set this to obtain dir info instead sub entries
    AttrsMask mask_with_stat_info = mask;
    mask_with_stat_info.stat_info = true;

    ListOptions listOptions(&mask_with_stat_info);
    std::vector<common::AttrsInfo> attrs;
    if (File::List(path_name, listOptions, &attrs, error_code) != 0) {
        return false;
    }
    if (attrs.size() != 1) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return false;
    }

    *attrs_info = attrs[0];
    return true;
}

// use this default implementation to avoid build break
bool File::ChangeSecondRoleImpl(const char* path_name,
                                const char* second_role_name,
                                bool is_recursive,
                                uint32_t* error_code) {
    LOG(FATAL) << "Not implement File::ChangeSecondRoleImpl yet.";
    return false;
}

const char* File::GetErrorCodeString(uint32_t error_code) {
    return GetFileErrorCodeStr(error_code);
}

bool File::IsRenameParamValid(const std::string& old_file_name,
                              const std::string& new_file_name,
                              uint32_t*   error_code) {
    if (old_file_name.empty() || new_file_name.empty()) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return false;
    }
    // Don't do other checks here. Leave the flexibility to file implementations.

    // TODO(aaronzou): How to handle the different src and dest file type?
    std::string prefix = GetFilePrefix(old_file_name);
    std::string dest_prefix = GetFilePrefix(new_file_name);
    if (prefix != dest_prefix) {
        SetErrorCode(error_code, ERR_FILE_PARAMETER);
        return false;
    }
    return true;
}

} // namespace common
