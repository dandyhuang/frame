// Copyright (c) 2014, Vivo Inc.
// All rights reserved.
//
// Author: rabbitliu <liubin18@baidu.com>
// Created: 01/11/14
// Description:

#ifndef COMMON_FILE_HDFS_HDFS_CLIENT_H
#define COMMON_FILE_HDFS_HDFS_CLIENT_H
#pragma once

#include <map>
#include <sstream>
#include <string>
#include <utility>

#include "thirdparty/monitor_sdk_common/base/smart_ptr/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/storage/file/hdfs/hdfs_def.h"
#include "thirdparty/monitor_sdk_common/storage/file/hdfs/hdfs_file.h"
#include "thirdparty/monitor_sdk_common/storage/file/hdfs/hdfs_impl.h"
#include "thirdparty/monitor_sdk_common/storage/file/hdfs/mock_hdfs.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/atomic.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"

namespace common {

class HDFSFile;

class HDFSClient {
    friend class HDFSFile;
public:
    struct Options {
    public:
        /// default使用hadoop-0.20.2/conf/core-site.xml的配置，默认为本地文件系统
        /// <property>
        ///    <name>fs.default.name</name>
        ///    <value>file:///</value>
        ///    <description>Deprecated. Use (fs.defaultFS) property
        ///    instead</description>
        /// </property>
        Options() : host("default"), port(0) {}
        ~Options() {}
        std::string ToString() const
        {
            std::ostringstream oss;
            oss << "host: " << host << ", port: " << port;
            return oss.str();
        }

        std::string host;
        int32_t port;
    };

    /// @brief 静态方法，初始化HDFSClient类,设置连接的集群和端口等
    /// @param options hdfs集群名,  "default"或者ip域名.default 指local file system
    /// @retval HDFSClient 如果成功得到一个初始化好的HDFSClient，否则返回NULL
    ///                    得到的client指针用户不能delete，
    ///                    当不需要的时候，需要调用ReleaseHDFSClient
    static HDFSClient* GetHDFSClient(const Options& options = Options());
    static void ReleaseHDFSClient(HDFSClient* hdfs_client);

    static void SetMockModel();
    static MockHDFS* GetMockImpl();

    ~HDFSClient();

    /// @brief 打开一个HDFS文件
    /// @param file_path hdfs的一个路径
    /// @param open_mode 只读，只写，读写，追加等。如kRead | kWrite
    /// @param opt 打开文件的选项，包括cache size，block size等，一般用默认即可
    /// @param error_code 如果错误返回错误码
    /// @retval HDFSFile* 如果成功返回一个HDFSFile指针，失败则返回NULL，
    ///                   (注意！！！)HDFSFile*指针由用户析构，HDFSClient析构之前要保证所有的
    ///                   HDFSFile都已经析构了。
    HDFSFile* Open(const std::string& file_path,
                   int32_t open_model,
                   const file::OpenFileOptions& opt = file::OpenFileOptions(),
                   int* error_code = NULL);

    /// @brief 判断一个文件是否存在
    /// @param file_path 要判断的文件路径
    /// @param error_code 错误码
    /// @retval bool true表示文件存在
    bool IsExist(const std::string& file_path, int* error_code = NULL);

    /// @brief 判断一个文件路径是否是目录
    /// @param file_path 要判断的文件路径
    /// @param error_code 错误码
    /// @retval bool true表示文件是目录
    bool IsDir(const std::string& file_path, int* error_code = NULL);

    /// @brief List一个路径
    /// @param file_path 要List的文件路径
    /// @param attrs_info 返回的属性信息
    /// @param error_code 错误码
    /// @retval bool true表示List成功，false表示失败
    bool List(const std::string& file_path,
              std::vector<file::AttrsInfo>* attrs_info,
              int* error_code = NULL);

    /// @brief 获取一个文件的大小
    /// @param file_path 要判断的文件路径
    /// @param error_code 错误码
    ///  @retval int64_t >=表示文件大小，-1表示错误
    int64_t GetFileSize(const std::string& file_path, int* error_code = NULL);

    /// @brief 创建目录
    /// @param file_path 要创建的目录路径
    /// @param error_code 错误码
    /// @retval 0:成功 -1:失败
    int32_t CreateDir(const std::string& dir_path, int* error_code = NULL);

    /// @brief 删除文件
    /// @param file_path 要删除的文件路径
    /// @param error_code 错误码
    /// @retval 0:成功 -1:失败
    int32_t Remove(const std::string& file_path, int* error_code = NULL);

    /// @brief 对文件改名
    /// @retval 0:成功 -1:失败
    int32_t Rename(const std::string& src_file,
                   const std::string& dest_file,
                   int* error_code = NULL);

private:
    HDFSClient(::hdfsFS fs, const Options& options);

private:
    ::hdfsFS m_fs;  // HDFS的文件系统句柄
    Options m_options;

    // 打开文件的引用计数，只有所有HDFSFile都析构了，HDFSClient才能析构
    common::Atomic<uint64_t> m_Opened_file_count;

    static scoped_ptr<HdfsFileInterface> s_file_impl;
    static bool s_is_mock;

    // 同一类型的文件系统对应同一个HDFSClient
    // key:options
    // value.first hdfs_client
    // value.second reference counter
    static std::map<std::string, std::pair<HDFSClient*, int32_t> > s_fs_options_to_client;
    static Mutex s_mutex;
};

} // namespace common

#endif // COMMON_FILE_HDFS_HDFS_CLIENT_H
