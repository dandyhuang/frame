//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 17:06
//  @file:      AtomicMscX86.h
//  @author:    
//  @brief:     
//
//********************************************************************

#ifndef COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICMSCX86_H
#define COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICMSCX86_H


#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicMscX86Asm.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicAsm.h"

    
namespace common {

typedef AtomicAsm AtomicImplementation;

} // end of namespace common


#endif // COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICMSCX86_H


