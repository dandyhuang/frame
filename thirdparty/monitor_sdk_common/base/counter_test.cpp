//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 13:42
//  @file:      counter_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include <iostream>
#include "thirdparty/monitor_sdk_common/base/counter.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/thread.h"
#include "thirdparty/gtest/gtest.h"

using common::RateCounter;
using common::StatCounter;


TEST(Counter, RateCounter)
{
    RateCounter rate_counter(5);
    // No data
    float rate = rate_counter.GetRate();
    ::std::cout << "Rate counter has no data, rate is " << rate << ::std::endl;
    EXPECT_LT(rate, 0.0001);
    // Increment 16 times
    for (size_t i = 0; i < 16; ++i)
    {
        rate_counter.Increment();
    }
    rate = rate_counter.GetRate();
    ::std::cout << "Increase 16 times, rate is " << rate << ::std::endl;
    EXPECT_GT(rate, 3.0);

    rate_counter.Add(10);
    rate = rate_counter.GetRate();
    ::std::cout << "Continue to add 10, rate is " << rate << ::std::endl;
    EXPECT_TRUE(rate > 5.0);

    ::std::cout << "Sleep 6 seconds ..." << ::std::endl;
    common::ThisThread::Sleep(6000);
    // Now all data should be expired after 6 seconds
    rate = rate_counter.GetRate();
    ::std::cout << "All data is expired, rate is " << rate << ::std::endl;
    EXPECT_TRUE(rate < 0.0001);

    for (size_t i = 0; i < 7; ++i)
    {
        // Add 2 per 800 milliseconds, suppose there will be 5/0.8 = 6 samples
        common::ThisThread::Sleep(800);
        rate_counter.Add(2);
    }

    rate = rate_counter.GetRate();
    ::std::cout << "Add 2 per 800ms, rate is " << rate << ::std::endl;
    EXPECT_TRUE(rate > 2.0);

    // Reset
    rate_counter.Reset();
    rate = rate_counter.GetRate();
    ::std::cout << "Reset, rate is " << rate << ::std::endl;
    EXPECT_TRUE(rate < 0.0001);

    rate_counter.Add(10);
    common::ThisThread::Sleep(4000);
    rate_counter.Add(10);
    rate = rate_counter.GetRate();
    ::std::cout << "Add 10 and 10 between 4 seconds, rate is " << rate << ::std::endl;
    EXPECT_TRUE(rate > 3.99);

    rate_counter.Reset();
    rate_counter.Add(10);
    common::ThisThread::Sleep(5000);
    rate_counter.Add(10);
    rate = rate_counter.GetRate();
    ::std::cout << "Add 10 and 10 between 5 seconds, rate is " << rate << ::std::endl;
    EXPECT_TRUE(rate < 2.01 && rate > 1.99);
}

TEST(Counter, StatCounter)
{
    StatCounter sample_counter(10);
    EXPECT_EQ(0, sample_counter.GetAverage());
    EXPECT_EQ(INT32_MIN, sample_counter.GetMax());
    EXPECT_EQ(INT32_MAX, sample_counter.GetMin());

    int32_t sum = 0;
    // 521
    sample_counter.AddSample(521);
    sum += 521;
    EXPECT_EQ(521, sample_counter.GetAverage());
    EXPECT_EQ(521, sample_counter.GetMax());
    EXPECT_EQ(521, sample_counter.GetMin());

    // 521, 120
    sample_counter.AddSample(120);
    sum += 120;
    EXPECT_EQ(sum/2, sample_counter.GetAverage());
    EXPECT_EQ(521, sample_counter.GetMax());
    EXPECT_EQ(120, sample_counter.GetMin());

    for (size_t i = 0; i < 6; ++i)
    {
        sample_counter.AddSample(130);
        sum += 130;
    }
    // 521, 120, 130, 130, 130, 130, 130, 130,
    EXPECT_EQ(sum/8, sample_counter.GetAverage());
    EXPECT_EQ(521, sample_counter.GetMax());
    EXPECT_EQ(120, sample_counter.GetMin());

    sample_counter.AddSample(256);
    sum += 256;
    sample_counter.AddSample(256);
    sum += 256;
    // 521, 120, 130, 130, 130, 130, 130, 130, 256, 256
    EXPECT_EQ(sum/10, sample_counter.GetAverage());
    EXPECT_EQ(521, sample_counter.GetMax());
    EXPECT_EQ(120, sample_counter.GetMin());

    // 120, 130, 130, 130, 130, 130, 130, 256, 256, 250
    sample_counter.AddSample(250);
    sum -= 521;
    sum += 250;
    EXPECT_EQ(sum/10, sample_counter.GetAverage());
    EXPECT_EQ(256, sample_counter.GetMax());
    EXPECT_EQ(120, sample_counter.GetMin());

    // 130, 130, 130, 130, 130, 130, 256, 256, 250, 140
    sample_counter.AddSample(140);
    sum -= 120;
    sum += 140;
    EXPECT_EQ(sum/10, sample_counter.GetAverage());
    EXPECT_EQ(256, sample_counter.GetMax());
    EXPECT_EQ(130, sample_counter.GetMin());

    for (size_t i = 0; i < 5; ++i)
    {
        sample_counter.AddSample(150);
        sum -= 130;
        sum += 150;
        EXPECT_EQ(sum/10, sample_counter.GetAverage());
        EXPECT_EQ(256, sample_counter.GetMax());
        EXPECT_EQ(130, sample_counter.GetMin());
    }

    // 256, 256, 250, 140, 150, 150, 150, 150, 150, 150
    sample_counter.AddSample(150);
    sum -= 130;
    sum += 150;
    EXPECT_EQ(sum/10, sample_counter.GetAverage());
    EXPECT_EQ(256, sample_counter.GetMax());
    EXPECT_EQ(140, sample_counter.GetMin());

    // 256, 250, 140, 150, 150, 150, 150, 150, 150, 150
    sample_counter.AddSample(150);
    sum -= 256;
    sum += 150;
    EXPECT_EQ(sum/10, sample_counter.GetAverage());
    EXPECT_EQ(256, sample_counter.GetMax());
    EXPECT_EQ(140, sample_counter.GetMin());

    // 250, 140, 150, 150, 150, 150, 150, 150, 150, 100
    sample_counter.AddSample(100);
    sum -= 256;
    sum += 100;
    EXPECT_EQ(sum/10, sample_counter.GetAverage());
    EXPECT_EQ(250, sample_counter.GetMax());
    EXPECT_EQ(100, sample_counter.GetMin());

    // 140, 150, 150, 150, 150, 150, 150, 150, 100, 100
    sample_counter.AddSample(100);
    sum -= 250;
    sum += 100;
    EXPECT_EQ(sum/10, sample_counter.GetAverage());
    EXPECT_EQ(150, sample_counter.GetMax());
    EXPECT_EQ(100, sample_counter.GetMin());

    // 150, 150, 150, 150, 150, 150, 150, 100, 100, 1000
    sample_counter.AddSample(1000);
    sum -= 140;
    sum += 1000;
    EXPECT_EQ(sum/10, sample_counter.GetAverage());
    EXPECT_EQ(1000, sample_counter.GetMax());
    EXPECT_EQ(100, sample_counter.GetMin());

    // 150, 150, 150, 150, 150, 150, 100, 100, 1000, -2000
    sample_counter.AddSample(-2000);
    sum -= 150;
    sum += -2000;
    EXPECT_EQ(sum/10, sample_counter.GetAverage());
    EXPECT_EQ(1000, sample_counter.GetMax());
    EXPECT_EQ(-2000, sample_counter.GetMin());

    sample_counter.Reset();
    EXPECT_EQ(0, sample_counter.GetAverage());
    EXPECT_EQ(INT32_MIN, sample_counter.GetMax());
    EXPECT_EQ(INT32_MAX, sample_counter.GetMin());

    sum = 0;
    sample_counter.AddSample(-200);
    sum += -200;
    sample_counter.AddSample(0);
    sample_counter.AddSample(-3);
    sum += -3;
    sample_counter.AddSample(0);
    sample_counter.AddSample(-123);
    sum += -123;

    // -200, 0, -3, 0, -123
    EXPECT_EQ(sum/10, sample_counter.GetAverage());
    EXPECT_EQ(0, sample_counter.GetMax());
    EXPECT_EQ(-200, sample_counter.GetMin());
}

