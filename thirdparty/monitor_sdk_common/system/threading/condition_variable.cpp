// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>

#include "thirdparty/monitor_sdk_common/system/threading/condition_variable.h"

#include <assert.h>
#include <sys/time.h>

#include <stdexcept>
#include <string>

#include "thirdparty/monitor_sdk_common/system/check_error.h"
#include "thirdparty/monitor_sdk_common/system/threading/mutex.h"
#include "thirdparty/monitor_sdk_common/system/time/posix_time.h"

namespace common {

ConditionVariable::ConditionVariable(internal::MutexBase* mutex)
{
    COMMON_CHECK_PTHREAD_ERROR(pthread_cond_init(&m_cond, NULL));
    m_mutex = mutex;
}

ConditionVariable::~ConditionVariable()
{
    COMMON_CHECK_PTHREAD_ERROR(pthread_cond_destroy(&m_cond));
    m_mutex = NULL;
}

void ConditionVariable::CheckValid() const
{
    // __total_seq will be set to -1 by pthread_cond_destroy
    assert(m_mutex != NULL && "this cond has been destructed");
}

void ConditionVariable::Signal()
{
    CheckValid();
    COMMON_CHECK_PTHREAD_ERROR(pthread_cond_signal(&m_cond));
}

void ConditionVariable::Broadcast()
{
    CheckValid();
    COMMON_CHECK_PTHREAD_ERROR(pthread_cond_broadcast(&m_cond));
}

void ConditionVariable::Wait()
{
    CheckValid();
    COMMON_CHECK_PTHREAD_ERROR(pthread_cond_wait(&m_cond, &m_mutex->m_mutex));
}

bool ConditionVariable::TimedWait(int64_t timeout_in_ms)
{
    timespec ts;
    RelativeMilliSecondsToAbsolute(timeout_in_ms, &ts);
    return COMMON_CHECK_PTHREAD_TIMED_ERROR(
        pthread_cond_timedwait(&m_cond, &m_mutex->m_mutex, &ts));
}

} // namespace common

