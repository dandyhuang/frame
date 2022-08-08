//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 13:05
//  @file:      fixed_thread_pool.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/fixed_thread_pool.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/this_thread.h"
#include "thirdparty/monitor_sdk_common/system/system_information.h"
#include "thirdparty/monitor_sdk_common/system/time/timestamp.h"

// GLOBAL_NOLINT(runtime/int)

namespace common {

FixedThreadPool::FixedThreadPool(int num_threads):
    m_NumThreads(0),
    m_Exit(false)
{
    if (num_threads < 0)
        m_NumThreads = GetLogicalCpuNumber();
    else if (num_threads == 0)
        m_NumThreads = 1;
    else
        m_NumThreads = num_threads;

    m_Threads = new PooledThread[m_NumThreads];
    for (size_t i = 0; i < m_NumThreads; ++i)
    {
        m_Threads[i].Initialize(
            MAKE_PARAMETERIZED_THREAD_CALLBACK(FixedThreadPool, WorkRoutine, PooledThread*),
            this, (unsigned long long) &m_Threads[i]
        );
        m_Threads[i].Start();
    }
}

FixedThreadPool::~FixedThreadPool()
{
    Terminate();
}

void FixedThreadPool::AddTask(Thread::StartRoutine routine,
                              void* context,
                              unsigned long long param,
                              int dispatch_key)
{
    AddTaskInternal(routine, context, param, NULL, dispatch_key);
}

void FixedThreadPool::AddTask(Closure<void>* callback, int dispatch_key)
{
    AddTaskInternal(NULL, NULL, 0, callback, dispatch_key);
}

void FixedThreadPool::AddTaskInternal(Thread::StartRoutine routine,
                                      void* context,
                                      unsigned long long param,
                                      Closure<void>* callback,
                                      int dispatch_key)
{
    Task* task = AllocateTask();
    task->Routine = routine;
    task->Context = context;
    task->Param = param;
    task->callback = callback;

    if (dispatch_key < 0) {
        // The memory address is random enough for load balance, but need
        // remove low alignment part. (The lowest 4 bits of allocated object
        // address are always 0 for 64 bit system).
        dispatch_key = reinterpret_cast<uintptr_t>(task) / 16;
    }
    PooledThread& thread = m_Threads[dispatch_key % m_NumThreads];
    {
        MutexLocker locker(thread.Mutex);
        thread.PendingTasks.push_back(*task);
    }
    thread.Cond.Signal();
}

void FixedThreadPool::WorkRoutine(PooledThread* thread)
{
    while (!m_Exit)
    {
        Task* task = GetPendingTask(thread);
        if (task)
        {
            if (task->callback == NULL) {
                task->Routine(task->Context, task->Param);
            } else {
                task->callback->Run();
            }
            ReleaseTask(task);
        }
    }
}

FixedThreadPool::Task* FixedThreadPool::AllocateTask()
{
    // There is a Task pool at the initial version of this code. But after
    // test, allocate a new Task with tcmalloc is faster than the pool, so we
    // removed it.
    // TODO(phongchen): Need a lockfree queue for case without tcmalloc.
    return new Task();
}

void FixedThreadPool::ReleaseTask(Task* task)
{
    delete task;
}

FixedThreadPool::Task* FixedThreadPool::GetPendingTask(PooledThread* thread)
{
    Task* task = NULL;
    MutexLocker locker(thread->Mutex);
    while (thread->PendingTasks.empty() && !m_Exit)
    {
        thread->Cond.Wait(thread->Mutex, -1);
    }
    if (!thread->PendingTasks.empty())
    {
        task = &thread->PendingTasks.front();
        thread->PendingTasks.pop_front();
    }
    return task;
}

bool FixedThreadPool::AnyTaskPending() const
{

    for (size_t i = 0; i < m_NumThreads; ++i)
    {
        MutexLocker locker(m_Threads[i].Mutex);
        if (!m_Threads[i].PendingTasks.empty())
            return true;
    }

    return false;
}

bool FixedThreadPool::AnyThreadRunning() const
{
    for (size_t i = 0; i < m_NumThreads; ++i)
    {
        if (m_Threads[i].IsAlive())
            return true;
    }
    return false;
}

void FixedThreadPool::Terminate(bool wait, int timeout)
{
    if (!m_Exit)
    {
        if (wait)
        {
            while (AnyTaskPending())
            {
                ThisThread::Sleep(1);
            }
        }

        m_Exit = true;
    }

    while (AnyThreadRunning())
    {
        for (size_t i = 0; i < m_NumThreads; ++i)
        {
            MutexLocker locker(m_Threads[i].Mutex);
            m_Threads[i].Cond.Signal();
        }
        ThisThread::Sleep(1);
    }

    {
        MutexLocker locker(m_Mutex);
        while (!m_FreeTasks.empty())
        {
            Task* task = &m_FreeTasks.front();
            m_FreeTasks.pop_front();
            delete task;
        }
    }

    for (size_t i = 0; i < m_NumThreads; ++i)
        m_Threads[i].Join();

    MutexLocker locker(m_Mutex);
    delete[] m_Threads;
    m_Threads = NULL;
    m_NumThreads = 0;
}

void FixedThreadPool::WaitForIdle()
{
    assert(!m_Exit);
    while (AnyTaskPending())
    {
        ThisThread::Sleep(1);
    }
}

void FixedThreadPool::GetStats(Stats* stats) const
{
    stats->NumThreads = m_NumThreads;
    stats->NumPendingTasks = 0;
    for (size_t i = 0; i < m_NumThreads; ++i)
    {
        MutexLocker locker(m_Threads[i].Mutex);
        stats->NumPendingTasks += m_Threads[i].PendingTasks.size();
    }
}

} // namespace common
