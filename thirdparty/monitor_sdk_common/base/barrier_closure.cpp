//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 14:02
//  @file:      barrier_closure.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/base/barrier_closure.h"

#include <assert.h>
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/atomic.h"

namespace common {

BarrierClosure::~BarrierClosure() {
    if (m_left > 0)
        delete m_all_done;
    m_all_done = NULL;
}

void BarrierClosure::Run() {
    int count = AtomicDecrement(&m_left);
    assert(count >= 0);
    if (count == 0) {
        m_all_done->Run();
        delete this;
    }
}

} // end of namespace common
