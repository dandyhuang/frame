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

    // ��������: ��ʼ������;
    // �������: file_name���ļ���;
    // ����ֵ:   ��ʼ���Ƿ�ɹ���true��ʾ�ɹ���false��ʾʧ��;
    bool Open(const ::std::string& file_name, const uint32_t permission = 0660);

    // ��������: �ر�;
    bool Close();

    // ��������: �ȴ��Ӷ�������;
    void ReaderLock();

    // ��������: ���ԼӶ�������;
    // ����ֵ:   ���Զ����Ƿ�ɹ���true��ʾ�ɹ���false��ʾʧ��;
    bool TryReaderLock();

    // ��������: �ȴ���д������;
    // �������: ��;
    void WriterLock();

    // ��������: ���Լ�д������;
    // ����ֵ:   ���Լ�д���Ƿ�ɹ���true��ʾ�ɹ���false��ʾʧ��;
    bool TryWriterLock();

    // ��������: ��������;
    void Unlock();

    // Adapt to ScopedTryReaderLocker and ScopedTryWriterLocker
    void ReaderUnlock() { return Unlock(); }
    void WriterUnlock() { return Unlock(); }

    bool IsLocked(pid_t *pid = NULL, bool *is_read_or_write_lock = NULL) const;

private:
    // ��������: ����������;
    // �������: cmd��������ָ��;
    // �������: type��״̬;
    // �������: offset�������whence��ƫ����;
    // �������: whence�����ַ�ʽ��SEEK_SET ��SEEK_CUR��SEEK_END;
    // �������: len�����õĳ���;
    // ����ֵ:   ����������;
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

