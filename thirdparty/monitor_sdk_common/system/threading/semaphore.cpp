// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "thirdparty/monitor_sdk_common/system/threading/semaphore.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

namespace common {
namespace internal {

bool SemaphoreOps::TryAcquire(sem_t* sem)
{
    if (COMMON_EINTR_IGNORED(sem_trywait(sem)) == 0)
        return true;
    int error = errno;
    if (error == EAGAIN)
        return false;
    COMMON_CHECK_ERRNO_ERROR(error);
    return true;
}

bool SemaphoreOps::TimedAcquire(sem_t* sem, int64_t timeout) // in ms
{
    struct timespec ts;
    RelativeMilliSecondsToAbsolute(timeout, &ts);
    return COMMON_CHECK_POSIX_TIMED_ERROR(
        COMMON_EINTR_IGNORED(sem_timedwait(sem, &ts)));
}

} // namespace internal

NamedSemaphore::NamedSemaphore(const std::string& name) :
    m_sem(NULL)
{
    if (!Open(name))
        COMMON_CHECK_POSIX_ERROR(-1);
}

NamedSemaphore::NamedSemaphore(const std::string& name,
                               unsigned int value,
                               mode_t mode) :
    m_sem(NULL)
{
    if (!OpenOrCreate(name, value, mode))
        COMMON_CHECK_POSIX_ERROR(-1);
}

NamedSemaphore::~NamedSemaphore()
{
    Close();
}

void NamedSemaphore::CheckNull() const
{
    if (m_sem)
    {
        fprintf(stderr, "NamedSemaphore::Open: Already opend\n");
        abort();
    }
}

bool NamedSemaphore::Open(const std::string& name)
{
    CheckNull();
    sem_t* sem = sem_open(name.c_str(), 0, 0666, 0);
    if (!sem)
        return false;
    m_sem = sem;
    return true;
}

bool NamedSemaphore::Create(const std::string& name,
                            unsigned int value,
                            mode_t mode)
{
    CheckNull();
    sem_t* sem = sem_open(name.c_str(), O_CREAT | O_EXCL, mode, value);
    if (!sem)
        return false;
    m_sem = sem;
    return true;
}

bool NamedSemaphore::OpenOrCreate(const std::string& name,
                                  unsigned int value,
                                  mode_t mode)
{
    CheckNull();
    sem_t* sem = sem_open(name.c_str(), O_CREAT, mode, value);
    if (!sem)
        return false;
    m_sem = sem;
    return true;
}

bool NamedSemaphore::Close()
{
    if (m_sem)
    {
        COMMON_CHECK_POSIX_ERROR(sem_close(m_sem));
        m_sem = NULL;
        return true;
    }
    return false;
}

bool NamedSemaphore::Unlink(const std::string& name)
{
    if (sem_unlink(name.c_str()) == 0)
        return true;
    if (errno != ENOENT)
        COMMON_CHECK_ERRNO_ERROR(-1);
    return false;
}

} // namespace common
