//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-08-13 17:24
//  @file:      blocking_queue_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/blocking_queue.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/thread.h"
#include "thirdparty/monitor_sdk_common/system/time/timestamp.h"
#include "thirdparty/gtest/gtest.h"


namespace common {
    
TEST(BlockingQueue, Init)
{
    BlockingQueue<int> queue;
    queue.PushBack(1);
    int n;
    ASSERT_TRUE(queue.TimedPopFront(&n, 0));
    ASSERT_EQ(1, n);
}
    
TEST(BlockingQueue, Full)
{
    // Initialize a queue with capacity 10.
    static const int kCapacity = 10;
    int n;
    BlockingQueue<int> queue(kCapacity);
    std::deque<int> values;
    
    // The queue is empty now.
    ASSERT_TRUE(queue.IsEmpty());
    ASSERT_FALSE(queue.TimedPopBack(&n, 0));
    ASSERT_FALSE(queue.TimedPopAll(&values, 0));
    
    // Push some elmenets.
    for (int i = 0; i < kCapacity; ++i) {
        ASSERT_FALSE(queue.IsFull());
        queue.PushBack(i);
    }
    
    // The queue is full now.
    ASSERT_TRUE(queue.IsFull());
    ASSERT_FALSE(queue.TimedPushBack(0, 0));
    
    // Pop from back.
    ASSERT_TRUE(queue.TimedPopBack(&n, 0));
    ASSERT_EQ(kCapacity - 1, n);
    
    // The queue is not full now.
    ASSERT_FALSE(queue.IsFull());
    queue.PushBack(kCapacity);
    ASSERT_TRUE(queue.TimedPopBack(&n, 0));
    ASSERT_EQ(kCapacity, n);
    
    // The queue is full now.
    queue.PushBack(kCapacity);
    ASSERT_TRUE(queue.IsFull());
    ASSERT_FALSE(queue.TimedPushBack(0, 0));
    
    ASSERT_TRUE(queue.TimedPopAll(&values, 0));
    ASSERT_EQ(static_cast<size_t>(kCapacity), values.size());
    
    // The queue is empty now.
    ASSERT_FALSE(queue.TimedPopBack(&n, 0));
}

TEST(BlockingQueue, TryPushFront)
{
    BlockingQueue<int> queue(1);
    EXPECT_TRUE(queue.TryPushFront(1));
    EXPECT_FALSE(queue.TryPushFront(2));
    int n;
    queue.PopFront(&n);
    EXPECT_EQ(1, n);
    EXPECT_TRUE(queue.TryPushFront(1));
}

TEST(BlockingQueue, TryPushBack)
{
    BlockingQueue<int> queue(1);
    EXPECT_TRUE(queue.TryPushBack(1));
    EXPECT_FALSE(queue.TryPushBack(2));
    int n;
    queue.PopBack(&n);
    EXPECT_EQ(1, n);
    EXPECT_TRUE(queue.TryPushBack(1));
}

TEST(BlockingQueue, TryPopFront)
{
    BlockingQueue<int> queue(1);
    int n;
    EXPECT_FALSE(queue.TryPopFront(&n));
    queue.PushFront(1);
    EXPECT_TRUE(queue.TryPopFront(&n));
    EXPECT_EQ(1, n);
    EXPECT_FALSE(queue.TryPopFront(&n));
}

TEST(BlockingQueue, TryPopBack)
{
    BlockingQueue<int> queue(1);
    int n;
    EXPECT_FALSE(queue.TryPopBack(&n));
    queue.PushFront(1);
    EXPECT_TRUE(queue.TryPopBack(&n));
    EXPECT_EQ(1, n);
    EXPECT_FALSE(queue.TryPopBack(&n));
}

TEST(BlockingQueue, TryPopAll)
{
    BlockingQueue<int> queue(1);
    BlockingQueue<int>::UnderlyContainerType dq;
    EXPECT_FALSE(queue.TryPopAll(&dq));
    queue.PushBack(1);
    ASSERT_TRUE(queue.TryPopAll(&dq));
    EXPECT_EQ(1U, dq.size());
    EXPECT_EQ(1, dq[0]);
    EXPECT_FALSE(queue.TryPopAll(&dq));
}

static void QueueThread(BlockingQueue<int64_t>* queue)
{
    int t = 0;
    for (int i = 0; i < 1000000; ++i)
    {
        queue->PushBack(++t);
    }
    queue->PushBack(-1);
}
    
TEST(BlockingQueue, Performance)
{
    BlockingQueue<int64_t> queue;
    Thread thread(Bind(QueueThread, &queue));
    thread.Start();
    
    for (;;)
    {
        int64_t t;
        queue.PopFront(&t);
        if (t < 0)
            break;
    }
    
    thread.Join();
}

} // namespace common


