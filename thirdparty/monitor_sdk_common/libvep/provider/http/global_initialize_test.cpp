// Copyright (c) 2020, VIVO Inc. All rights reserved.                                                                                  |  1
// Author: Fly Qiu <rock3qiu@gmail.com>                                                                                                 |~
// Created: 2020-01-09                                                                                                                    |~

#include "global_initialize.h"
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
