//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 17:42
//  @file:      mutex.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"

#ifdef _WIN32

#include "thirdparty/monitor_sdk_common/base/common_windows.h"
#include "thirdparty/monitor_sdk_common/base/static_assert.h"

COMMON_STATIC_ASSERT(sizeof(MutexBase) == sizeof(CRITICAL_SECTION));

// if _WIN32_WINNT not defined, TryEnterCriticalSection will not be declared
// in windows.h
extern "C" WINBASEAPI
BOOL WINAPI TryEnterCriticalSection(__inout LPCRITICAL_SECTION lpCriticalSection);


namespace common {

namespace
{

/// cast MutexBase to CRITICAL_SECTION
CRITICAL_SECTION* MutexBaseAsCs(MutexBase* mutex)
{
    return reinterpret_cast<CRITICAL_SECTION*>(mutex);
}

/// cast MutexBase to CRITICAL_SECTION
const CRITICAL_SECTION* MutexBaseAsCs(const MutexBase* mutex)
{
    return reinterpret_cast<const CRITICAL_SECTION*>(mutex);
}

// In Microsoft Windows Server 2003 Service Pack 1 and later versions of
// Windows, the LockCount field is parsed as follows:
// * The lowest bit shows the lock status. If this bit is 0, the critical
//   section is locked; if it is 1, the critical section is not locked.
// * The next bit shows whether a thread has been woken for this lock.
//   If this bit is 0, then a thread has been woken for this lock; if it
//   is 1, no thread has been woken.
// * The remaining bits are the ones-complement of the number of threads
//   waiting for the lock.
bool DoIsNewBehavior()
{
    CRITICAL_SECTION mutex;
    ::InitializeCriticalSection(&mutex);
    int old_lock_count = mutex.LockCount;
    ::EnterCriticalSection(&mutex);
    int new_lock_count = mutex.LockCount;
    ::LeaveCriticalSection(&mutex);
    ::DeleteCriticalSection(&mutex);
    return new_lock_count < old_lock_count;
}

bool IsNewBehavior()
{
    static bool result = DoIsNewBehavior();
    return result;
}

}

MutexBase::MutexBase()
{
    ::InitializeCriticalSection(MutexBaseAsCs(this));
}

MutexBase::~MutexBase()
{
    ::DeleteCriticalSection(MutexBaseAsCs(this));
}

void MutexBase::Lock()
{
    ::EnterCriticalSection(MutexBaseAsCs(this));
    assert(IsLocked());
}

bool MutexBase::TryLock()
{
    return ::TryEnterCriticalSection(MutexBaseAsCs(this)) != FALSE;
}

void MutexBase::Unlock()
{
    assert(IsLocked());
    ::LeaveCriticalSection(MutexBaseAsCs(this));
}

bool MutexBase::IsLocked() const
{
    if (IsNewBehavior()) // after win2k3 sp1
        return (MutexBaseAsCs(this)->LockCount & 1) == 0;
    else
        return MutexBaseAsCs(this)->LockCount >= 0;
}

} // end of namespace common

#endif

