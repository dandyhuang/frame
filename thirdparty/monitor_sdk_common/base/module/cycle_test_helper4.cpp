//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-28 11:59
//  @file:      cycle_test_helper4.cpp
//  @author:    
//  @brief:     test cycle dependancy
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/module.h"

namespace common {

// acyclic modules
DEFINE_MODULE(mod4)
{
    return true;
}
USING_MODULE(mod3);

} // end of namespace common
