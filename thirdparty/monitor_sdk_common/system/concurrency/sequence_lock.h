//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 15:18
//  @file:      sequence_lock.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_SEQUENCE_LOCK_H
#define COMMON_SYSTEM_CONCURRENCY_SEQUENCE_LOCK_H
#pragma once

#include "thirdparty/monitor_sdk_common/system/concurrency/scoped_locker.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/spinlock.h"
#include "thirdparty/monitor_sdk_common/system/cpu/cpu_relax.h"
#include "thirdparty/monitor_sdk_common/system/memory/barrier.h"

namespace common {

// Reader/writer consistent mechanism without starving writers. This type of
// lock for data where the reader wants a consistent set of information and
// is willing to retry if the information changes.  Readers never block but
// they may have to retry if a writer is in progress. Writers do not wait for
// readers.
//
// This will not work for data that contains pointers, because any writer could
// invalidate a pointer that a reader was following.
//
// Expected reader usage:
//      do {
//          seq = lock.BeginRead();
//          // Read shared data...
//
//      } while (!lock.EndRead(seq)); // Retry if writing occurred
//
// Expected writer usage:
//      {
//          // Multiple writer should be exclusion by the locker
//          SequenceLock::WriterLocker locker(lock);
//          // Safe to write the shared data
//      }
//
// See http://lxr.linux.no/#linux+v3.3.2/include/linux/seqlock.h for details.
//
class SequenceLock
{
public:
    typedef ScopedLocker<SequenceLock> WriterLocker;
    typedef ScopedTryWriterLocker<SequenceLock> TryWriterLocker;

public:
    SequenceLock();
    ~SequenceLock();

    // A writer should acquire this lock before writing
    inline void WriterLock();

    // Try acquire the writer lock.
    // Return true if success
    inline bool TryWriterLock();

    // Ralease the lock after writing finished.
    inline void WriterUnlock();

    // Call this befor reading, and save the returned sequence number.
    inline int BeginRead() const;

    // After reading finished, call this with previous sequence number.
    // Return true if this reading is success, otherwise, means somen writing
    // operation occurred durring reading. Typecally, you should retry.
    inline bool EndRead(int start_seq) const;

private:
    // Spinlock is better than mutex here, because mutex may sleep, and the
    // critical section protected by SequenceLock should be short usually.
    Spinlock m_lock;

    // NOTE: Can't be signed integer here. According to C/C++ std, if signed
    // integer overflow, the hevavior is undefined.
    unsigned int m_sequence;
};

void SequenceLock::WriterLock()
{
    m_lock.Lock();
    ++m_sequence;
    MemoryWriteBarrier();
}

bool SequenceLock::TryWriterLock()
{
    if (m_lock.TryLock())
    {
        ++m_sequence;
        MemoryWriteBarrier();
        return true;
    }
    return false;
}

void SequenceLock::WriterUnlock()
{
    MemoryWriteBarrier();
    ++m_sequence;
    m_lock.Unlock();
}

int SequenceLock::BeginRead() const
{
    unsigned int ret;
repeat:
    // The volatile guaranteeing read the actual value each in each loop.
    ret = const_cast<volatile unsigned int&>(m_sequence);
    if (ret & 1) // ret is odd means writing is processing.
    {
        CpuRelax();
        goto repeat;
    }
    MemoryReadBarrier();

    // Return int to shorten the typing for user
    return static_cast<int>(ret);
}

bool SequenceLock::EndRead(int start_seq) const
{
    MemoryReadBarrier();
    return m_sequence == static_cast<unsigned int>(start_seq);
}

} // end of namespace common

#endif // COMMON_SYSTEM_CONCURRENCY_SEQUENCE_LOCK_H
