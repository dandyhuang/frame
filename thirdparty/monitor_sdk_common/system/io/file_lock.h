//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 10:07
//  @file:      file_lock.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_IO_FILE_LOCK_H
#define COMMON_SYSTEM_IO_FILE_LOCK_H

#include <string>
#include <sys/types.h>
#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/base/uncopyable.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/scoped_locker.h"

namespace common {

class FileLock {
    COMMON_DECLARE_UNCOPYABLE(FileLock);

public:
    typedef ScopedReaderLocker<FileLock> ReaderLocker;
    typedef ScopedWriterLocker<FileLock> WriterLocker;
    typedef ScopedTryReaderLocker<FileLock> TryReaderLocker;
    typedef ScopedTryWriterLocker<FileLock> TryWriterLocker;

public:
    FileLock();
    ~FileLock();

    // 功能描述: 初始化函数;
    // 输入参数: file_name，文件名;
    // 返回值:   初始化是否成功，true表示成功；false表示失败;
    bool Open(const ::std::string& file_name, const uint32_t permission = 0660);

    // 功能描述: 关闭;
    bool Close();

    // 功能描述: 等待加读锁函数;
    void ReaderLock();

    // 功能描述: 尝试加读锁函数;
    // 返回值:   尝试读锁是否成功，true表示成功；false表示失败;
    bool TryReaderLock();

    // 功能描述: 等待加写锁函数;
    // 输入参数: 无;
    void WriterLock();

    // 功能描述: 尝试加写锁函数;
    // 返回值:   尝试加写锁是否成功，true表示成功；false表示失败;
    bool TryWriterLock();

    // 功能描述: 解锁函数;
    void Unlock();

    // Adapt to ScopedTryReaderLocker and ScopedTryWriterLocker
    void ReaderUnlock() { return Unlock(); }
    void WriterUnlock() { return Unlock(); }

    bool IsLocked(pid_t *pid = NULL, bool *is_read_or_write_lock = NULL) const;

private:
    // 功能描述: 设置锁函数;
    // 输入参数: cmd，操作的指令;
    // 输入参数: type，状态;
    // 输入参数: offset，相对于whence的偏移量;
    // 输入参数: whence，三种方式：SEEK_SET ，SEEK_CUR，SEEK_END;
    // 输入参数: len，设置的长度;
    // 返回值:   返回整型数;
    int DoLock(int cmd,
               int type,
               int64_t offset,
               int whence,
               int64_t len);

private:
    int m_fd;
};

} // end of namespace common

#endif // COMMON_SYSTEM_IO_FILE_LOCK_H

