//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 16:52
//  @file:      AtomicGccX64.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICGCCX64_H
#define COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICGCCX64_H

#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicGccX64Asm.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicAsm.h"

    
namespace common {

typedef AtomicAsm AtomicImplementation;

} // end of namespace common



#endif // COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICGCCX64_H

