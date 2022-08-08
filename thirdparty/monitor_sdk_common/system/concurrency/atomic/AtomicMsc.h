//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 17:01
//  @file:      AtomicMsc.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICMSC_H
#define COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICMSC_H

#ifdef _M_IX86
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicMscX86.h"
#else
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicMscIntrinsic.h"
    
namespace common {

typedef AtomicMscIntrinsic AtomicImplementation;

} // end of namespace common


#endif

#endif // COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICMSC_H

