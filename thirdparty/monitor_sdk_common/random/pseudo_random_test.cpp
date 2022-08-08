//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-15 11:50
//  @file:      pseudo_random_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/random/pseudo_random.h"

#include <stdio.h>
#include <time.h>

#include "thirdparty/gtest/gtest.h"

namespace common {

TEST(PseudoRandom, NextUInt32)
{
    PseudoRandom r(1);
    double total = 0.0;
    for (int i = 0; i < 1000; ++i)
    {
        uint32_t n = r.NextUInt32();
        EXPECT_GE(n, 0U);
        EXPECT_LE(n, UINT32_MAX);
        total += n;
    }
    EXPECT_NEAR(total / 1000 / UINT32_MAX, 0.5, 0.05);
}

TEST(PseudoRandom, NextUInt32WithRange)
{
    PseudoRandom r(2);
    uint32_t max = 10000;
    double total = 0.0;
    for (int i = 0; i < 1000; ++i)
    {
        uint32_t n = r.NextUInt32(max);
        EXPECT_GE(n, 0U);
        EXPECT_LT(n, max);
        total += n;
    }
    EXPECT_NEAR(total / 1000 / max, 0.5, 0.05);
}

TEST(PseudoRandom, NextDouble)
{
    PseudoRandom r(3);
    double total = 0.0;
    for (int i = 0; i < 1000; ++i)
    {
        double n = r.NextDouble();
        EXPECT_GE(n, 0);
        EXPECT_LE(n, 1.0);
        total += n;
    }
    EXPECT_NEAR(total / 1000, 0.5, 0.05);
}

} // end of namespace common
