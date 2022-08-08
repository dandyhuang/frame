//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 15:59
//  @file:      thread_group.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/thread_group.h"
#include <assert.h>
#include "thirdparty/monitor_sdk_common/base/unique_ptr.h"

namespace common {

ThreadGroup::ThreadGroup()
{
}

ThreadGroup::ThreadGroup(const Function<void ()>& callback, size_t count)
{
    Add(callback, count);
}

ThreadGroup::~ThreadGroup()
{
    for (size_t i = 0; i < m_threads.size(); ++i)
    {
        if (m_threads[i]->IsJoinable())
            m_threads[i]->Join();
        delete m_threads[i];
    }
    m_threads.clear();
}

void ThreadGroup::Add(const Function<void ()>& callback, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        std::unique_ptr<Thread> thread(new Thread(callback));
        m_threads.push_back(thread.get());
        thread.release();
    }
}

void ThreadGroup::Start()
{
    assert(Size() > 0);
    for (size_t i = 0; i < m_threads.size(); ++i)
    {
        m_threads[i]->Start();
    }
}

void ThreadGroup::Join()
{
    // TODO(): using pthread_barrier
    for (size_t i = 0; i < m_threads.size(); ++i)
        m_threads[i]->Join();
}

size_t ThreadGroup::Size() const
{
    return m_threads.size();
}

} // end of namespace common
