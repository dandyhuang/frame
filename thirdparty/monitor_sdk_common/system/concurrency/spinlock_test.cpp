//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 18:03
//  @file:      spinlock_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************

#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/spinlock.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/thread_group.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

const int test_count = 10000000;

TEST(MutexTest, Mutex)
{
    SimpleMutex lock;
    for (int i = 0; i < test_count; ++i)
    {
        Mutex::Locker locker(lock);
    }
}

void TestThread(int* p, SimpleMutex* mutex)
{
    for (;;)
    {
        Mutex::Locker locker(mutex);
        if (++(*p) >= test_count)
            return;
    }
}

TEST(MutexTest, ThreadMutex)
{
    int n = 0;
    SimpleMutex lock;
    ThreadGroup thread_group(Bind(TestThread, &n, &lock), 4);
    thread_group.Start();
    thread_group.Join();
}

TEST(SpinlockTest, Spinlock)
{
    Spinlock lock;
    for (int i = 0; i < test_count; ++i)
    {
        Spinlock::Locker locker(lock);
    }
}

} // end of namespace common
