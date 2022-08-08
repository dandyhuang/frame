//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-08-13 17:19
//  @file:      event_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#undef __DEPRECATED
#include "thirdparty/monitor_sdk_common/system/concurrency/event.h"
#define __DEPRECATED 1
#include <iostream>
#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/blocking_queue.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/thread.h"
#include "thirdparty/monitor_sdk_common/system/time/timestamp.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"


namespace common {
    
void SetThread(SyncEvent* sync_event, volatile bool* done)
{
    for (int i = 0; i < 500000; ++i)
    {
        sync_event->Set();
    }
    *done = true;
    sync_event->Set();
}
    
TEST(SyncEvent, Test)
{
    volatile bool done = false;
    SyncEvent sync_event;
    Thread thread(Bind(SetThread, &sync_event, &done));
    thread.Start();
    while (!done)
    {
        uint64_t t0 = GetTimeStampInMs();
        sync_event.Wait();
        uint64_t t1 = GetTimeStampInMs();
        if (t1 - t0 > 100)
        {
            std::cout << "Wait Time: " << t1 - t0 << " ms" << std::endl;
        }
    }
}
    
const int kLoopCount = 100000;
    
static void ProduceThread(SyncEvent* produce_event, SyncEvent* consume_event, int* n)
{
    for (int i = 0; i < kLoopCount; ++i)
    {
        produce_event->Set();
        consume_event->Wait();
    }
}
    
static void TestPerformance(int spin_count)
{
    SyncEvent produce_event(false, false, spin_count);
    SyncEvent consume_event(false, false, spin_count);
    int n = 0;
    Thread produce_thread(NewClosure(ProduceThread, &produce_event, &consume_event, &n));
    produce_thread.Start();
    for (int i = 0; i < kLoopCount; ++i)
    {
        produce_event.Wait();
        consume_event.Set();
    }
    produce_thread.Join();
}
    
TEST(SyncEvent, Performance)
{
    TestPerformance(0);
}
    
TEST(SyncEvent, SpinPerformance)
{
    TestPerformance(4000);
}
    
TEST(AutoResetEvent, SetAndWait)
{
    AutoResetEvent event;
    EXPECT_FALSE(event.TryWait());
    event.Set();
    EXPECT_TRUE(event.TryWait());
    EXPECT_FALSE(event.TryWait());
}
    
TEST(AutoResetEvent, TimedWait)
{
    AutoResetEvent event;
    EXPECT_FALSE(event.Wait(1));
    event.Set();
    EXPECT_TRUE(event.Wait(1));
    EXPECT_FALSE(event.TryWait());
}
    
    
TEST(AutoResetEvent, InitValue)
{
    AutoResetEvent event(true);
    EXPECT_TRUE(event.TryWait());
    EXPECT_FALSE(event.TryWait());
    event.Set();
    EXPECT_TRUE(event.TryWait());
}
    
TEST(AutoResetEvent, Set)
{
    AutoResetEvent event(false);
    event.Set();
    EXPECT_TRUE(event.TryWait());
}
    
TEST(AutoResetEvent, Reset)
{
    AutoResetEvent event(true);
    event.Reset();
    EXPECT_FALSE(event.TryWait());
}
    
static void WakeThread(BlockingQueue<AutoResetEvent*>* queue)
{
    for (;;)
    {
        AutoResetEvent* e;
        queue->PopFront(&e);
        if (!e)
            break;
        e->Set();
    }
}
    
TEST(AutoResetEvent, DestroyAfterSet)
{
    BlockingQueue<AutoResetEvent*> queue;
    Thread thread(Bind(WakeThread, &queue));
    thread.Start();
    
    for (int i = 0; i < 1000; ++i)
    {
        if (i % 64 == 0)
            VLOG(3) << "Wait Time: " << i;
        AutoResetEvent* e;
        {
            AutoResetEvent event;
            e = &event;
            queue.PushBack(&event);
            event.Wait();
        }
        memset(e, 0xFF, sizeof(*e));
        ThisThread::Sleep(1);
    }
    
    queue.PushBack(NULL);
    thread.Join();
}
    
TEST(ManualResetEvent, SetAndWait)
{
    ManualResetEvent event;
    EXPECT_FALSE(event.TryWait());
    event.Set();
    EXPECT_TRUE(event.TryWait());
    EXPECT_TRUE(event.TryWait());
    event.Reset();
    EXPECT_FALSE(event.TryWait());
}
    
TEST(ManualResetEvent, InitValue)
{
    ManualResetEvent event(true);
    EXPECT_TRUE(event.TryWait());
    EXPECT_TRUE(event.TryWait());
    event.Set();
    EXPECT_TRUE(event.TryWait());
    EXPECT_TRUE(event.TryWait());
}
    
TEST(ManualResetEvent, TimedWait)
{
    ManualResetEvent event(false);
    event.Set();
    EXPECT_TRUE(event.Wait(1));
    event.Reset();
    EXPECT_FALSE(event.Wait(1));
}
    
    
TEST(ManualResetEvent, Set)
{
    ManualResetEvent event(false);
    event.Set();
    event.Wait();
    EXPECT_TRUE(event.TryWait());
}
    
TEST(ManualResetEvent, Reset)
{
    ManualResetEvent event(true);
    event.Reset();
    EXPECT_FALSE(event.TryWait());
}
    
} // namespace common


