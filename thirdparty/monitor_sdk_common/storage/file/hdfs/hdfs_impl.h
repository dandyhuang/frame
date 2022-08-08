// Copyright (c) 2014, Vivo Inc.
// All rights reserved.
//
// Author: rabbitliu <liubin18@baidu.com>
// Created: 01/10/14
// Description:

#ifndef COMMON_FILE_HDFS_HDFS_IMPL_H
#define COMMON_FILE_HDFS_HDFS_IMPL_H
#pragma once

#include "thirdparty/monitor_sdk_common/storage/file/hdfs/hdfs_interface.h"

namespace common {

class HDFSImpl : public HdfsFileInterface {
public:
    virtual ~HDFSImpl() {}

    virtual hdfsFS hdfsConnect(const char* host, tPort port) {
        return ::hdfsConnect(host, port);
    }

    virtual int hdfsDisconnect(hdfsFS fs) {
        return ::hdfsDisconnect(fs);
    }

    virtual hdfsFile hdfsOpenFile(hdfsFS fs, const char* path, int flags,
                                  int bufferSize, short replication, tSize blocksize) {
        return ::hdfsOpenFile(fs, path, flags, bufferSize, replication, blocksize);
    }

    virtual int hdfsCloseFile(hdfsFS fs, hdfsFile file) {
        return ::hdfsCloseFile(fs, file);
    }

    virtual int hdfsExists(hdfsFS fs, const char *path) {
        return ::hdfsExists(fs, path);
    }

    virtual tSize hdfsRead(hdfsFS fs, hdfsFile file, void* buffer, tSize length) {
        return ::hdfsRead(fs, file, buffer, length);
    }

    virtual tSize hdfsWrite(hdfsFS fs, hdfsFile file, const void* buffer, tSize length) {
        return ::hdfsWrite(fs, file, buffer, length);
    }

    virtual tOffset hdfsTell(hdfsFS fs, hdfsFile file) {
        return ::hdfsTell(fs, file);
    }

    virtual int hdfsSeek(hdfsFS fs, hdfsFile file, tOffset desiredPos) {
        return ::hdfsSeek(fs, file, desiredPos);
    }

    virtual int hdfsFlush(hdfsFS fs, hdfsFile file) {
        return ::hdfsFlush(fs, file);
    }

    virtual int hdfsHFlush(hdfsFS fs, hdfsFile file) {
        return ::hdfsHFlush(fs, file);
    }

    virtual int hdfsHSync(hdfsFS fs, hdfsFile file) {
        return ::hdfsHSync(fs, file);
    }

    virtual hdfsFileInfo* hdfsListDirectory(hdfsFS fs, const char* path, int* numEntries) {
        return ::hdfsListDirectory(fs, path, numEntries);
    }

    virtual hdfsFileInfo* hdfsGetPathInfo(hdfsFS fs, const char* path) {
        return ::hdfsGetPathInfo(fs, path);
    }

    virtual void hdfsFreeFileInfo(hdfsFileInfo *hdfsFileInfo, int numEntries) {
        return ::hdfsFreeFileInfo(hdfsFileInfo, numEntries);
    }

    virtual int hdfsCreateDirectory(hdfsFS fs, const char* path) {
        return ::hdfsCreateDirectory(fs, path);
    }

    virtual int hdfsDelete(hdfsFS fs, const char* path, int recursive) {
        return ::hdfsDelete(fs, path, recursive);
    }

    virtual int hdfsRename(hdfsFS fs, const char* oldpath, const char* newpath) {
        return ::hdfsRename(fs, oldpath, newpath);
    }
};

} // namespace common
#endif // COMMON_FILE_HDFS_HDFS_IMPL_H
