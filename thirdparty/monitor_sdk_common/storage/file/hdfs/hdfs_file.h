// Copyright (c) 2014, Vivo Inc.
// All rights reserved.
//
// Author: rabbitliu <liubin18@baidu.com>
// Created: 01/10/14
// Description:

#ifndef COMMON_FILE_HDFS_HDFS_FILE_H
#define COMMON_FILE_HDFS_HDFS_FILE_H
#pragma once

#include <string>
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/atomic.h"
#include "thirdparty/libhdfs-2.5.0/hdfs.h"

namespace common {

class HDFSClient;

class HDFSFile {
public:
    HDFSFile(HDFSClient* client, ::hdfsFile fd, const std::string& path)
        : m_client(client), m_fd(fd), m_path(path), m_is_closed(false) {}
    ~HDFSFile() { Close(); }

    /// @brief 关闭文件句柄
    /// @param error_code 错误码
    /// @retval bool true表示成功，false失败，失败了错误码在error_code中查看
    bool Close(int32_t* error_code = NULL);

    /// @brief 从文件中读取数据
    /// @param buffer 数据存放的buffer
    /// @param size buffer的长度
    /// @param error_code 错误码
    /// @retval int32_t 读取的真正长度，-1表示失败，error_code表示原因
    int32_t Read(void* buffer, int64_t size, int32_t* error_code = NULL);

    /// @brief 像文件中写入数据
    /// @param buffer 数据存放的buffer
    /// @param size buffer的长度
    /// @param error_code 错误码
    /// @retval int32_t 写入的真正长度，-1表示失败，error_code表示原因
    int32_t Write(const void* buffer, int64_t size, int32_t* error_code = NULL);

    /// @brief 得到文件的偏移量
    /// @param error_code 错误码
    /// @retval int32_t 文件的偏移量，-1表示错误
    int64_t Tell(int32_t* error_code = NULL);

    /// @brief 将缓冲数据刷入
    /// @param error_code 错误码
    /// @retval bool true表示成功，false失败，失败了错误码在error_code中查看
    bool Flush(int32_t* error_code = NULL);

    /// @brief 改变文件当前偏移量
    /// @param offset 偏移量的相对位置
    /// @param origin 偏移量的相对位置, SEEK_SET SEEK_END AND SEEK_CUR
    /// @param error_code 错误码
    /// @retval int64_t >=0 成功(返回当前文件的偏移量), -1出错, error_code表示原因
    int64_t Seek(int64_t offset, int32_t origin, int32_t* error_code = NULL);

    /// @brief 得到文件路径
    /// @retval string 文件路径
    const std::string& GetPath() { return m_path; }

private:
    HDFSClient* m_client;
    ::hdfsFile m_fd;    // HDFS文件句柄

    std::string m_path;
    common::Atomic<bool> m_is_closed;
};

} // namespace common

#endif // COMMON_FILE_HDFS_HDFS_FILE_H
