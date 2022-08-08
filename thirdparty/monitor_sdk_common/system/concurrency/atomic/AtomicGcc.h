//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 16:52
//  @file:      AtomicGcc.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICGCC_H
#define COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICGCC_H

#if __i386__
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicGccX86.h"
#elif __x86_64__
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicGccX64.h"
#else
#error unsupported architect
#endif

#endif // COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICGCC_H

