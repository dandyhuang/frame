//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-22 14:58
//  @file:      container_of_test.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/container_of.h"
#include "thirdparty/gtest/gtest.h"

struct Container
{
    int member1;
    int member2;
};

TEST(ContainerOf, Test)
{
    Container c;
    Container* pc = &c;
    int* p1 = &pc->member1;
    int* p2 = &pc->member2;
    EXPECT_EQ(pc, container_of(p1, Container, member1));
    EXPECT_EQ(pc, container_of(p2, Container, member2));
}
