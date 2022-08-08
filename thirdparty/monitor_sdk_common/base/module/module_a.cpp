//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-28 12:08
//  @file:      module_a.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include <stdio.h>
#include "thirdparty/monitor_sdk_common/base/module.h"

namespace common {

int A;

DEFINE_MODULE(A)
{
    A = 1;
    return true;
}

DEFINE_MODULE_DTOR(A)
{
    printf("Module A dtor\n");
}

int GetA()
{
    return A;
}

} // end of namespace common
