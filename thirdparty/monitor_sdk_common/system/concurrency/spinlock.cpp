//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 15:07
//  @file:      spinlock.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/spinlock.h"
#include "thirdparty/monitor_sdk_common/system/check_error.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/this_thread.h"

namespace common {

Spinlock::Spinlock()
{
    CHECK_PTHREAD_ERROR(pthread_spin_init(&m_lock, 0));
    m_owner = 0;
}

Spinlock::~Spinlock()
{
    CHECK_PTHREAD_ERROR(pthread_spin_destroy(&m_lock));
    m_owner = -1;
}

void Spinlock::Lock()
{
    CHECK_PTHREAD_ERROR(pthread_spin_lock(&m_lock));
    m_owner = ThisThread::GetId();
}

bool Spinlock::TryLock()
{
    if (CHECK_PTHREAD_TRYLOCK_ERROR(pthread_spin_trylock(&m_lock)))
    {
        m_owner = ThisThread::GetId();
        return true;
    }
    return false;
}

void Spinlock::Unlock()
{
    m_owner = 0;
    CHECK_PTHREAD_ERROR(pthread_spin_unlock(&m_lock));
}

} // end of namespace common

