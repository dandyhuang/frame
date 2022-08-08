//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-28 11:56
//  @file:      cycle_test_helper2.cpp
//  @author:    
//  @brief:     test cycle dependancy
//
//********************************************************************

#include "thirdparty/monitor_sdk_common/base/module.h"

namespace common {

// cyclic module
DEFINE_MODULE(mod2)
{
    return true;
}
USING_MODULE(mod1);

} // end of namespace common
