// Copyright (c) 2014, Vivo Inc.
// All rights reserved.
//
// Author: rabbitliu <liubin18@baidu.com>
// Created: 01/11/14
// Description:

#ifndef COMMON_FILE_HDFS_HDFS_DEF_H
#define COMMON_FILE_HDFS_HDFS_DEF_H
#pragma once

#include <string>
#include "thirdparty/monitor_sdk_common/base/stdint.h"

namespace common {
namespace file {

enum OpenModel {
    kRead = 0x01,
    kWrite = 0x02,
    kAppend = 0x04,
};

std::string OpenModelToString(int32_t open_mode);

struct OpenFileOptions {
public:
    OpenFileOptions() : buffer_size(512 * 1024), replication(3), block_size(0) {}

    int64_t buffer_size;
    int32_t replication;
    int32_t block_size;
};

enum HDFSFileErrorCode {
    HDFS_OK = 0,
    HDFS_FAIL = 1,
    HDFS_FILE_NOT_EXIST = 2,
    HDFS_PARM_ERR = 3,
};

enum FileType {
    FILE_TYPE_UNKNOW = 0,
    FILE_TYPE_FILE = 1,
    FILE_TYPE_DIR = 2,
};

struct AttrsInfo {
    std::string file_name;
    FileType file_type;
    int64_t file_size;
    // TODO(rabbitliu) ownner, group, permission and so on.

    AttrsInfo() : file_name(""), file_type(FILE_TYPE_UNKNOW), file_size(0) {}
};

} // namespace file
} // namespace common

#endif // COMMON_FILE_HDFS_HDFS_DEF_H
