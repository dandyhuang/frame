//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 18:00
//  @file:      scoped_locker.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_SCOPED_LOCKER_H
#define COMMON_SYSTEM_CONCURRENCY_SCOPED_LOCKER_H

/// scoped locker, is just like scoped pointer

#include "thirdparty/monitor_sdk_common/base/uncopyable.h"

// GLOBAL_NOLINT(runtime/references)

namespace common {

template <typename LockType>
class ScopedLocker : private Uncopyable
{
public:
    explicit ScopedLocker(LockType& lock)
        : m_lock(&lock)
    {
        m_lock->Lock();
    }

    explicit ScopedLocker(LockType* lock)
        : m_lock(lock)
    {
        m_lock->Lock();
    }
    ~ScopedLocker()
    {
        m_lock->Unlock();
    }
private:
    LockType* m_lock;
};

template <typename LockType>
class ScopedTryLocker : private Uncopyable
{
public:
    explicit ScopedTryLocker(LockType& lock)
        : m_lock(&lock)
    {
        m_locked = m_lock->TryLock();
    }

    explicit ScopedTryLocker(LockType* lock)
        : m_lock(lock)
    {
        m_locked = m_lock->TryLock();
    }
    ~ScopedTryLocker()
    {
        if (m_locked)
            m_lock->Unlock();
    }
    bool IsLocked() const
    {
        return m_locked;
    }
private:
    LockType* m_lock;
    bool m_locked;
};

template <typename LockType>
class ScopedReaderLocker : private Uncopyable
{
public:
    explicit ScopedReaderLocker(LockType& lock)
        : m_lock(&lock)
    {
        m_lock->ReaderLock();
    }

    explicit ScopedReaderLocker(LockType* lock)
        : m_lock(lock)
    {
        m_lock->ReaderLock();
    }
    ~ScopedReaderLocker()
    {
        m_lock->ReaderUnlock();
    }
private:
    LockType* m_lock;
};

template <typename LockType>
class ScopedTryReaderLocker : private Uncopyable
{
public:
    explicit ScopedTryReaderLocker(LockType& lock)
        : m_lock(&lock)
    {
        m_locked = m_lock->TryReaderLock();
    }

    explicit ScopedTryReaderLocker(LockType* lock)
        : m_lock(lock)
    {
        m_locked = m_lock->TryReaderLock();
    }
    ~ScopedTryReaderLocker()
    {
        if (m_locked)
            m_lock->ReaderUnlock();
    }
    bool IsLocked() const
    {
        return m_locked;
    }
private:
    LockType* m_lock;
    bool m_locked;
};

template <typename LockType>
class ScopedWriterLocker : private Uncopyable
{
public:
    explicit ScopedWriterLocker(LockType& lock) : m_lock(lock)
    {
        m_lock.WriterLock();
    }
    explicit ScopedWriterLocker(LockType* lock) : m_lock(*lock)
    {
        m_lock.WriterLock();
    }
    ~ScopedWriterLocker()
    {
        m_lock.WriterUnlock();
    }
private:
    LockType& m_lock;
};

template <typename LockType>
class ScopedTryWriterLocker : private Uncopyable
{
public:
    explicit ScopedTryWriterLocker(LockType& lock)
        : m_lock(&lock)
    {
        m_locked = m_lock->TryWriterLock();
    }

    explicit ScopedTryWriterLocker(LockType* lock)
        : m_lock(lock)
    {
        m_locked = m_lock->TryWriterLock();
    }
    ~ScopedTryWriterLocker()
    {
        if (m_locked)
            m_lock->WriterUnlock();
    }
    bool IsLocked() const
    {
        return m_locked;
    }
private:
    LockType* m_lock;
    bool m_locked;
};

} // end of namespace common

#endif // COMMON_SYSTEM_CONCURRENCY_SCOPED_LOCKER_H
