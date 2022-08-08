//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 16:00
//  @file:      thread_group_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/thread_group.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"
#include "thirdparty/gtest/gtest.h"

namespace common {

class ThreadGroupTest : public testing::Test
{
public:
    static const int  kCount = 100000;
public:
    ThreadGroupTest() : n(0)
    {
    }
    void TestThread()
    {
        for (;;)
        {
            Mutex::Locker locker(mutex);
            if (++n >= kCount)
                return;
        }
    }
protected:
    int n;
    Mutex mutex;
};

const int ThreadGroupTest::kCount;

TEST_F(ThreadGroupTest, Test)
{
    ThreadGroup thread_group(Bind(&ThreadGroupTest::TestThread, this), 4);
    thread_group.Start();
    thread_group.Join();
    EXPECT_GE(n, kCount);
    EXPECT_EQ(4U, thread_group.Size());
}

TEST_F(ThreadGroupTest, Add)
{
    ThreadGroup thread_group;
    thread_group.Add(Bind(&ThreadGroupTest::TestThread, this), 3);
    EXPECT_EQ(3U, thread_group.Size());
    thread_group.Add(Bind(&ThreadGroupTest::TestThread, this));
    EXPECT_EQ(4U, thread_group.Size());
    thread_group.Start();
    thread_group.Join();
    EXPECT_GE(n, kCount);
    EXPECT_EQ(4U, thread_group.Size());
}

} // end of namespace common
