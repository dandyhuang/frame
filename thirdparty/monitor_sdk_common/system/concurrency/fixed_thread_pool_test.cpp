//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 17:25
//  @file:      fixed_thread_pool_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/concurrency/fixed_thread_pool.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/this_thread.h"
#include "thirdparty/gtest/gtest.h"

// GLOBAL_NOLINE(runtime/int)

namespace common {

class Foo
{
public:
    void test1()
    {
    }
    void test2(intptr_t param)
    {
    }
};

TEST(MemberFunctionTest, MemberFunction)
{
    FixedThreadPool threadpool(4);
    Foo foo;
    for (int i = 0; i < 10; ++i)
    {
        for (int j = 0; j < 20; ++j)
        {
            threadpool.AddTask(MAKE_THREAD_CALLBACK(Foo, test1), &foo);
            threadpool.AddTask(
                MAKE_PARAMETERIZED_THREAD_CALLBACK(Foo, test2, intptr_t), &foo, i * 20 + j);
            FixedThreadPool::Stats stats;
            threadpool.GetStats(&stats);
            // printf("%d: NumThreads=%zu, NumBusyThreads=%zu, NumPengdingTasks=%zu\n",
            //       i*20+j, stat.NumThreads, stat.NumBusyThreads, stat.NumPendingTasks);
        }
    }
}

TEST(MemberFunctionTest, Closure)
{
    FixedThreadPool threadpool(4);
    Foo foo;
    for (int i = 0; i < 10; ++i)
    {
        for (int j = 0; j < 20; ++j)
        {
            threadpool.AddTask(NewClosure(&foo, &Foo::test1));
            threadpool.AddTask(
                NewClosure(
                    &foo, &Foo::test2, static_cast<intptr_t>(i*20+j)));
            FixedThreadPool::Stats stats;
            threadpool.GetStats(&stats);
            // printf("%d: NumThreads=%zu, NumBusyThreads=%zu, NumPengdingTasks=%zu\n",
            //       i*20+j, stat.NumThreads, stat.NumBusyThreads, stat.NumPendingTasks);
        }
    }
}

static void Nop(void*, unsigned long long)
{
}

TEST(FixedThreadPool, Terminate)
{
    FixedThreadPool threadpool;
    for (int i = 0; i < 1000; ++i)
        threadpool.AddTask(Nop);
    threadpool.Terminate();
    threadpool.Terminate();
}

static void blocking(void*, unsigned long long)
{
    ThisThread::Sleep(1000);
}

static void nonblocking(void*, unsigned long long)
{
    ThisThread::Sleep(1);
}

TEST(BlockingTest, Blocking)
{
    FixedThreadPool threadpool;
    threadpool.AddTask(blocking);
    threadpool.AddTask(blocking);
    threadpool.AddTask(blocking);
    for (int i = 0; i < 10; ++i)
    {
        threadpool.AddTask(nonblocking);
    }
}

void test(void*, unsigned long long param)
{
}

TEST(SlowCallTest, SlowCall)
{
    FixedThreadPool threadpool;
    for (int i = 0; i < 4; ++i)
    {
        threadpool.AddTask(test, NULL, i);
        ThisThread::Sleep(1000);
    }
}

TEST(GlobalFunctionTest, GlobalFunction)
{
    FixedThreadPool threadpool(4);
    for (int i = 0; i < 10; ++i)
    {
        for (int j = 0; j < 20; ++j)
        {
            threadpool.AddTask(test, NULL, i*20+j);
            FixedThreadPool::Stats stats;
            threadpool.GetStats(&stats);
        }
    }
}

const int kLoopCount = 500000;

static void DoNothong(void* p, unsigned long long)
{
}

class FixedThreadPoolTest : public testing::TestWithParam<int> {};

TEST_P(FixedThreadPoolTest, Performance)
{
    int num_threads = GetParam();
    std::cout << "Test with " << num_threads << " threads." << "\n";
    FixedThreadPool threadpool(num_threads);
    for (int i = 0; i < kLoopCount; ++i)
        threadpool.AddTask(DoNothong, NULL, 0);
    threadpool.WaitForIdle();
}

INSTANTIATE_TEST_CASE_P(FixedThreadPoolTest, FixedThreadPoolTest, testing::Values(1, 2, 4, 8));


} // namespace common
