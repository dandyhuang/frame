// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef COMMON_SYSTEM_THREADING_CONDITION_VARIABLE_H
#define COMMON_SYSTEM_THREADING_CONDITION_VARIABLE_H

#include <assert.h>
#include <pthread.h>
#include <stdint.h>

namespace common {
namespace internal {
class MutexBase;
} // namespace internal

class ConditionVariable
{
public:
    explicit ConditionVariable(internal::MutexBase* mutex);
    ~ConditionVariable();
    void Signal();
    void Broadcast();
    void Wait();
    bool TimedWait(int64_t timeout_in_ms);
private:
    void CheckValid() const;
private:
    pthread_cond_t m_cond;
    internal::MutexBase* m_mutex;
};

} // namespace common

#endif // COMMON_SYSTEM_THREADING_CONDITION_VARIABLE_H

