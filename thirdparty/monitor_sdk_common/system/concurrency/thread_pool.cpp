//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 18:12
//  @file:      thread_pool.cpp
//  @author:    	
//  @brief:     
//
//********************************************************************


#include <assert.h>
// #include "thirdparty/monitor_sdk_common/base/module.h"
#include "thirdparty/monitor_sdk_common/base/singleton.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/this_thread.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/thread_pool.h"
#include "thirdparty/monitor_sdk_common/system/system_information.h"
#include "thirdparty/monitor_sdk_common/system/time/timestamp.h"

// DEFINE_MODULE(ThreadPool)
// {
//     ThreadPool::DefaultInstance();
//     return true;
// }

namespace common {

ThreadPool::ThreadPool(
        int min_threads,
        int max_threads,
        int idle_timeout,
        size_t stack_size)
{
    CommonConstruct("unnamed", min_threads, max_threads,
            idle_timeout, stack_size);
}

ThreadPool::ThreadPool(
    const std::string& name,
    int min_threads,
    int max_threads,
    int idle_timeout,
    size_t stack_size)
{
    CommonConstruct(name, min_threads, max_threads, idle_timeout, stack_size);
}

void ThreadPool::CommonConstruct(
    const std::string& name,
    int min_threads,
    int max_threads,
    int idle_timeout,
    size_t stack_size)
{
    m_name = name;
    m_Exit = false;
    m_Terminating = false;
    m_IdleTime = idle_timeout;
    m_NumThreads = 0;
    m_NumBusyThreads = 0;
    m_NumPendingTasks = 0;
    m_StackSize = stack_size;

    if (min_threads < 0)
        m_MinThreads = GetLogicalCpuNumber();
    else
        m_MinThreads = min_threads;

    if (max_threads < 0)
        m_MaxThreads = GetLogicalCpuNumber();
    else
        m_MaxThreads = max_threads;

    if (m_MaxThreads < m_MinThreads)
        m_MaxThreads = m_MinThreads;
}

ThreadPool::~ThreadPool()
{
    Terminate();
}

void ThreadPool::SetMinThreads(int size)
{
    assert(!m_Exit);
    if (size < 0)
        m_MinThreads = GetLogicalCpuNumber();
    else
        m_MinThreads = size;
}

void ThreadPool::SetMaxThreads(int size)
{
    assert(!m_Exit);

    if (size < 0)
        m_MaxThreads = GetLogicalCpuNumber();
    else
        m_MaxThreads = size;

    if (m_MaxThreads < m_MinThreads)
        m_MaxThreads = m_MinThreads;
}

void ThreadPool::SetIdleTime(int time)
{
    assert(!m_Exit);
    m_IdleTime = time;
}

void ThreadPool::AddTask(Thread::StartRoutine routine,
                         void* context,
                         unsigned long long param)
{
    AddTaskInternal(false, routine, context, param, NULL, NULL);
}

void ThreadPool::AddTask(Closure<void>* callback)
{
    AddTaskInternal(false, NULL, NULL, 0, callback, NULL);
}

void ThreadPool::AddTask(const Function<void ()>& function)
{
    AddTaskInternal(false, NULL, NULL, 0, NULL, function);
}

void ThreadPool::AddPriorityTask(Closure<void>* callback)
{
    AddTaskInternal(true, NULL, NULL, 0, callback, NULL);
}

void ThreadPool::AddPriorityTask(const Function<void ()>& function)
{
    AddTaskInternal(true, NULL, NULL, 0, NULL, function);
}

void ThreadPool::AddTaskInternal(
    bool is_priority,
    Thread::StartRoutine routine,
    void* context,
    unsigned long long param,
    Closure<void>* callback,
    const Function<void ()>& function)
{
    assert(!m_Terminating);

    {
        MutexLocker locker(m_Mutex);

        TaskNode* task = NULL;

        // try allocate from freelist first
        if (!m_FreeTasks.empty())
        {
            task = &m_FreeTasks.front();
            m_FreeTasks.pop_front();
        }
        else
        {
            task = new TaskNode;
        }

        task->Routine = routine;
        task->Context = context;
        task->Param = param;
        task->callback = callback;
        task->function = function;

        if (is_priority)
            m_PendingTasks.push_front(*task);
        else
            m_PendingTasks.push_back(*task);

        ++m_NumPendingTasks;

        if (NeedNewThread())
        {
            AddThread();
        }
    }

    m_NewTaskCond.Signal();
}

bool ThreadPool::NeedNewThread() const
{
    if (m_NumThreads >= m_MaxThreads)
        return false;

    if (m_NumThreads < m_MinThreads)
        return true;

    // all active thread are busy
    if (m_NumBusyThreads == m_NumThreads)
        return true;

    return false;
}

void ThreadPool::AddThread()
{
    PooledThread* thread = NULL;
    if (!m_FreedThreads.empty())
    {
        // allocate from free list
        thread = &m_FreedThreads.front();
        m_FreedThreads.pop_front();
        thread->Join();

        // destroy and recreate
        thread->~PooledThread();
        new (thread) PooledThread();
    }
    else
    {
        thread = new PooledThread();
    }

    Thread::StartRoutine start_routine =
        MAKE_PARAMETERIZED_THREAD_CALLBACK(ThreadPool, WorkRoutine, PooledThread*);
    m_Threads.push_back(*thread);
    ++m_NumThreads;
    thread->Initialize(start_routine, this, (unsigned long long)thread);
    if (m_StackSize != 0)
    {
        thread->SetStackSize(m_StackSize);
    }
    thread->Start();
}

bool ThreadPool::DequeueTask(Task* task)
{
    if (!m_PendingTasks.empty())
    {
        TaskNode* front = &m_PendingTasks.front();
        m_PendingTasks.pop_front();
        --m_NumPendingTasks;
        *task = *front;

        // recycle task node
        front->function = NULL;
        m_FreeTasks.push_back(*front);

        return true;
    }

    return false;
}

void ThreadPool::Task::Run() const
{
    if (this->function) {
        this->function();
    } else if (this->callback) {
        this->callback->Run();
    } else {
        this->Routine(this->Context, this->Param);
    }
}

void ThreadPool::WorkerLoop()
{
    while (!m_Exit)
    {
        Task task;

        {
            MutexLocker locker(m_Mutex);
            if (!DequeueTask(&task))
            {
                // TODO(phongchen): Add some random factor
                --m_NumBusyThreads;
                bool wait_success = m_NewTaskCond.Wait(m_Mutex, m_IdleTime);
                ++m_NumBusyThreads;
                if (wait_success)
                {
                    if (!DequeueTask(&task))
                        continue;
                }
                else
                {
                    if (m_PendingTasks.empty() && m_NumThreads > m_MinThreads)
                        break;
                    continue;
                }
            }
        }
        task.Run();
    }
}

void ThreadPool::WorkRoutine(PooledThread* thread)
{
    {
        MutexLocker locker(m_Mutex);
        ++m_NumBusyThreads;
    }

    WorkerLoop();

    MutexLocker locker(m_Mutex);
    m_Threads.erase(*thread);
    m_FreedThreads.push_back(*thread);
    --m_NumThreads;
    --m_NumBusyThreads;
    if (m_NumThreads == 0)
        m_AllWorkersExitedCond.Signal();
}

void ThreadPool::Terminate(bool wait)
{
    m_Terminating = true;

    if (!m_Exit)
    {
        if (wait)
        {
            while (m_NumPendingTasks > 0)
            {
                ThisThread::Sleep(1);
            }
        }

        m_Exit = true;
    }

    // wakeup all worker threads to exit
    MutexLocker locker(m_Mutex);
    while (m_NumThreads > 0)
    {
        for (size_t i = 0; i < m_NumThreads; ++i)
            m_NewTaskCond.Broadcast();
        m_AllWorkersExitedCond.Wait(&m_Mutex);
    }

    // all the workers are exited, we are safe to clear all
    while (!m_FreeTasks.empty())
    {
        TaskNode* task = &m_FreeTasks.front();
        m_FreeTasks.pop_front();
        delete task;
    }

    while (!m_PendingTasks.empty())
    {
        TaskNode* task = &m_PendingTasks.front();
        m_PendingTasks.pop_front();
        delete task->callback;
        delete task;
    }

    while (!m_FreedThreads.empty())
    {
        PooledThread* thread = &m_FreedThreads.front();
        m_FreedThreads.pop_front();
        thread->Join();
        delete thread;
    }
}

void ThreadPool::GetStats(Stats* stat) const
{
    assert(!m_Exit);

    MutexLocker locker(m_Mutex);
    stat->NumThreads = m_NumThreads;
    stat->NumBusyThreads = m_NumBusyThreads;
    stat->NumPendingTasks = m_NumPendingTasks;
}

void ThreadPool::WaitForIdle()
{
    assert(!m_Exit);

    // this function is rarely used, using condition variable in WorkRoutine
    // to notify here is not economically
    for (;;)
    {
        {
            MutexLocker locker(m_Mutex);
            if (m_PendingTasks.empty() && m_NumBusyThreads == 0)
                return;
        }

        // sleep must be out of lock
        ThisThread::Sleep(1);
    }
}

ThreadPool* ThreadPool::DefaultInstance()
{
    return Singleton<ThreadPool>::Instance("default");
}

} // end of namespace common
