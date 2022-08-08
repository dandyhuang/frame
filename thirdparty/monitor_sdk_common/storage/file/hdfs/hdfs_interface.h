// Copyright (c) 2014, Vivo Inc.
// All rights reserved.
//
// Author: liubin18 <liubin18@baidu.com>
// Created: 01/10/14
// Description:

#ifndef COMMON_FILE_HDFS_HDFS_INTERFACE_H
#define COMMON_FILE_HDFS_HDFS_INTERFACE_H
#pragma once

#include "thirdparty/libhdfs-2.5.0/hdfs.h"

namespace common {

// File接口，为了后面测试时好Mock
class HdfsFileInterface {
public:
    HdfsFileInterface() {}
    virtual ~HdfsFileInterface() {}
    /**
     * hdfsConnect - Connect to a hdfs file system.
     * Connect to the hdfs.
     * @param host A string containing either a host name, or an ip address
     * of the namenode of a hdfs cluster. 'host' should be passed as NULL if
     * you want to connect to local filesystem. 'host' should be passed as
     * 'default' (and port as 0) to used the 'configured' filesystem
     * (core-site/core-default.xml).
     * @param port The port on which the server is listening.
     * @return Returns a handle to the filesystem or NULL on error.
     */
    virtual hdfsFS hdfsConnect(const char* host, tPort port) = 0;
    /**
     * hdfsConnect - Connect to a hdfs file system.
     * Connect to the hdfs.
     * @param host A string containing either a host name, or an ip address
     * of the namenode of a hdfs cluster. 'host' should be passed as NULL if
     * you want to connect to local filesystem. 'host' should be passed as
     * 'default' (and port as 0) to used the 'configured' filesystem
     * (core-site/core-default.xml).
     * @param port The port on which the server is listening.
     * @return Returns a handle to the filesystem or NULL on error.
     */
    virtual int hdfsDisconnect(hdfsFS fs) = 0;
    /**
     * hdfsOpenFile - Open a hdfs file in given mode.
     * @param fs The configured filesystem handle.
     * @param path The full path to the file.
     * @param flags - an | of bits/fcntl.h file flags -
     *              supported flags are O_RDONLY, O_WRONLY (meaning create or overwrite i.e.,
     *              implies O_TRUNCAT), O_WRONLY|O_APPEND. Other flags are generally ignored other
     *              than (O_RDWR || (O_EXCL & O_CREAT)) which return NULL and set errno equal ENOTSUP.
     * @param bufferSize Size of buffer for read/write - pass 0 if you want
     * to use the default configured values.
     * @param replication Block replication - pass 0 if you want to use
     * the default configured values.
     * @param blocksize Size of block - pass 0 if you want to use the
     * default configured values.
     * @return Returns the handle to the open file or NULL on error.
     */
    virtual hdfsFile hdfsOpenFile(hdfsFS fs, const char* path, int flags,
                                  int bufferSize, short replication, tSize blocksize) = 0;
    /**
     * hdfsCloseFile - Close an open file.
     * @param fs The configured filesystem handle.
     * @param file The file handle.
     * @return Returns 0 on success, -1 on error.
     */
    virtual int hdfsCloseFile(hdfsFS fs, hdfsFile file) = 0;

    /**
     * hdfsExists - Checks if a given path exsits on the filesystem
     * @param fs The configured filesystem handle.
     * @param path The path to look for
     * @return Returns 0 on success, -1 on error.
     */
    virtual int hdfsExists(hdfsFS fs, const char *path) = 0;
    /**
     * hdfsRead - Read data from an open file.
     * @param fs The configured filesystem handle.
     * @param file The file handle.
     * @param buffer The buffer to copy read bytes into.
     * @param length The length of the buffer.
     * @return Returns the number of bytes actually read, possibly less
     * than than length;-1 on error.
     */
    virtual tSize hdfsRead(hdfsFS fs, hdfsFile file, void* buffer, tSize length) = 0;
    /**
     * hdfsWrite - Write data into an open file.
     * @param fs The configured filesystem handle.
     * @param file The file handle.
     * @param buffer The data.
     * @param length The no. of bytes to write.
     * @return Returns the number of bytes written, -1 on error.
     */
    virtual tSize hdfsWrite(hdfsFS fs, hdfsFile file, const void* buffer, tSize length) = 0;

    /**
     * hdfsTell - Get the current offset in the file, in bytes.
     * @param fs The configured filesystem handle.
     * @param file The file handle.
     * @return Current offset, -1 on error.
     */
    virtual tOffset hdfsTell(hdfsFS fs, hdfsFile file) = 0;

    /**
     * hdfsSeek - Seek to given offset in file.
     * This works only for files opened in read-only mode.
     * @param fs The configured filesystem handle.
     * @param file The file handle.
     * @param desiredPos Offset into the file to seek into.
     * @return Returns 0 on success, -1 on error.
     */
    virtual int hdfsSeek(hdfsFS fs, hdfsFile file, tOffset desiredPos) = 0;

    /**
     * hdfsWrite - Flush the data.
     * @param fs The configured filesystem handle.
     * @param file The file handle.
     * @return Returns 0 on success, -1 on error.
     */
    virtual int hdfsFlush(hdfsFS fs, hdfsFile file) = 0;
    virtual int hdfsHFlush(hdfsFS fs, hdfsFile file) = 0;
    virtual int hdfsHSync(hdfsFS fs, hdfsFile file) = 0;

    /**
     * hdfsGetPathInfo - Get information about a path as a (dynamically
     * allocated) single hdfsFileInfo struct. hdfsFreeFileInfo should be
     * called when the pointer is no longer needed.
     * @param fs The configured filesystem handle.
     * @param path The path of the file.
     * @return Returns a dynamically-allocated hdfsFileInfo object;
     * NULL on error.
     */
    virtual hdfsFileInfo *hdfsGetPathInfo(hdfsFS fs, const char* path) = 0;

    /**
     * hdfsListDirectory - Get list of files/directories for a given
     * directory-path. hdfsFreeFileInfo should be called to deallocate memory.
     * @param fs The configured filesystem handle.
     * @param path The path of the directory.
     * @param numEntries Set to the number of files/directories in path.
     * @return Returns a dynamically-allocated array of hdfsFileInfo
     * objects; NULL on error.
     */
    hdfsFileInfo *hdfsListDirectory(hdfsFS fs, const char* path,
                                    int *numEntries);

    /**
     * hdfsFreeFileInfo - Free up the hdfsFileInfo array (including fields)
     * @param hdfsFileInfo The array of dynamically-allocated hdfsFileInfo
     * objects.
     * @param numEntries The size of the array.
     */
    virtual void hdfsFreeFileInfo(hdfsFileInfo *hdfsFileInfo, int numEntries) = 0;

    /**
     * hdfsCreateDirectory - Make the given file and all non-existent
     * parents into directories.
     * @param fs The configured filesystem handle.
     * @param path The path of the directory.
     * @return Returns 0 on success, -1 on error.
     */
    virtual int hdfsCreateDirectory(hdfsFS fs, const char* path) = 0;

    /**
     * hdfsDelete - Delete file.
     * @param fs The configured filesystem handle.
     * @param path The path of the file.
     * @return Returns 0 on success, -1 on error.
     */
    virtual int hdfsDelete(hdfsFS fs, const char* path, int recursive) = 0;

    /**
     * hdfsRename - Rename file.
     * @param fs The configured filesystem handle.
     * @param oldPath The path of the source file.
     * @param newPath The path of the destination file.
     * @return Returns 0 on success, -1 on error.
     */
    virtual int hdfsRename(hdfsFS fs, const char* oldPath, const char* newPath) = 0;
};

} // namespace common
#endif // COMMON_FILE_HDFS_HDFS_INTERFACE_H
