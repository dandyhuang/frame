//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 12:02
//  @file:      barrier_closure_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/barrier_closure.h"
#include "thirdparty/gtest/gtest.h"


namespace common {

namespace {

void test_callback(bool* flag) {
    *flag = !*flag;
}

}

TEST(BarrierClosureTest, HitBarrier)
{
    bool flag = false;
    Closure<void>* done = NewClosure(test_callback, &flag);
    BarrierClosure* barrier_done = new BarrierClosure(3, done);
    barrier_done->Run();
    barrier_done->Run();
    barrier_done->Run();
    EXPECT_TRUE(flag);
}

TEST(BarrierClosureTest, NotHitBarrier)
{
    bool flag = false;
    Closure<void>* done = NewClosure(test_callback, &flag);
    BarrierClosure* barrier_done = new BarrierClosure(3, done);
    barrier_done->Run();
    barrier_done->Run();
    EXPECT_FALSE(flag);
    delete barrier_done;
}

} // end of namespace common

