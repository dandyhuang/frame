//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 17:21
//  @file:      fixed_thread_pool.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_FIXED_THREAD_POOL_H
#define COMMON_SYSTEM_CONCURRENCY_FIXED_THREAD_POOL_H

#include <stddef.h>
#include "thirdparty/monitor_sdk_common/base/closure2.h"
#include "thirdparty/monitor_sdk_common/base/stdext/intrusive_list.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/atomic.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/condition_variable.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/thread.h"

namespace common {

class FixedThreadPool
{
public:
    struct Stats
    {
        size_t NumThreads;
        size_t NumPendingTasks;
    };

    /// @param mun_threads number of threads, -1 means cpu number
    explicit FixedThreadPool(int num_threads = -1);
    ~FixedThreadPool();

    void AddTask(Thread::StartRoutine routine,
                 void* context = NULL,
                 unsigned long long param = 0,
                 int dispatch_key = -1);
    void AddTask(Closure<void>* callback, int dispatch_key = -1);

    void Terminate(bool wait = true, int timeout = 1000);
    void WaitForIdle();
    void GetStats(Stats* stats) const;

private:
    struct Task
    {
        list_node link;

        // Description about this task by Thread::StartRoutine.
        Thread::StartRoutine Routine;
        void* Context;
        unsigned long long Param; ///< any param

        // Description about this task by Closure.
        // If it's set, ignore the above routine.
        Closure<void>* callback;
    };

    struct PooledThread : public Thread
    {
        mutable SimpleMutex Mutex;
        ConditionVariable Cond;
        intrusive_list<Task> PendingTasks;
    } __attribute__((aligned(64))); // Make cache alignment.

    void AddTaskInternal(Thread::StartRoutine routine,
                         void* context,
                         unsigned long long param,
                         Closure<void>* callback,
                         int dispatch_key);
    Task* AllocateTask();
    void ReleaseTask(Task* task);
    bool AnyTaskPending() const;
    void WorkRoutine(PooledThread* thread);
    Task* GetPendingTask(PooledThread* thread);
    bool AnyThreadRunning() const;

private:
    PooledThread* m_Threads;
    size_t m_NumThreads;

    bool m_Exit;
    mutable Mutex m_Mutex;
    intrusive_list<Task> m_FreeTasks;
};

} // end of namespace common

#endif // COMMON_SYSTEM_CONCURRENCY_FIXED_THREAD_POOL_H

