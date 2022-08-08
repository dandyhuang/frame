//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-08-13 17:15
//  @file:      event.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_EVENT_H
#define COMMON_SYSTEM_CONCURRENCY_EVENT_H

/// @brief Win32 Event synchronization object
/// @author phongchen

#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/base/uncopyable.h"

#include "thirdparty/monitor_sdk_common/base/deprecate.h"

#ifndef _WIN32
#include "thirdparty/monitor_sdk_common/system/concurrency/condition_variable.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"
#endif


namespace common {

/// Windows Event Object
class DEPRECATED_BY(AutoResetEvent or ManualResetEvent) SyncEvent :
    private Uncopyable
{
public:
    /// @param manual_reset manual_reset or auto reset
    /// @param init_state initial state is signaled or non-signaled
    /// @param spin_count try spin count times to check signaled flag before wait.
    /// if the event may be signaled quiet soon after wait, proper spin count may
    /// reduce the latency and then promote the total performance.
    SyncEvent(
        bool manual_reset = false,
        bool init_state = false,
        int spin_count = 0
    );
    ~SyncEvent();
    void Wait();
    bool Wait(int64_t timeout); // in ms
    bool TryWait();
    void Set();
    void Reset();
private:
    bool SpinWait();
private:
#ifdef _WIN32
    void* m_hEvent;
#else
    Mutex m_mutex;
    ConditionVariable m_cond;
    const bool m_manual_reset;
    const int m_spin_count;
    bool m_signaled;
#endif
};
    
// The AutoResetEvent class represents a local waitable event that resets
// automatically when signaled, after releasing a single waiting thread.
//
// AutoResetEvent allows threads to communicate with each other by signaling.
// Typically, you use this class when threads need exclusive access to a resource.
//
// Important
// There is no guarantee that every call to the Set method will release a thread.
// If two calls are too close together, so that the second call occurs before a
// thread has been released, only one thread is released. It is as if the second
// call did not happen. Also, if Set is called when there are no threads waiting
// and the AutoResetEvent is already signaled, the call has no effect.
//
// If you want to release a thread after each call, Semaphore is a good choice.
class AutoResetEvent
{
    COMMON_DECLARE_UNCOPYABLE(AutoResetEvent);
public:
    /// @param init_state initial state is signaled or non-signaled
    explicit AutoResetEvent(bool init_state = false);
    
    ~AutoResetEvent();
    
    // Calling thread waits for a signal by calling Wait on the AutoResetEvent.
    // If the event is in the non-signaled state, the thread blocks, waiting for
    // the thread that currently controls the resource to signal that the
    // resource is available by calling Set.
    //
    // If a thread calls Wait while the AutoResetEvent is in the signaled state,
    // the thread does not block. The AutoResetEvent releases the thread
    // immediately and returns to the non-signaled state.
    void Wait();
    
    // Wait with timout, in milliseconds.
    // return true if success, false if timeout
    bool Wait(int64_t timeout);
    
    // Try to wait the event.
    // return true immediately if signaled, else return false
    bool TryWait();
    
    // Signals the event to release a waiting thread.
    // The event remains signaled until a single waiting thread is released,
    // and then automatically returns to the non-signaled state. If no threads
    // are waiting, the state remains signaled indefinitely.
    void Set();
    
    // Sets the state of the event to nonsignaled, causing threads to block.
    void Reset();
    
private:
#ifdef _WIN32
    void* m_hEvent;
#else
    Mutex m_mutex;
    ConditionVariable m_cond;
    bool m_signaled;
#endif
};
    
// This class represents a local waitable event object that must be reset
// manually after it is signaled.
//
// ManualResetEvent allows threads to communicate with each other by signaling.
// Typically, this communication concerns a task which one thread must complete
// before other threads can proceed.
//
// The object remains signaled until its Reset method is called. Any number of
// waiting threads, or threads that wait on the event after it has been signaled,
// can be released while the object's state is signaled.
class ManualResetEvent
{
    COMMON_DECLARE_UNCOPYABLE(ManualResetEvent);
public:
    /// @param init_state initial state is signaled or non-signaled
    explicit ManualResetEvent(bool init_state = false);
    ~ManualResetEvent();
    
    // Blocks the current thread until the current WaitHandle receives a signal.
    void Wait();
    
    // Wait with timout, in milliseconds.
    // return true if success, false if timeout
    bool Wait(int64_t timeout);
    
    // Try to wait the event.
    bool TryWait();
    
    // Sets the state of the event to signaled, allowing one or more waiting
    // threads to proceed.
    void Set();
    
    // Sets the state of the event to nonsignaled, causing threads to block.
    void Reset();
    
private:
#ifdef _WIN32
    void* m_hEvent;
#else
    Mutex m_mutex;
    ConditionVariable m_cond;
    bool m_signaled;
#endif
};
    
} // namespace common


#endif // COMMON_SYSTEM_CONCURRENCY_EVENT_H
