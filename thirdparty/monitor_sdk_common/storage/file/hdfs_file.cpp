// Copyright (c) 2015, Vivo Inc. All rights reserved.
/**
 * @file hdfs_file.cpp
 * @brief
 * @author chenzheng05@baidu.com
 * @date 2015-01-21
 */

#include <errno.h>

#include "thirdparty/monitor_sdk_common/base/closure2.h"
#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/base/string/string_piece.h"
#include "thirdparty/monitor_sdk_common/crypto/lite_crc/fast_crc.h"
#include "thirdparty/monitor_sdk_common/storage/file/aioframe/aioframe.h"
#include "thirdparty/monitor_sdk_common/storage/file/hdfs_file.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/thread_pool.h"
#include "thirdparty/monitor_sdk_common/text/wildcard.h"

#include "thirdparty/glog/logging.h"

DEFINE_bool(hdfs_enable_unittest_mode, false,
            "Application should NOT change this flag. "
            "This flag is only used in ut to create a mock object");

#define CHECK_HANDLE(h) \
    if (h == NULL) { \
        SetErrorCode(error_code, ERR_FILE_FAIL); \
        return -1; \
    }

namespace {
// Set cur_attrs according to given hdfsFileInfo and mask.
void SetEntryAttributes(
    const hdfsFileInfo& info,
    const ListOptions& list_option,
    AttrsInfo* cur_attrs) {

    std::string file_name =
        common::StringRemoveSuffix(info.mName, common::StringPiece("/"));
    cur_attrs->file_name = file_name.assign(strrchr(file_name.c_str(), '/') + 1);
    if (list_option.mask.file_type) {
        cur_attrs->file_type = (info.mKind == 'D' ? FILE_TYPE_DIR : FILE_TYPE_NORMAL);
    }
    if (list_option.mask.file_size) {
        cur_attrs->file_size = info.mSize;
    }
    if (list_option.mask.file_identity) {
        cur_attrs->file_identity = info.mOwner;
    }
    if (list_option.mask.file_role) {
        cur_attrs->file_role = info.mGroup;
    }
    if (list_option.mask.modify_time) {
        cur_attrs->modify_time = info.mLastMod;
    }
    if (list_option.mask.access_time) {
        cur_attrs->access_time = info.mLastAccess;
    }
    if (list_option.mask.backup_factor) {
        cur_attrs->backup_factor = info.mReplication;
    }
    if (list_option.mask.file_permission) {
        cur_attrs->file_permission = info.mPermissions;
    }
}
} // namespace

namespace common {

REGISTER_FILE_IMPL(HDFS_FILE_PREFIX, HdfsFile);
hdfsFS HdfsFile::s_fs = NULL;

bool HdfsFile::InitImpl()
{
    if (!FLAGS_hdfs_enable_unittest_mode) {
        // init s_fs, we use default configuration ,
        // users can set parameters in hdfs configuration files.
        s_fs = hdfsConnect("default", 0);
        if (s_fs == NULL) {
            LOG(ERROR) << "hdfs init failed!";
            return false;
        }
    }
    return true;
}

bool HdfsFile::CleanupImpl()
{
    if (!FLAGS_hdfs_enable_unittest_mode) {
        int ret = hdfsDisconnect(s_fs);
        if (ret == -1) {
            LOG(ERROR) << "hdfs cleanup failed!";
            return false;
        }
    }
    return true;
}

bool HdfsFile::OpenImpl(const char* file_path, uint32_t flags,
                        const OpenFileOptions& options,
                        uint32_t *error_code)
{
    const char* path = file_path + strlen(HDFS_FILE_PREFIX) - 1;
    m_path_name = path;

    int32_t open_flag = 0;
    if (flags == ENUM_FILE_OPEN_MODE_R) {
        if (!IsExist(file_path))
            return false;
        open_flag = O_RDONLY;
    }
    else if (flags == ENUM_FILE_OPEN_MODE_W)
        open_flag = O_WRONLY;
    else if (flags == (ENUM_FILE_OPEN_MODE_W|ENUM_FILE_OPEN_MODE_A)) {
        open_flag = O_WRONLY | O_APPEND;
    } else {
        SetErrorCode(error_code, ERR_FILE_FAIL);
        return false;
    }

    // We do not support hdfs block_size option now
    m_fd = hdfsOpenFile(s_fs, path, open_flag, options.cache_buf_len,
                        options.backup_factor, 0);
    if (m_fd == NULL) {
        LOG(ERROR) << "failed to open hdfs file";
        SetErrorCode(error_code, ERR_FILE_FAIL);
        return false;
    } else {
        SetErrorCode(error_code, ERR_FILE_OK);
        if (m_buffer_cache == NULL) {
            m_buffer_cache = new char[kBufferCacheSize];
        }
        m_cache_front_idx = 0;
        m_cache_back_idx = 0;
        return true;
    }
}

int64_t HdfsFile::Read(void* buf, int64_t buf_size, uint32_t* error_code)
{
    CHECK_HANDLE(s_fs);
    CHECK_HANDLE(m_fd);
    int ret = hdfsRead(s_fs, m_fd, buf, buf_size);
    SetErrorCode(error_code, (ret >= 0 ? ERR_FILE_OK : ERR_FILE_FAIL));
    return ret;
}

int32_t HdfsFile::ReadLine(void* buffer, int32_t max_size)
{
    char* string_buffer = static_cast<char*>(buffer);
    uint32_t error_code = ERR_FILE_OK;
    int32_t has_read_len = 0;
    int32_t read_len = 0;

    int32_t cache_size = kBufferCacheSize;
    while (has_read_len < max_size - 1) {
        bool do_remote_read = IsCacheEmpty();
        if (do_remote_read) {
            // do remote read: read from HDFS file to buffer cache
            read_len = Read(m_buffer_cache,
                            std::min(max_size - 1, cache_size),
                            &error_code);
            if (error_code != ERR_FILE_OK) {
                return -1;
            }
            m_cache_front_idx = 0;
            m_cache_back_idx = (read_len > 0) ? read_len : 0;
        }

        // do local read: read from buffer cache
        read_len = ReadFromCache(string_buffer + has_read_len,
                                 max_size - 1 - has_read_len);
        if (read_len > 0) {
            has_read_len += read_len;
            if (string_buffer[has_read_len - 1] == '\n') {
                break;
            }
        } else {
            break;
        }
    }
    string_buffer[has_read_len] = '\0';
    return has_read_len;
}

int64_t HdfsFile::Write(const void* buf, int64_t buf_size, uint32_t* error_code)
{
    CHECK_HANDLE(s_fs);
    CHECK_HANDLE(m_fd);
    int ret = hdfsWrite(s_fs, m_fd, buf, buf_size);
    SetErrorCode(error_code, (ret >= 0 ? ERR_FILE_OK : ERR_FILE_FAIL));
    return ret;
}

int32_t HdfsFile::Flush(uint32_t* error_code)
{
    CHECK_HANDLE(s_fs);
    CHECK_HANDLE(m_fd);
    int ret = hdfsFlush(s_fs, m_fd);
    SetErrorCode(error_code, (ret >= 0 ? ERR_FILE_OK : ERR_FILE_FAIL));
    return ret;
}


int64_t HdfsFile::Seek(int64_t offset, int32_t origin, uint32_t* error_code)
{
    CHECK_HANDLE(s_fs);
    CHECK_HANDLE(m_fd);

    int64_t hdfs_offset = -1;
    int64_t end_pos = 0;
    int64_t ret = -1;

    // to support SEEK_CUR, SEEK_END and SEEK_SET
    hdfsFileInfo* info = hdfsGetPathInfo(s_fs, m_path_name.c_str());
    if (!info) {
        LOG(ERROR) << "GetPathInfo failed!";
        SetErrorCode(error_code, ERR_FILE_FAIL);
        return -1;
    }
    end_pos = info->mSize + 1;
    hdfsFreeFileInfo(info, 1);

    if (origin == SEEK_SET)
        hdfs_offset = offset;
    else if (origin == SEEK_CUR)
        hdfs_offset = offset + hdfsTell(s_fs, m_fd);
    else if (origin == SEEK_END)
        hdfs_offset = end_pos + offset;

    if (hdfs_offset < 0 || hdfs_offset > end_pos) {
        SetErrorCode(error_code, ERR_FILE_FAIL);
        return ret;
    }
    ret = hdfsSeek(s_fs, m_fd, hdfs_offset);
    SetErrorCode(error_code, (ret == 0 ? ERR_FILE_OK : ERR_FILE_FAIL));
    return (ret == 0 ? hdfs_offset : -1);
}

int64_t HdfsFile::Tell(uint32_t* error_code)
{
    CHECK_HANDLE(s_fs);
    CHECK_HANDLE(m_fd);
    int64_t ret = hdfsTell(s_fs, m_fd);
    SetErrorCode(error_code, (ret >= 0 ? ERR_FILE_OK : ERR_FILE_FAIL));
    return ret;
}

bool HdfsFile::CheckExistImpl(const char* file_name, uint32_t* error_code)
{
    if (s_fs == NULL) {
        SetErrorCode(error_code, ERR_FILE_FAIL);
        return false;
    }
    const char* path = file_name + strlen(HDFS_FILE_PREFIX) - 1;
    int ret = hdfsExists(s_fs, path);
    SetErrorCode(error_code, (ret == 0 ? ERR_FILE_OK : ERR_FILE_FAIL));
    return !ret;
}

int32_t HdfsFile::ListImpl(const char* pattern,
                           const ListOptions& list_option, std::vector<AttrsInfo>* attrs_info,
                           uint32_t* error_code)
{
    CHECK_HANDLE(s_fs);
    if (!IsExist(pattern)) {
        SetErrorCode(error_code, ERR_FILE_FAIL);
        LOG(INFO) << "file does not exist!";
        return -1;
    }
    // we do not support wildcard now.
    const char* path = pattern + strlen(HDFS_FILE_PREFIX) - 1;

    if (list_option.mask.stat_info) {
        hdfsFileInfo* my_info = hdfsGetPathInfo(s_fs, path);
        if (!my_info) {
            if (errno) {
                SetErrorCode(error_code, ERR_FILE_FAIL);
                return -1;
            }
            // empty directory
            return 0;
        } else {
            AttrsInfo attr;
            SetEntryAttributes(my_info[0], list_option, &attr);
            attr.error_code = ERR_FILE_OK;
            attrs_info->push_back(attr);

            hdfsFreeFileInfo(my_info, 1);
            SetErrorCode(error_code, ERR_FILE_OK);
            return 0;
        }
    }

    int32_t num_entries = 0;
    hdfsFileInfo* info = hdfsListDirectory(s_fs, path, &num_entries);
    attrs_info->clear();
    if (!info) {
        if (errno) {
            SetErrorCode(error_code, ERR_FILE_FAIL);
            return -1;
        }
        // empty directory
        return 0;
    } else {
        for (int i = 0; i < num_entries; i++) {
            AttrsInfo attr;

            SetEntryAttributes(info[i], list_option, &attr);
            attr.error_code = ERR_FILE_OK;
            attrs_info->push_back(attr);
        }
        hdfsFreeFileInfo(info, num_entries);
        SetErrorCode(error_code, ERR_FILE_OK);
        return 0;
    }
}

bool HdfsFile::GetContentSummaryImpl(
    const char* path_name,
    FileContentSummaryMask* mask,
    FileContentSummary* summary,
    uint32_t* error_code) {
    LOG(FATAL) << "Not implement HdfsFile::GetContentSummaryImpl yet.";
    return false;
}

bool HdfsFile::GetDigestImpl(File* file_obj,
                             int64_t file_size,
                             uint32_t* digest,
                             uint32_t* error_code) {
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

int32_t HdfsFile:: MoveImpl(const char* src_name, const char* dst_name, uint32_t* error_code)
{
    CHECK_HANDLE(s_fs);
    if (!IsExist(src_name) || !dst_name) {
        SetErrorCode(error_code, ERR_FILE_FAIL);
        LOG(INFO) << "error parameters in Move!";
        return -1;
    }

    const char* src = src_name + strlen(HDFS_FILE_PREFIX) - 1;
    const char* dst = dst_name + strlen(HDFS_FILE_PREFIX) - 1;
    int ret = hdfsMove(s_fs, src, s_fs, dst);
    SetErrorCode(error_code, (ret == 0 ? ERR_FILE_OK : ERR_FILE_FAIL));
    return ret;
}

int32_t HdfsFile::RenameImpl(const char* old_path_name, const char* new_path_name,
                             uint32_t* error_code)
{
    CHECK_HANDLE(s_fs);
    if (!IsExist(old_path_name) || !new_path_name) {
        SetErrorCode(error_code, ERR_FILE_FAIL);
        LOG(INFO) << "error parameters in Rename!";
        return -1;
    }
    const char* src = old_path_name + strlen(HDFS_FILE_PREFIX) - 1;
    const char* dst = new_path_name + strlen(HDFS_FILE_PREFIX) - 1;
    int ret = hdfsRename(s_fs, src, dst);
    SetErrorCode(error_code, (ret == 0 ? ERR_FILE_OK : ERR_FILE_FAIL));
    return ret;
}

int32_t HdfsFile::RemoveImpl(const char* file_name, bool is_recursive,
                             uint32_t* error_code)
{
    CHECK_HANDLE(s_fs);
    if (!IsExist(file_name)) {
        SetErrorCode(error_code, ERR_FILE_FAIL);
        LOG(INFO) << "file does not exist!";
        return -1;
    }
    // is_recursive is always true.
    if (!is_recursive) {
        LOG(ERROR) << "only support recursive remove!";
        return -1;
    }
    const char* path = file_name + strlen(HDFS_FILE_PREFIX) - 1;
    int ret = hdfsDelete(s_fs, path);
    SetErrorCode(error_code, (ret == 0 ? ERR_FILE_OK : ERR_FILE_FAIL));
    return ret;
}

int32_t HdfsFile::AddDirImpl(const char* file_name, uint32_t* error_code)
{
    CHECK_HANDLE(s_fs);
    if (!file_name) {
        SetErrorCode(error_code, ERR_FILE_FAIL);
        return -1;
    }
    const char* path = file_name + strlen(HDFS_FILE_PREFIX) - 1;
    int ret = hdfsCreateDirectory(s_fs, path);
    SetErrorCode(error_code, (ret == 0 ? ERR_FILE_OK : ERR_FILE_FAIL));
    return ret;
}

int64_t HdfsFile::GetSizeImpl(const char* file_name, uint32_t* error_code)
{
    CHECK_HANDLE(s_fs);
    if (!IsExist(file_name)) {
        SetErrorCode(error_code, ERR_FILE_FAIL);
        LOG(INFO) << "file does not exist!";
        return -1;
    }
    const char* path = file_name + strlen(HDFS_FILE_PREFIX) - 1;
    int ret = -1;
    hdfsFileInfo* info = hdfsGetPathInfo(s_fs, path);
    if (info) {
        // check  if directory
        int is_directory = info->mKind;
        if (is_directory == 'D') {
            LOG(ERROR) << "inputfile: " << path << " is directory";
            SetErrorCode(error_code, ERR_FILE_FAIL);
            hdfsFreeFileInfo(info, 1);
            return ret;
        }
        ret = info->mSize;
        hdfsFreeFileInfo(info, 1);
        SetErrorCode(error_code, ERR_FILE_OK);
        return ret;
    }
    SetErrorCode(error_code, ERR_FILE_FAIL);
    return ret;
}

bool HdfsFile::ChmodImpl(const char* path_name,
                         const uint32_t permission,
                         uint32_t* error_code)
{
    if (s_fs == NULL) {
        SetErrorCode(error_code, ERR_FILE_FAIL);
        return false;
    }
    if (!IsExist(path_name)) {
        SetErrorCode(error_code, ERR_FILE_FAIL);
        LOG(INFO) << "file does not exist!";
        return -1;
    }
    const char* path = path_name + strlen(HDFS_FILE_PREFIX) - 1;
    int ret = hdfsChmod(s_fs, path, permission);
    SetErrorCode(error_code, (ret == 0 ? ERR_FILE_OK : ERR_FILE_FAIL));
    return !ret;
}

bool HdfsFile::ChangeRoleImpl(const char* path_name,
                                const char* role_name,
                                uint32_t* error_code)
{
    return false;
}

int32_t HdfsFile::Close(uint32_t* error_code)
{
    CHECK_HANDLE(s_fs);
    int ret = 0;
    if (m_fd != NULL) {
        ret = hdfsCloseFile(s_fs, m_fd);
        if (ret == -1)
            SetErrorCode(error_code, ERR_FILE_FAIL);
    }
    m_fd = NULL;

    if (m_buffer_cache != NULL) {
        delete [] m_buffer_cache;
        m_buffer_cache = NULL;
    }
    m_cache_front_idx = 0;
    m_cache_back_idx = 0;
    return ret;
}

inline int64_t HdfsFile::GetValidCacheSize() const {
    return (m_cache_back_idx - m_cache_front_idx);
}

inline bool HdfsFile::IsCacheEmpty() const {
    return m_cache_back_idx == m_cache_front_idx;
}

inline bool HdfsFile::IsCacheFull() const {
    return m_cache_back_idx - m_cache_front_idx == kBufferCacheSize;
}

int64_t HdfsFile::ReadFromCache(void* buffer, int64_t size) {
    size = std::min(size, GetValidCacheSize());
    if (size <= 0) {
        return -1;
    }
    // copy line from cache
    std::string::size_type end_pos = StringPiece(m_buffer_cache + m_cache_front_idx,
                                                 size).find_first_of('\n');
    if (end_pos != std::string::npos) {
        size = end_pos + 1;
    }
    memcpy(buffer,
           m_buffer_cache + m_cache_front_idx,
           sizeof(m_buffer_cache[0]) * size);
    m_cache_front_idx += size;
    return size;
}

int64_t HdfsFile::WriteToCache(const void* buffer, int64_t size) {
    if (size + GetValidCacheSize() > kBufferCacheSize) {
        LOG(FATAL) << "reserved buffer cache size (" << kBufferCacheSize << " bytes)"
            << " is too small to caching data, which has used " << GetValidCacheSize()
            << " and needs " << size << " bytes more";
    }
    // will overwrite all unread data in cache
    memcpy(m_buffer_cache,
           buffer,
           sizeof(m_buffer_cache[0]) * size);
    m_cache_front_idx = 0;
    m_cache_back_idx = size;
    return size;
}

} // namespace common
