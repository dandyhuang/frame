//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 17:52
//  @file:      rwlock_base.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_RWLOCK_BASE_H
#define COMMON_SYSTEM_CONCURRENCY_RWLOCK_BASE_H
#pragma once

#include <assert.h>

#include "thirdparty/monitor_sdk_common/base/uncopyable.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/atomic.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/scoped_locker.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/this_thread.h"
#include "thirdparty/monitor_sdk_common/system/memory/barrier.h"

namespace common {

/**
 * A lightweight rwlock implementation.
 *
 * This class is designed to be used in large scale data nodes such as XFS dir
 * entries. In this case, normal RWLock will consume too many memory.
 *
 * In any other case, you should use normal RWLock, which is a based on
 * pthread_rwlock, may be better balance.
 */
template <typename CocreteType>
class RwLockBase
{
    COMMON_DECLARE_UNCOPYABLE(RwLockBase);

public:
    typedef ScopedReaderLocker<CocreteType> ReaderLocker;
    typedef ScopedWriterLocker<CocreteType> WriterLocker;
    typedef ScopedTryReaderLocker<CocreteType> TryReaderLocker;
    typedef ScopedTryWriterLocker<CocreteType> TryWriterLocker;

public:
    RwLockBase() : m_lock(0), m_last_owner_id(0)
    {
    }

    ~RwLockBase()
    {
        assert(m_lock == 0);
        m_lock = 0;
        m_last_owner_id = -1;
    }

    bool TryReaderLock()
    {
        int value = m_lock;
        while (value >= 0)
        {
            if (AtomicCompareExchange(&m_lock, value, value + 1, &value))
                return true;
        }

        // Lock writed, it's not necessary to try again
        return false;
    }

    void ReaderLock()
    {
        while (!TryReaderLock())
            CocreteType::Relax();
        m_last_owner_id = ThisThread::GetId();
    }

    void ReaderUnlock()
    {
        m_last_owner_id = 0;
        int n = AtomicExchangeSub(&m_lock, 1);
        assert(n > 0);
        (void) n;
    }

    bool TryWriterLock()
    {
        return AtomicCompareExchange(&m_lock, 0, -1);
    }

    void WriterLock()
    {
        while (!TryWriterLock())
            CocreteType::Relax();
        m_last_owner_id = ThisThread::GetId();
    }

    void WriterUnlock()
    {
        m_last_owner_id = 0;
        int old_value = AtomicExchange(&m_lock, 0);
        assert(old_value == -1);
        (void) old_value;
    }

public: // only for test and debug, should not be used in normal code logical
    bool IsReaderLocked() const
    {
        MemoryReadBarrier();
        return m_lock > 0;
    }

    bool IsWriterLocked() const
    {
        MemoryReadBarrier();
        return m_lock < 0;
    }

    bool IsLocked() const
    {
        MemoryReadBarrier();
        return m_lock != 0;
    }

private:
    int m_lock;
    int m_last_owner_id;
};

} // end of namespace common

#endif // COMMON_SYSTEM_CONCURRENCY_RWLOCK_BASE_H
