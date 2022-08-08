// Copyright (c) 2014, Vivo Inc.
// All rights reserved.
//
// Author: rabbitliu <liubin18@baidu.com>
// Created: 01/10/14
// Description:

#ifndef COMMON_FILE_HDFS_MOCK_HDFS_H
#define COMMON_FILE_HDFS_MOCK_HDFS_H
#pragma once

#include "thirdparty/monitor_sdk_common/storage/file/hdfs/hdfs_interface.h"

#include "thirdparty/gmock/gmock.h"

namespace common {

class MockHDFS : public HdfsFileInterface {
public:
    MockHDFS() {}
    ~MockHDFS() {}

    MOCK_METHOD2(hdfsConnect, hdfsFS(const char*, tPort));
    MOCK_METHOD1(hdfsDisconnect, int(hdfsFS));
    MOCK_METHOD6(hdfsOpenFile, hdfsFile(hdfsFS, const char*, int, int, short, tSize));
    MOCK_METHOD2(hdfsCloseFile, int(hdfsFS, hdfsFile));
    MOCK_METHOD2(hdfsExists, int(hdfsFS, const char*));
    MOCK_METHOD4(hdfsRead, tSize(hdfsFS, hdfsFile, void*, tSize));
    MOCK_METHOD4(hdfsWrite, tSize(hdfsFS fs, hdfsFile file, const void* buffer, tSize length));
    MOCK_METHOD2(hdfsFlush, int(hdfsFS, hdfsFile));
    MOCK_METHOD2(hdfsHFlush, int(hdfsFS, hdfsFile));
    MOCK_METHOD2(hdfsHSync, int(hdfsFS, hdfsFile));
    MOCK_METHOD2(hdfsTell, tOffset(hdfsFS, hdfsFile));
    MOCK_METHOD3(hdfsSeek, int(hdfsFS, hdfsFile, tOffset));
    MOCK_METHOD2(hdfsGetPathInfo, hdfsFileInfo*(hdfsFS, const char*));
    MOCK_METHOD3(hdfsListDirectory, hdfsFileInfo*(hdfsFS, const char*, int*));
    MOCK_METHOD2(hdfsFreeFileInfo, void(hdfsFileInfo*, int));
    MOCK_METHOD2(hdfsCreateDirectory, int(hdfsFS, const char*));
    MOCK_METHOD3(hdfsDelete, int(hdfsFS, const char*, int));
    MOCK_METHOD3(hdfsRename, int(hdfsFS, const char*, const char*));
};

} // namespace common

#endif // COMMON_FILE_HDFS_MOCK_HDFS_H
