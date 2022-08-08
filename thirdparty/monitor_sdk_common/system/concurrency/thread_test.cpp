//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 18:20
//  @file:      thread_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/thread.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/this_thread.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

static void ThreadCallback(int* p)
{
    ++*p;
}

TEST(Thread, Test)
{
    int n = 0;
    Thread thread(Bind(ThreadCallback, &n));
    thread.Start();
    thread.Join();
    EXPECT_EQ(1, n);
}

static void DoNothing()
{
}

TEST(Thread, Restart)
{
    Thread thread(Bind(DoNothing));
    for (int i = 0; i < 10; ++i)
    {
        thread.Start();
        int tid1 = thread.GetId();
        thread.Join();

        thread.Start();
        int tid2 = thread.GetId();
        EXPECT_NE(tid1, tid2);
        thread.Join();
    }
}

TEST(Thread, Reinitialize)
{
    Thread thread(Bind(DoNothing));
    thread.Start();
    thread.Join();
    thread.Start();
    thread.Join();

    thread.Initialize(NewPermanentClosure(DoNothing));
    thread.Start();
    thread.Join();
    thread.Start();
    thread.Join();
}

TEST(Thread, RestartDeathTest)
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";
    Thread thread;
    thread.Initialize(NewClosure(DoNothing));
    thread.Start();
    thread.Join();
    EXPECT_DEATH(thread.Start(), "Invalid argument");
}

TEST(Thread, DuplicatedStartDeathTest)
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";
    {
        Thread thread(Bind(DoNothing));
        thread.Start();
        EXPECT_DEATH(thread.Start(), "Invalid argument");
    }
}

static void IsAliveTestThread(volatile const bool* stop)
{
    while (!*stop)
        ThisThread::Sleep(1);
    // ThisThread::Exit();
}

TEST(Thread, IsAlive)
{
    bool stop = false;
    Thread thread(Bind(IsAliveTestThread, &stop));
    thread.Start();
    for (int i = 0; i < 1000; ++i)
    {
        if (!thread.IsAlive())
            ThisThread::Sleep(1);
    }
    stop = true;
    thread.Join();
    EXPECT_FALSE(thread.IsAlive());
}

TEST(Thread, Detach)
{
    for (int i = 0; i < 100; ++i)
    {
        Thread thread(Bind(ThisThread::Sleep, 1));
        thread.Start();
        thread.Detach();
        ThisThread::Sleep(1);
    }
}

} // end of namespace common
