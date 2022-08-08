// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef COMMON_SYSTEM_THREADING_THREAD_GROUP_H
#define COMMON_SYSTEM_THREADING_THREAD_GROUP_H
#pragma once

#include <vector>

#include "thirdparty/monitor_sdk_common/base/functional.h"
#include "thirdparty/monitor_sdk_common/base/uncopyable.h"
#include "thirdparty/monitor_sdk_common/system/threading/thread.h"

namespace common {

class ThreadGroup
{
    COMMON_DECLARE_UNCOPYABLE(ThreadGroup);
public:
    ThreadGroup();
    ThreadGroup(const std::function<void ()>& callback, size_t count);
    ~ThreadGroup();
    void Add(const std::function<void ()>& callback, size_t count = 1);
    void Start();
    void Join();
    size_t Size() const;
private:
    std::vector<Thread*> m_threads;
};

} // namespace common

#endif // COMMON_SYSTEM_THREADING_THREAD_GROUP_H
