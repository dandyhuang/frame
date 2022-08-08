//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 10:13
//  @file:      file_lock.cpp
//  @author:    
//  @brief:     implementation of the FileLock class.
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/io/file_lock.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif
#include <sys/fcntl.h>

#ifdef _WIN32
#define open _open
#define close _close
#endif

#include "thirdparty/glog/logging.h"

namespace common {

FileLock::FileLock()
{
    m_fd = -1;
}

FileLock::~FileLock()
{
    if (m_fd != -1)
        Unlock();
    Close();
}

bool FileLock::Open(const ::std::string& file_name, const uint32_t permission)
{
    if (m_fd >= 0) {
        close(m_fd);
    }

    m_fd = open(file_name.c_str(), O_RDWR | O_CREAT, permission);

    if (m_fd < 0) {
        LOG(WARNING) << "open " << file_name << " error.";
        return false;
    }

    return true;
}

bool FileLock::Close()
{
    if (m_fd >= 0) {
        close(m_fd);
        m_fd = -1;
    }
    return true;
}

void FileLock::ReaderLock()
{
    PCHECK(DoLock(F_SETLKW, F_RDLCK, 0, 0, 0) == 0) << "ReaderLock";
}

bool FileLock::TryReaderLock()
{
    return DoLock(F_SETLK, F_RDLCK, 0, 0, 0) == 0;
}

bool FileLock::TryWriterLock()
{
    return DoLock(F_SETLK, F_WRLCK, 0, 0, 0) == 0;
}

void FileLock::WriterLock()
{
    PCHECK(DoLock(F_SETLKW, F_WRLCK, 0, 0, 0) >= 0) << "WriterLock";
}

void FileLock::Unlock() {
    PCHECK(DoLock(F_SETLK, F_UNLCK, 0, 0, 0) ==0) << "Unlock";
}

int FileLock::DoLock(
    int cmd,
    int type,
    int64_t offset,
    int whence,
    int64_t len)
{
    CHECK_NE(m_fd, -1) << "FileLock not opened";

    struct flock lock;
    lock.l_type     = type;
    lock.l_start    = offset;
    lock.l_whence   = whence;
    lock.l_len      = len;

    return fcntl(m_fd, cmd, &lock);
}

bool FileLock::IsLocked(pid_t *pid,  bool *is_read_or_write_lock) const
{
    CHECK_NE(m_fd, -1) << "FileLock not opened";
    struct flock lock;
    lock.l_type     = F_RDLCK;
    lock.l_start    = 0;
    lock.l_whence   = 0;
    lock.l_len      = 0;
    if(fcntl(m_fd, F_GETLK, &lock) == -1){
        LOG(ERROR) << "Can not get file lock info.";
        return false;
    } else if(lock.l_type != F_UNLCK) {
        if (pid != NULL) {
            *pid = lock.l_pid;
        }
        if (is_read_or_write_lock != NULL) {
            if (lock.l_type == F_RDLCK) {
                *is_read_or_write_lock = true;
            } else if (lock.l_type == F_WRLCK) {
                *is_read_or_write_lock = false;
            } else {
                LOG(ERROR) << "Unknown file lock type:" << lock.l_type;
            }
        }
        return true;
    }
    return false;
}


} // end of namespace common
