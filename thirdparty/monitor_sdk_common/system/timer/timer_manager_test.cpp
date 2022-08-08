//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-16 19:20
//  @file:      timer_manager_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include <iostream>
#include "thirdparty/monitor_sdk_common/base/closure2.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/thread.h"
#include "thirdparty/monitor_sdk_common/system/timer/timer_manager.h"
#include "thirdparty/gtest/gtest.h"

using namespace std;

namespace common {

static int n1 = 0;
static int n2 = 0;
static int n3 = 0;
static int n4 = 0;
static int n5 = 0;

class TimerManagerTest : public testing::Test
{
public:
    void EchoCallBack(int *count, uint64_t timer_id)
    {
        (*count)++;
        cout << "echo callback, timer: " <<
            timer_id << ", count: " << *count << endl;
    }

    void RemoveSelf(int *count, uint64_t timer_id)
    {
        (*count)++;
        cout << "remove self, timer: " << timer_id
            << ", count: " << *count << endl;
        timer_manager.RemoveTimer(timer_id);
    }

    void GetTimerManagerInfo()
    {
        TimerManager::Stats info;
        timer_manager.GetStats(&info);
        cout << "--------------------\n";
        cout << "oneshot: " << info.oneshot_timer_num << endl;
        cout << "period:  " << info.period_timer_num << endl;
        cout << "runover: " << info.estimate_runover_time << endl;
        cout << "--------------------\n";
    }
protected:
    TimerManager timer_manager;
};

TEST_F(TimerManagerTest, EchoTimerTest)
{
    // ���һ�������Զ�ʱ��
    TimerManager::CallbackFunction cb1 = Bind(&TimerManagerTest::EchoCallBack, this, &n1);
    uint64_t id1 = timer_manager.AddPeriodTimer(500, cb1);

    // ���һ�����ڴ�����һ���Զ�ʱ��
    TimerManager::CallbackFunction cb2 = Bind(&TimerManagerTest::EchoCallBack, this, &n2);
    timer_manager.AddOneshotTimer(14, cb2);

    // ���һ����ʱ�䴥���������Զ�ʱ��
    TimerManager::CallbackFunction cb3 = Bind(&TimerManagerTest::EchoCallBack, this, &n3);
    uint64_t id3 = timer_manager.AddPeriodTimer(20, cb3);

    // ���һ�������ܴ�����һ���Զ�ʱ��
    TimerManager::CallbackFunction cb4 = Bind(&TimerManagerTest::EchoCallBack, this, &n4);
    uint64_t id4 = timer_manager.AddOneshotTimer(1000000000000LL, cb4);

    // ��Ϣһ�룬�ȴ�һ���Զ�ʱ��ִ�����
    ThisThread::Sleep(1000);
    // �鿴��ʱ��ʱ���������е�ͳ����Ϣ
    GetTimerManagerInfo();

    // �޸����ڶ�ʱ�������ԣ���������ʱ��
    timer_manager.ModifyTimer(id3, 200);
    ThisThread::Sleep(1000);

    // �޸������Զ�ʱ�����ڻص�����ɾ��
    cout << "timer 3 will be modified to self delete:\n";
    cb3 = Bind(&TimerManagerTest::RemoveSelf, this, &n3);
    timer_manager.ModifyTimer(id3, 200, cb3);

    // �޸�һ���Զ�ʱ�������ԣ����ڻص���ɾ���Լ�
    cout << "timer 4 will be modified to self delete:\n";
    cb4 = Bind(&TimerManagerTest::RemoveSelf, this, &n4);
    timer_manager.ModifyTimer(id4, 1000, cb4);

    // �ȴ�����������ʱ��ִ�����
    ThisThread::Sleep(1000);
    GetTimerManagerInfo();

    // �����һ�������ܴ�����һ���Զ�ʱ��
    TimerManager::CallbackFunction cb5 = Bind(&TimerManagerTest::EchoCallBack, this, &n5);
    timer_manager.AddOneshotTimer(100000000LL, cb5);

    timer_manager.RemoveTimer(id1);
    GetTimerManagerInfo();
}

void Inc(int* p, uint64_t id)
{
    ++*p;
}

TEST_F(TimerManagerTest, Performance)
{
    int n = 0;
    for (int i = 0; i < 500; ++i)
    {
        for (int j = 0; j < 1000; ++j)
            timer_manager.AddOneshotTimer(i, Bind(Inc, &n));
    }

    ::std::cout << "n = " << n << '\n';
    ThisThread::Sleep(1000);
    ::std::cout << "n = " << n << '\n';
}

TEST(TimerManager, Stable)
{
    for (int i = 0; i < 1000; ++i)
    {
        TimerManager timer_manager;
    }
}

TEST(TimerManager, DefaultInstance)
{
    TimerManager& timer_manager = TimerManager::DefaultInstance();
    (void) timer_manager;
}

void OrderTest(int *n, uint64_t id)
{
    ++*n;
}

TEST(TimerManager, Order)
{
    TimerManager& timer_manager = TimerManager::DefaultInstance();
    int n = 0;
    uint64_t id = timer_manager.AddOneshotTimer(10000, Bind(OrderTest, &n));
    timer_manager.AddOneshotTimer(0, Bind(OrderTest, &n));
    ThisThread::Sleep(100);
    timer_manager.RemoveTimer(id);

    EXPECT_EQ(1, n);
}

static void DoNothing(uint64_t id)
{
}

TEST(TimerManager, Stop)
{
    TimerManager timer_manager;
    timer_manager.AddOneshotTimer(10, Bind(DoNothing));
    timer_manager.AddPeriodTimer(100, Bind(DoNothing));
    timer_manager.Stop();
    timer_manager.Stop();
}

} // end of namespace common

