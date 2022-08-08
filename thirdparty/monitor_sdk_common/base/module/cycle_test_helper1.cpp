//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-28 11:55
//  @file:      cycle_test_helper1.cpp
//  @author:    
//  @brief:     test cycle dependancy
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/module.h"

namespace common {

DEFINE_MODULE(mod1)
{
    return true;
}
USING_MODULE(mod2);

} // end of namespace common
