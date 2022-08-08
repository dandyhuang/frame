// Copyright (c) 2014, Vivo Inc.
// All rights reserved.
//
// Author: rabbitliu <liubin18@baidu.com>
// Created: 01/12/14
// Description:

#include "thirdparty/monitor_sdk_common/storage/file/hdfs/hdfs_file.h"

#include "thirdparty/monitor_sdk_common/storage/file/hdfs/hdfs_client.h"
#include "thirdparty/glog/logging.h"

namespace common {

#define ERROR_REPLACE(error_code) \
    int error_placeholder = 0; \
    if (error_code == NULL) { \
        error_code = &error_placeholder; \
    } \
    *error_code = file::HDFS_OK;

bool HDFSFile::Close(int32_t* error_code)
{
    if (m_is_closed) {
        return true;
    }
    VLOG(1) << "close from path: " << m_path;
    ERROR_REPLACE(error_code);
    int ret = m_client->s_file_impl->hdfsCloseFile(m_client->m_fs, m_fd);
    if (ret != 0) {
        *error_code = file::HDFS_FAIL;
        return false;
    }
    --m_client->m_Opened_file_count;
    m_is_closed = true;
    return true;
}

int32_t HDFSFile::Read(void* buffer, int64_t size, int32_t* error_code)
{
    VLOG(1) << "read from path: " << m_path;
    ERROR_REPLACE(error_code);
    int ret = m_client->s_file_impl->hdfsRead(m_client->m_fs, m_fd, buffer, size);
    if (ret < 0) {
        *error_code = file::HDFS_FAIL;
    }
    return ret;
}

int32_t HDFSFile::Write(const void* buffer, int64_t size, int32_t* error_code)
{
    VLOG(1) << "write from path: " << m_path;
    ERROR_REPLACE(error_code);
    int ret = m_client->s_file_impl->hdfsWrite(m_client->m_fs, m_fd, buffer, size);
    if (ret < 0) {
        *error_code = file::HDFS_FAIL;
    }
    return ret;
}

int64_t HDFSFile::Tell(int32_t* error_code)
{
    VLOG(1) << "Tell from path: " << m_path;
    ERROR_REPLACE(error_code);
    int ret = m_client->s_file_impl->hdfsTell(m_client->m_fs, m_fd);
    if (ret < 0) {
        *error_code = file::HDFS_FAIL;
    }
    return ret;
}

int64_t HDFSFile::Seek(int64_t offset, int32_t origin, int32_t* error_code)
{
    VLOG(1) << "Seek from path: " << m_path;
    ERROR_REPLACE(error_code);

    int64_t hdfs_offset = -1;
    int64_t end_pos = 0;
    int64_t ret = -1;

    // to support SEEK_CUR, SEEK_END and SEEK_SET
    hdfsFileInfo* info = m_client->s_file_impl->hdfsGetPathInfo(m_client->m_fs, m_path.c_str());
    if (info == NULL) {
        LOG(ERROR) << "GetPathInfo failed!";
        *error_code = file::HDFS_FAIL;
        return -1;
    }
    VLOG(3) << "mName=" << info->mName << ", mLastMod=" << info->mLastMod
        << ", mSize=" << info->mSize << ", mReplication=" << info->mReplication
        << ", mBlockSize=" << info->mBlockSize << ", mOwner=" << info->mOwner
        << ", mGroup=" << info->mGroup << ", mPermissions=" << info->mPermissions
        << ", mLastAccess=" << info->mLastAccess << ", mKind=" << info->mKind
        << ", kObjectKindFile=" << kObjectKindFile;
    end_pos = info->mSize + 1;
    m_client->s_file_impl->hdfsFreeFileInfo(info, 1);

    if (origin == SEEK_SET)
        hdfs_offset = offset;
    else if (origin == SEEK_CUR)
        hdfs_offset = offset + m_client->s_file_impl->hdfsTell(m_client->m_fs, m_fd);
    else if (origin == SEEK_END)
        hdfs_offset = end_pos + offset - 1;

    if (hdfs_offset < 0 || hdfs_offset > end_pos) {
        *error_code = file::HDFS_FAIL;
        return ret;
    }
    ret = m_client->s_file_impl->hdfsSeek(m_client->m_fs, m_fd, hdfs_offset);
    return (ret == 0 ? hdfs_offset : -1);
}

bool HDFSFile::Flush(int32_t* error_code)
{
    VLOG(1) << "flush from path: " << m_path;
    ERROR_REPLACE(error_code);
    int ret = m_client->s_file_impl->hdfsHSync(m_client->m_fs, m_fd);
    if (ret != 0) {
        *error_code = file::HDFS_FAIL;
        return false;
    }
    return true;
}

} // namespace common
