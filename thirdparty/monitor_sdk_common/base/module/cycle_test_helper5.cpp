//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-28 12:00
//  @file:      cycle_test_helper5.cpp
//  @author:    
//  @brief:     test cycle dependancy
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/module.h"

namespace common {

// a standalone module
DEFINE_MODULE(mod5)
{
    return true;
}

} // end of namespace common
