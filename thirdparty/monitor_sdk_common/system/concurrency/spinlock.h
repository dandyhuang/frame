//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 18:02
//  @file:      spinlock.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_SPINLOCK_H
#define COMMON_SYSTEM_CONCURRENCY_SPINLOCK_H

#include <errno.h>
#include <stdlib.h>

#ifdef _WIN32

#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"

namespace common {

typedef ::common::Mutex Spinlock;

} // end of namespace common
#else

#include <pthread.h>
#include "thirdparty/monitor_sdk_common/system/concurrency/scoped_locker.h"

namespace common {

/// spinlock is faster than mutex at some condition, but
class Spinlock
{
public:
    typedef ScopedLocker<Spinlock> Locker;
public:
    Spinlock();
    ~Spinlock();
    void Lock();
    bool TryLock();
    void Unlock();
private:
    Spinlock(const Spinlock&);
    Spinlock& operator=(const Spinlock&);
private:
    pthread_spinlock_t m_lock;
    pid_t m_owner;
};

#endif // _WIN32

// compatible reason
typedef Spinlock::Locker SpinlockLocker;

} // end of namespace common

#endif // COMMON_SYSTEM_CONCURRENCY_SPINLOCK_H

