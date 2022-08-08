//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 15:58
//  @file:      thread_group.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_THREAD_GROUP_H
#define COMMON_SYSTEM_CONCURRENCY_THREAD_GROUP_H
#pragma once

#include <vector>

#include "thirdparty/monitor_sdk_common/base/uncopyable.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/thread.h"

namespace common {

class ThreadGroup
{
    COMMON_DECLARE_UNCOPYABLE(ThreadGroup);
public:
    ThreadGroup();
    ThreadGroup(const Function<void ()>& callback, size_t count);
    ~ThreadGroup();
    void Add(const Function<void ()>& callback, size_t count = 1);
    void Start();
    void Join();
    size_t Size() const;
private:
    ::std::vector<Thread*> m_threads;
};

} // end of namespace common

#endif // COMMON_SYSTEM_CONCURRENCY_THREAD_GROUP_H
