//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 17:33
//  @file:      condition_variable.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/condition_variable.h"

#include <assert.h>
#if __unix__
#include <sys/time.h>
#endif

#include <stdexcept>
#include <string>

#include "thirdparty/monitor_sdk_common/system/check_error.h"

#ifdef _WIN32

#include "thirdparty/monitor_sdk_common/base/common_windows.h"

namespace common {

ConditionVariable::ConditionVariable()
{
    m_hCondition = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    m_nWaitCount = 0;
    CHECK_WINDOWS_ERROR(m_hCondition != NULL);
}

ConditionVariable::~ConditionVariable()
{
    CHECK_WINDOWS_ERROR(::CloseHandle(m_hCondition));
}

void ConditionVariable::CheckValid() const
{
}

void ConditionVariable::Wait(MutexBase* mutex)
{
    // clear previous state
    CHECK_WINDOWS_ERROR(::ResetEvent(m_hCondition));
    mutex->Unlock();
    m_nWaitCount++;
    DWORD theErr = ::WaitForSingleObject(m_hCondition, INFINITE);
    m_nWaitCount--;
    mutex->Lock();
    CHECK_WINDOWS_WAIT_ERROR(theErr);
}

bool ConditionVariable::Wait(MutexBase* mutex, int timeout_in_ms)
{
    // clear previous state
    CHECK_WINDOWS_ERROR(::ResetEvent(m_hCondition));
    mutex->Unlock();
    m_nWaitCount++;
    DWORD theErr = ::WaitForSingleObject(m_hCondition, timeout_in_ms);
    m_nWaitCount--;
    mutex->Lock();

    return CHECK_WINDOWS_WAIT_ERROR(theErr);
}

void ConditionVariable::Signal()
{
    if (!::SetEvent(m_hCondition))
        throw std::runtime_error("ConditionVariable::Signal");
}

void ConditionVariable::Broadcast()
{
    // There doesn't seem like any more elegant way to
    // implement Broadcast using events in Win32.
    // This will work, it may generate spurious wakeups,
    // but condition variables are allowed to generate
    // spurious wakeups
    unsigned int waitCount = m_nWaitCount;
    for (unsigned int x = 0; x < waitCount; x++)
    {
        CHECK_WINDOWS_ERROR(::SetEvent(m_hCondition));
    }
}

} // end of namespace common

#elif defined __unix__

#include "thirdparty/monitor_sdk_common/system/time/posix_time.h"

namespace common {

ConditionVariable::ConditionVariable()
{
    pthread_condattr_t cond_attr;
    CHECK_PTHREAD_ERROR(pthread_condattr_init(&cond_attr));
    CHECK_PTHREAD_ERROR(pthread_cond_init(&m_hCondition, &cond_attr));
    CHECK_PTHREAD_ERROR(pthread_condattr_destroy(&cond_attr));
}

ConditionVariable::~ConditionVariable()
{
    CHECK_PTHREAD_ERROR(pthread_cond_destroy(&m_hCondition));
}

void ConditionVariable::CheckValid() const
{
//#ifdef __linux__
//    // __total_seq will be set to -1 by pthread_cond_destroy
//    assert(m_hCondition.__data.__total_seq != -1ULL && "this cond has been destructed");
//#endif
}

void ConditionVariable::Signal()
{
    CheckValid();
    CHECK_PTHREAD_ERROR(pthread_cond_signal(&m_hCondition));
}

void ConditionVariable::Broadcast()
{
    CheckValid();
    CHECK_PTHREAD_ERROR(pthread_cond_broadcast(&m_hCondition));
}

void ConditionVariable::Wait(MutexBase* mutex)
{
    CheckValid();
    CHECK_PTHREAD_ERROR(pthread_cond_wait(&m_hCondition, &mutex->m_mutex));
}

bool ConditionVariable::Wait(MutexBase* mutex, int timeout_in_ms)
{
    // -1 转为无限等
    if (timeout_in_ms < 0)
    {
        Wait(mutex);
        return true;
    }

    timespec ts;
    RelativeTimeInMillSecondsToAbsTimeInTimeSpec(timeout_in_ms, &ts);
    return CHECK_PTHREAD_TIMED_ERROR(pthread_cond_timedwait(&m_hCondition, &mutex->m_mutex, &ts));
}

} // end of namespace common

#endif

