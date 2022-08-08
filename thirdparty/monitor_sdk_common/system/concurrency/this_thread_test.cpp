//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 18:06
//  @file:      this_thread_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/this_thread.h"
#include "thirdparty/monitor_sdk_common/base/annotation.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/thread.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

static void IsMainThreadTestThread(bool* b)
{
    *b = ThisThread::IsMain();
}

TEST(ThisThread, GetId)
{
    EXPECT_GT(ThisThread::GetId(), 0);
}

TEST(ThisThread, GetIdBench)
{
    int total = 0;
    for (int i = 0; i < 100000000; ++i)
        total += ThisThread::GetId();
    volatile int other_total = total;
    IgnoreUnused(other_total);
}

TEST(ThisThread, GetHandle)
{
#if defined _WIN32 || defined __linux__
    ThreadHandleType null_id = 0;
    EXPECT_NE(null_id, ThisThread::GetHandle());
#endif
}

TEST(ThisThread, Exit)
{
    // EXPECT_EXIT(ThisThread::Exit(), ::testing::ExitedWithCode(0), "");
}

TEST(ThisThread, IsMainThread)
{
    EXPECT_TRUE(ThisThread::IsMain());
    bool is_main = true;
    Thread thread(Bind(IsMainThreadTestThread, &is_main));
    EXPECT_TRUE(thread.Start());
    thread.Join();
    EXPECT_FALSE(is_main);
}

TEST(ThisThread, Yield)
{
    ThisThread::Yield();
}

} // end of namespace common
