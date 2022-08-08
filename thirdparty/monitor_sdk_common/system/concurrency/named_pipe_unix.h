//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-08-29 18:55
//  @file:      named_pipe_unix.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_NAMED_PIPE_UNIX_H
#define COMMON_SYSTEM_CONCURRENCY_NAMED_PIPE_UNIX_H

#ifndef _WIN32

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include "thirdparty/monitor_sdk_common/base/uncopyable.h"
namespace common {

/**
 @brief ���ڽ��̼�ͨ�ŵ������ܵ�
*/

class NamedPipe : private Uncopyable
{
public:
    enum NamedPipeType {
        kReadOnly, kWriteOnly,
    };
public:
    NamedPipe();
    ~NamedPipe();

    bool Open(
        const char *fifo_name,
        NamedPipeType fifo_type,
        bool nonblocking = false,
        mode_t mode = 0755
    );

    void Close();
    bool SetNonblock(bool onoff = true);
    bool IsNonblock() const;
    int  Read(void *buffer, size_t buffer_len);
    int  Write(const void *buffer, size_t buffer_len);
    int  GetFd() const { return m_fd; }
private:
    bool Reopen();
    bool DoOpen();
private:
    std::string  m_name;  // �����ܵ��ļ���
    int          m_mode;
    bool         m_nonblocking;
    int          m_fd;          // �����ܵ��ļ����
    NamedPipeType m_type;       // �����ܵ��Ķ�д״̬
};

} // namespace common

#endif // _WIN32

#endif // COMMON_SYSTEM_CONCURRENCY_NAMED_PIPE_UNIX_H
