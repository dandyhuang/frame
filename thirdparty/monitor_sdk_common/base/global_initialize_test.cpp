//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 17:49
//  @file:      global_initialize_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/global_initialize.h"
#include "thirdparty/gtest/gtest.h"

static int global_int = 0;

GLOBAL_INITIALIZE(global_initialize_test)
{
    global_int = 1;
}

TEST(GlibalInit, Test)
{
    EXPECT_EQ(1, global_int);
}
