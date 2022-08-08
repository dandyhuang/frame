//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-08-29 18:57
//  @file:      named_pipe_unix.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/named_pipe.h"

#ifdef __unix__

namespace common {

NamedPipe::NamedPipe():
    m_mode(0),
    m_nonblocking(false),
    m_fd(-1),
    m_type(kReadOnly)
{
}

NamedPipe::~NamedPipe() {
    Close();
}

bool NamedPipe::Open(
    const char *fifo_name,
    NamedPipeType fifo_type,
    bool nonblocking,
    mode_t mode)
{
    m_type = fifo_type;
    m_name = fifo_name;
    m_mode = mode;
    m_nonblocking = nonblocking;
    return DoOpen();
}

bool NamedPipe::SetNonblock(bool nonblocking) {
    if (m_fd < 0)
        return false;

    int flags = fcntl(m_fd, F_GETFL, 0);
    if (flags == -1)
        return false;

    if (nonblocking) {
        if (flags & O_NONBLOCK)
            return true;
        flags |= O_NONBLOCK;
    } else {
        if ((flags & O_NONBLOCK) == 0)
            return true;
        flags &= ~O_NONBLOCK;
    }

    if (fcntl(m_fd, F_SETFL, flags) == -1)
        return false;

    m_nonblocking = nonblocking;
    return true;
}

bool NamedPipe::IsNonblock() const {
    int flags = fcntl(m_fd, F_GETFL, 0);
    if (flags == -1)
        return false;

    return (flags & O_NONBLOCK) != 0;
}

bool NamedPipe::DoOpen()
{
    if (mkfifo(m_name.c_str(), m_mode) != 0 && errno != EEXIST)
        return false;

    int flags = m_nonblocking ? O_NONBLOCK : 0;
    switch (m_type) {
    case kReadOnly:
        flags |= O_RDONLY;
        break;
    case kWriteOnly:
        flags |= O_WRONLY;
        break;
    default:
        return false;
        break;
    }

    m_fd = open(m_name.c_str(), flags);
    return m_fd >= 0;
}

bool NamedPipe::Reopen()
{
    Close();
    return DoOpen();
}

void NamedPipe::Close() {
    if (m_fd >= 0) {
        close(m_fd);
        m_fd = -1;
    }
}

int NamedPipe::Read(void *buffer, size_t buffer_len) {
    int return_code = -1;

    for (;;) {
        return_code = read(m_fd, buffer, buffer_len);
        if (return_code < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN) {
                return return_code;
            } else {
                // 出错，重新打开
                if (!Reopen())
                    return return_code;
            }
        } else if (return_code == 0) {
            if (!Reopen())
                return return_code;
        } else {
            break;
        }
    }

    return return_code;
}

int NamedPipe::Write(const void *buffer, size_t buffer_len) {
    int return_code = -1;

    if (m_fd < 0 && kWriteOnly == m_type) {
        // reopen fifo
        m_fd = open(m_name.c_str(), O_NONBLOCK | O_WRONLY);
        if (m_fd < 0) {
            if (errno == ENXIO) {
                return -1;
            }

            return -1;
        }
    }

    for (;;) {
        return_code = write(m_fd, buffer, buffer_len);
        if (return_code < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN || errno == EPIPE) {
                return return_code;
            } else {
                if (!Reopen())
                    return return_code;
            }
        } else if (return_code == 0) {
            if (!Reopen())
                return return_code;
        } else {
            break;
        }
    }

    return return_code;
}

} // namespace common

#endif
