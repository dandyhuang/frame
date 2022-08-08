// Copyright (c) 2014, Vivo Inc.
// All rights reserved.
//
// Author: rabbitliu <liubin18@baidu.com>
// Created: 01/11/14
// Description:

#include "thirdparty/monitor_sdk_common/storage/file/hdfs/hdfs_client.h"

#include <stdio.h>
#include "thirdparty/monitor_sdk_common/system/concurrency/this_thread.h"
#include "thirdparty/glog/logging.h"

namespace common {

#define ERROR_REPLACE(error_code) \
    int error_placeholder = 0; \
    if (error_code == NULL) { \
        error_code = &error_placeholder; \
    } \
    *error_code = file::HDFS_OK;

scoped_ptr<HdfsFileInterface> HDFSClient::s_file_impl(new HDFSImpl());
bool HDFSClient::s_is_mock = false;
std::map<std::string, std::pair<HDFSClient*, int32_t> > HDFSClient::s_fs_options_to_client;
Mutex HDFSClient::s_mutex;

void HDFSClient::SetMockModel()
{
    LOG(WARNING) << "User set mock model";
    s_file_impl.reset(new MockHDFS());
    s_is_mock = true;
}

MockHDFS* HDFSClient::GetMockImpl()
{
    if (!s_is_mock) {
        return NULL;
    }
    return reinterpret_cast<MockHDFS*>(s_file_impl.get());
}

HDFSClient* HDFSClient::GetHDFSClient(const Options& options)
{
    MutexLocker locker(s_mutex);
    std::string fs_options = options.ToString();
    std::map<std::string, std::pair<HDFSClient*, int32_t> >::iterator iter =
        s_fs_options_to_client.find(fs_options);
    if (iter != s_fs_options_to_client.end()) {
        // 增加引用计数
        ++iter->second.second;
        return iter->second.first;
    }

    ::hdfsFS fs = s_file_impl->hdfsConnect(options.host.c_str(), static_cast<tPort>(options.port));
    if (fs == NULL) {
        LOG(ERROR) << "Init HDFS FileSystem error, options: " << options.ToString();
        return NULL;
    }
    HDFSClient* hdfs_client = new HDFSClient(fs, options);
    LOG(INFO) << "Init HDFS FileSystem successful, options: " << fs_options;
    std::pair<HDFSClient*, int32_t> value = std::make_pair(hdfs_client, 1);
    s_fs_options_to_client[fs_options] = value;

    return hdfs_client;
}

void HDFSClient::ReleaseHDFSClient(HDFSClient* hdfs_client) {
    MutexLocker locker(s_mutex);
    std::string fs_options = hdfs_client->m_options.ToString();
    std::map<std::string, std::pair<HDFSClient*, int32_t> >::iterator iter =
        s_fs_options_to_client.find(fs_options);
    if (iter != s_fs_options_to_client.end()) {
        // 减少引用计数
        --iter->second.second;
        if (iter->second.second <= 0) {
            // 该fs options对应的hdfs client已经不再需要，可以释放资源
            delete iter->second.first;
            s_fs_options_to_client.erase(iter);
        }
    }
}

HDFSClient::HDFSClient(::hdfsFS fs, const Options& options) : m_fs(fs), m_options(options)
{
    m_Opened_file_count = 0;
}

HDFSClient::~HDFSClient()
{
    // 存在打开状态的HDFSFile，则HDFSClient不应该被析构
    // ???(rabbitliu) 客户端程序，是否可以CHECK
    while (m_Opened_file_count != 0) {
        LOG(ERROR) << "Not all HDFSFile closed, m_Opened_file_count: " << m_Opened_file_count;
        ThisThread::Sleep(1);
    }
    LOG(INFO) << "Close HDFS FileSystem." << m_options.ToString();
    s_file_impl->hdfsDisconnect(m_fs);
}

HDFSFile* HDFSClient::Open(const std::string& file_path,
                           int32_t open_model,
                           const file::OpenFileOptions& opt,
                           int* error_code)
{
    VLOG(1) << "Try to Open with path: " << file_path
        << ", open_model: " << file::OpenModelToString(open_model);
    ERROR_REPLACE(error_code);

    int flags = 0;
    if (open_model == file::kRead) {
        if (!IsExist(file_path)) {
            *error_code = file::HDFS_FILE_NOT_EXIST;
            return NULL;
        }
        flags = O_RDONLY;
    } else if (open_model == file::kWrite) {
        flags = O_WRONLY;
    } else if (open_model == file::kAppend) {
        flags = O_WRONLY | O_APPEND;
    /*  测试不支持以读写方式打开，目前未知原因
    } else if (open_model == file::kRead | file::kWrite) {
        flags = O_RDWR;
    */
    } else {
        LOG(ERROR) << "Open model not supported, open_model: " << open_model;
        *error_code = file::HDFS_PARM_ERR;
        return NULL;
    }

    ::hdfsFile fd = s_file_impl->hdfsOpenFile(m_fs, file_path.c_str(), flags,
                                              opt.buffer_size, opt.replication, opt.block_size);
    if (fd == NULL) {
        LOG(ERROR) << "hdfsOpenFile error!";
        *error_code = file::HDFS_FAIL;
        return NULL;
    }
    ++m_Opened_file_count;
    HDFSFile* hdfs_file = new HDFSFile(this, fd, file_path);
    return hdfs_file;
}

bool HDFSClient::IsExist(const std::string& file_path, int* error_code)
{
    VLOG(1) << "Check file is exist, file_path: " << file_path;
    CHECK_NOTNULL(m_fs);
    ERROR_REPLACE(error_code);

    int ret = s_file_impl->hdfsExists(m_fs, file_path.c_str());
    *error_code = ret == 0 ? file::HDFS_OK : file::HDFS_FAIL;
    return ret == 0;
}

bool HDFSClient::IsDir(const std::string& file_path, int* error_code) {
    VLOG(1) << "Isdir, file_path: " << file_path;
    CHECK_NOTNULL(m_fs);
    ERROR_REPLACE(error_code);

    hdfsFileInfo* file_info = s_file_impl->hdfsGetPathInfo(m_fs, file_path.c_str());
    if (!file_info) {
        *error_code = file::HDFS_FAIL;
        return false;
    }

    *error_code = file::HDFS_OK;
    bool is_dir = file_info->mKind == kObjectKindDirectory;
    s_file_impl->hdfsFreeFileInfo(file_info, 1);

    return is_dir;
}

bool HDFSClient::List(const std::string& file_path,
                      std::vector<file::AttrsInfo>* attrs_info,
                      int* error_code) {
    VLOG(1) << "List dir, file_path: " << file_path;
    CHECK_NOTNULL(m_fs);
    ERROR_REPLACE(error_code);

    if (!IsDir(file_path)) {
        *error_code = file::HDFS_FAIL;
        return false;
    }

    int num_entries = 0;
    hdfsFileInfo* info = hdfsListDirectory(m_fs, file_path.c_str(), &num_entries);
    if (!info) {
        *error_code = file::HDFS_FAIL;
        return false;
    }
    attrs_info->clear();

    for (int i = 0; i < num_entries; ++i) {
        file::AttrsInfo attr;
        attr.file_name = info[i].mName;
        attr.file_type = (info[i].mKind == 'D' ? file::FILE_TYPE_DIR : file::FILE_TYPE_FILE);
        attr.file_size = info[i].mSize;
        attrs_info->push_back(attr);
    }
    hdfsFreeFileInfo(info, num_entries);
    *error_code = file::HDFS_OK;

    return true;
}

int64_t HDFSClient::GetFileSize(const std::string& file_path, int* error_code) {
    VLOG(1) << "GetFileSize, file_path: " << file_path;
    CHECK_NOTNULL(m_fs);
    ERROR_REPLACE(error_code);

    hdfsFileInfo* file_info = s_file_impl->hdfsGetPathInfo(m_fs, file_path.c_str());
    if (!file_info) {
        *error_code = file::HDFS_FAIL;
        return -1;
    }

    int64_t file_size = file_info->mSize;
    // path不是文件，是目录，需要返回错误
    if (file_info->mKind != kObjectKindFile) {
        *error_code = file::HDFS_PARM_ERR;
        file_size = -1;
    }
    s_file_impl->hdfsFreeFileInfo(file_info, 1);

    return file_size;
}

int32_t HDFSClient::CreateDir(const std::string& dir_path, int* error_code) {
    VLOG(1) << "CreateDir, file_path: " << dir_path;
    CHECK_NOTNULL(m_fs);
    ERROR_REPLACE(error_code);

    int32_t ret = s_file_impl->hdfsCreateDirectory(m_fs, dir_path.c_str());
    *error_code = ret == 0 ? file::HDFS_OK : file::HDFS_FAIL;
    return ret;
}

int32_t HDFSClient::Remove(const std::string& file_path, int* error_code) {
    VLOG(1) << "Remove, file_path: " << file_path;
    CHECK_NOTNULL(m_fs);
    ERROR_REPLACE(error_code);

    int recursive = 1;
    int32_t ret = s_file_impl->hdfsDelete(m_fs, file_path.c_str(), recursive);
    *error_code = ret == 0 ? file::HDFS_OK : file::HDFS_FAIL;
    return ret;
}

int32_t HDFSClient::Rename(const std::string& src_file_path,
                           const std::string& dest_file_path,
                           int* error_code) {
    VLOG(1) << "Rename file_path: " << src_file_path << " to " << dest_file_path;
    CHECK_NOTNULL(m_fs);
    ERROR_REPLACE(error_code);

    int32_t ret = s_file_impl->hdfsRename(m_fs, src_file_path.c_str(), dest_file_path.c_str());
    *error_code = ret == 0 ? file::HDFS_OK : file::HDFS_FAIL;
    return ret;
}

} // namespace common
