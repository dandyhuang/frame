//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-28 12:10
//  @file:      module_b.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include <stdio.h>
#include "thirdparty/monitor_sdk_common/base/module.h"
#include "thirdparty/monitor_sdk_common/base/module/module_a.h"

namespace common {

int B;
DEFINE_MODULE(B)
{
    B = GetA() + 1;
    return true;
}

DEFINE_MODULE_DTOR(B)
{
    printf("Module B dtor\n");
}

USING_MODULE(A);

int GetB()
{
    return B;
}

} // end of namespace common
