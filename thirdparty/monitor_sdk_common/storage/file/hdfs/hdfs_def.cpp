// Copyright (c) 2014, Vivo Inc.
// All rights reserved.
//
// Author: rabbitliu <liubin18@baidu.com>
// Created: 01/11/14
// Description:

#include "thirdparty/monitor_sdk_common/storage/file/hdfs/hdfs_def.h"

namespace common {
namespace file {

std::string OpenModelToString(int32_t open_mode)
{
    std::string result;
    result.append((open_mode & kRead) != 0  ? "r" : "-");
    result.append((open_mode & kWrite) != 0  ? "w" : "-");
    result.append((open_mode & kAppend) != 0  ? "a" : "-");
    return result;
}

} // namespace file
} // namespace common
