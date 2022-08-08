//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-08-13 17:18
//  @file:      event.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/event.h"
#include <stdlib.h>
#include "thirdparty/monitor_sdk_common/system/check_error.h"

#ifdef _WIN32
#include "thirdparty/monitor_sdk_common/base/common_windows.h"
#else
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/atomic.h"
#include "thirdparty/monitor_sdk_common/system/system_information.h"
#endif


namespace common {

#ifdef _WIN32

SyncEvent::SyncEvent(bool manual_reset, bool init_state, int spin_count)
{
    m_hEvent = CreateEvent(NULL, manual_reset, init_state, NULL);
    CHECK_WINDOWS_ERROR(m_hEvent != NULL);
}
    
SyncEvent::~SyncEvent()
{
    CHECK_WINDOWS_ERROR(CloseHandle(m_hEvent));
    m_hEvent = NULL;
}
    
void SyncEvent::Wait()
{
    CHECK_WINDOWS_WAIT_ERROR(WaitForSingleObject(m_hEvent, INFINITE));
}
    
// Timeout in milliseconds.
bool SyncEvent::Wait(int64_t timeout)
{
    return CHECK_WINDOWS_WAIT_ERROR(WaitForSingleObject(m_hEvent, timeout));
}
    
bool SyncEvent::TryWait()
{
    return Wait(0);
}
    
void SyncEvent::Set()
{
    CHECK_WINDOWS_ERROR(SetEvent(m_hEvent));
}
    
void SyncEvent::Reset()
{
    CHECK_WINDOWS_ERROR(ResetEvent(m_hEvent));
}
    
/////////////////////////////////////////////////////////////////////////////
// AutoResetEvent staff
    
AutoResetEvent::AutoResetEvent(bool init_state)
{
    m_hEvent = CreateEvent(NULL, FALSE, init_state, NULL);
    CHECK_WINDOWS_ERROR(m_hEvent != NULL);
}
    
AutoResetEvent::~AutoResetEvent()
{
    CHECK_WINDOWS_ERROR(CloseHandle(m_hEvent));
    m_hEvent = NULL;
}
    
void AutoResetEvent::Wait()
{
    CHECK_WINDOWS_WAIT_ERROR(WaitForSingleObject(m_hEvent, INFINITE));
}
    
// Timeout in milliseconds.
bool AutoResetEvent::Wait(int64_t timeout)
{
    return CHECK_WINDOWS_WAIT_ERROR(WaitForSingleObject(m_hEvent, timeout));
}
    
bool AutoResetEvent::TryWait()
{
    return Wait(0);
}
    
void AutoResetEvent::Set()
{
    CHECK_WINDOWS_ERROR(SetEvent(m_hEvent));
}
    
void AutoResetEvent::Reset()
{
    CHECK_WINDOWS_ERROR(ResetEvent(m_hEvent));
}
    
/////////////////////////////////////////////////////////////////////////////
// ManualResetEvent staff
    
ManualResetEvent::ManualResetEvent(bool init_state)
{
    m_hEvent = CreateEvent(NULL, TRUE, init_state, NULL);
    CHECK_WINDOWS_ERROR(m_hEvent != NULL);
}
    
ManualResetEvent::~ManualResetEvent()
{
    CHECK_WINDOWS_ERROR(CloseHandle(m_hEvent));
    m_hEvent = NULL;
}
    
void ManualResetEvent::Wait()
{
    CHECK_WINDOWS_WAIT_ERROR(WaitForSingleObject(m_hEvent, INFINITE));
}
    
// Timeout in milliseconds.
bool ManualResetEvent::Wait(int64_t timeout)
{
    return CHECK_WINDOWS_WAIT_ERROR(WaitForSingleObject(m_hEvent, timeout));
}
    
bool ManualResetEvent::TryWait()
{
    return Wait(0);
}
    
void ManualResetEvent::Set()
{
    CHECK_WINDOWS_ERROR(SetEvent(m_hEvent));
}
    
void ManualResetEvent::Reset()
{
    CHECK_WINDOWS_ERROR(ResetEvent(m_hEvent));
}
    
#else // _WIN32
    
SyncEvent::SyncEvent(bool manual_reset, bool init_state, int spin_count):
    m_manual_reset(manual_reset),
    m_spin_count(spin_count),
    m_signaled(init_state)
{
}
    
SyncEvent::~SyncEvent()
{
}
    
bool SyncEvent::SpinWait()
{
    static bool is_smp = GetLogicalCpuNumber() > 1;
    if (!is_smp)
        return false;
    
    for (int i = 0; i < m_spin_count; ++i)
    {
        if (m_manual_reset)
        {
            if (AtomicGet(&m_signaled))
            {
                return true;
            }
        }
        else
        {
            if (AtomicCompareExchange(&m_signaled, true, false))
            {
                return true;
            }
        }
    }
    
    return false;
}
    
void SyncEvent::Wait()
{
    if (SpinWait())
        return;
    
    MutexLocker locker(m_mutex);
    
    while (!m_signaled)
        m_cond.Wait(m_mutex);
    
    if (!m_manual_reset)
        m_signaled = false;
}
    
bool SyncEvent::Wait(int64_t timeout)
{
    if (SpinWait())
        return true;
    
    MutexLocker locker(m_mutex);
    
    if (!m_signaled)
        m_cond.Wait(m_mutex, timeout);
    
    if (!m_signaled) return false;
    
    if (!m_manual_reset)
        m_signaled = false;
    
    return true;
}
    
bool SyncEvent::TryWait()
{
    return Wait(0);
}
    
void SyncEvent::Set()
{
    MutexLocker locker(m_mutex);
    m_signaled = true;
    if (m_manual_reset)
        m_cond.Broadcast();
    else
        m_cond.Signal();
}
    
void SyncEvent::Reset()
{
    MutexLocker locker(m_mutex);
    m_signaled = false;
}
    
//////////////////////////////////////////////////////////////////////////////
// AutoResetEvent staff
    
AutoResetEvent::AutoResetEvent(bool init_state):
    m_signaled(init_state)
{
}
    
AutoResetEvent::~AutoResetEvent()
{
}
    
void AutoResetEvent::Wait()
{
    MutexLocker locker(m_mutex);
    while (!m_signaled)
        m_cond.Wait(m_mutex);
    m_signaled = false;
}
    
bool AutoResetEvent::Wait(int64_t timeout)
{
    MutexLocker locker(m_mutex);
    if (!m_signaled)
        m_cond.Wait(m_mutex, timeout);
    
    if (!m_signaled)
        return false;
    
    m_signaled = false;
    return true;
}
    
bool AutoResetEvent::TryWait()
{
    return Wait(0);
}
    
void AutoResetEvent::Set()
{
    MutexLocker locker(m_mutex);
    m_signaled = true;
    m_cond.Signal();
}
    
void AutoResetEvent::Reset()
{
    MutexLocker locker(m_mutex);
    m_signaled = false;
}
    
//////////////////////////////////////////////////////////////////////////////
// ManualResetEvent staff
    
ManualResetEvent::ManualResetEvent(bool init_state):
    m_signaled(init_state)
{
}
    
ManualResetEvent::~ManualResetEvent()
{
}
    
void ManualResetEvent::Wait()
{
    MutexLocker locker(m_mutex);
    while (!m_signaled)
        m_cond.Wait(m_mutex);
}
    
bool ManualResetEvent::Wait(int64_t timeout)
{
    MutexLocker locker(m_mutex);
    if (!m_signaled)
        m_cond.Wait(m_mutex, timeout);
    return m_signaled;
}
    
bool ManualResetEvent::TryWait()
{
    return Wait(0);
}
    
void ManualResetEvent::Set()
{
    MutexLocker locker(m_mutex);
    m_signaled = true;
    m_cond.Broadcast();
}
    
void ManualResetEvent::Reset()
{
    MutexLocker locker(m_mutex);
    m_signaled = false;
}
    
#endif // _WIN32
    
} // namespace common



