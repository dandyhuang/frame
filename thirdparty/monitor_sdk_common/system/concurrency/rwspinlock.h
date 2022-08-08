//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 17:57
//  @file:      rwspinlock.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_RWSPINLOCK_H
#define COMMON_SYSTEM_CONCURRENCY_RWSPINLOCK_H
#pragma once

#include <assert.h>

#include "thirdparty/monitor_sdk_common/system/concurrency/rwlock_base.h"
#include "thirdparty/monitor_sdk_common/system/cpu/cpu_relax.h"

namespace common {

/**
 * A lightweight r/w spinlock implementation.
 *
 * SpinLock is designed for very fast operations, such as very lightweight
 * memory reading/writing.
 *
 * In any other case, especially (maybe) with memory allocation or system
 * call, such as I/O, the busy wait will waste observable CPU, you should
 * use normal RWLock.
 */
class RwSpinLock : public RwLockBase<RwSpinLock>
{
    friend class RwLockBase<RwSpinLock>;
public:
    // See RwLockBase for accessable members.
private:
    static void Relax()
    {
        CpuRelax();
    }
};

} // end of namespace common

#endif // COMMON_SYSTEM_CONCURRENCY_LITE_RWLOCK_H
