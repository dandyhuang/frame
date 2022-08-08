//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 13:35
//  @file:      stdint_test.cpp
//  @author:    	
//  @brief:     
//
//********************************************************************


#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include "thirdparty/monitor_sdk_common/base/inttypes.h"
#include "thirdparty/monitor_sdk_common/base/stdint.h"

int main()
{
    int8_t i8 = INT8_C(1);
    i8 = INT8_MAX;
    int64_t i64 = INT64_MAX;
    i64 = INT64_C(222222222222222222);
    printf("%" PRId8, i8);
    printf("%" PRId64, i64);
}
