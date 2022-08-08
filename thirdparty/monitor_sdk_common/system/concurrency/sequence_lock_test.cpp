//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 15:20
//  @file:      sequence_lock_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/sequence_lock.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(SequenceLock, ReadWithoutRace)
{
    SequenceLock lock;
    int seq = lock.BeginRead();
    ASSERT_TRUE(lock.EndRead(seq));
}

TEST(SequenceLock, ReadDuringWrite)
{
    SequenceLock lock;
    int seq = lock.BeginRead();
    lock.WriterLock();
    ASSERT_FALSE(lock.EndRead(seq));
    lock.WriterUnlock();
    ASSERT_FALSE(lock.EndRead(seq));
}

TEST(SequenceLock, ReadAfterWrite)
{
    SequenceLock lock;
    int seq = lock.BeginRead();
    lock.WriterLock();
    lock.WriterUnlock();
    ASSERT_FALSE(lock.EndRead(seq));
}

} // end of namespace common
