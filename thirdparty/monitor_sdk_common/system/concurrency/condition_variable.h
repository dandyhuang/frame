//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 17:31
//  @file:      condition_variable.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_CONDITION_VARIABLE_H
#define COMMON_SYSTEM_CONCURRENCY_CONDITION_VARIABLE_H

#include <assert.h>

#if __unix__
#include <pthread.h>
#endif

#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"

// GLOBAL_NOLINT(runtime/references)

namespace common {

class ConditionVariable
{
public:
    ConditionVariable();
    ~ConditionVariable();
    void Signal();
    void Broadcast();

    // If timeout_in_ms is -1, it means infinite and equals to
    // Wait(Mutex* mutex);
    bool Wait(MutexBase* mutex, int timeout_in_ms);
    bool Wait(MutexBase& mutex, int timeout_in_ms)
    {
        return Wait(&mutex, timeout_in_ms);
    }

    void Wait(MutexBase* mutex);
    void Wait(MutexBase& mutex)
    {
        return Wait(&mutex);
    }
private:
    void CheckValid() const;
private:
#ifdef _WIN32
    void* m_hCondition;
    unsigned int m_nWaitCount;
#elif __unix__
    pthread_cond_t m_hCondition;
#endif
};

} // namespace common

#endif // COMMON_SYSTEM_CONCURRENCY_CONDITION_VARIABLE_H

