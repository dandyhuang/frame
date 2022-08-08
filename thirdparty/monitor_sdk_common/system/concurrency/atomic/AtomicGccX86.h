//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 16:55
//  @file:      AtomicGccX86.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICGCCX86_H
#define COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICGCCX86_H

#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicGccX86Asm.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicAsm.h"
    
namespace common {

        
typedef AtomicAsm AtomicImplementation;

} // end of namespace common



#endif // COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICGCCX86_H

