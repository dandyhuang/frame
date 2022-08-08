//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 16:54
//  @file:      AtomicGccX64Asm.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICGCCX64ASM_H
#define COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICGCCX64ASM_H

#include <string.h>

#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicAsmWidth.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicDefault.h"

// X86/X64 common implementation
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicGccX86X64Asm.h"

    
namespace common {

// for eight bytes types
template <>
struct AtomicAsmWidth<8> :
    public AtomicDefaultGet,
    public AtomicDefaultSet,
    public AtomicCompareExchangeEmulated<AtomicAsmWidth<8> >
{
    using AtomicDefaultSet::Set;
        
    template <typename T>
    static void Exchange(volatile T* target, T value, T* old)
    {
        __asm__ __volatile__(
            "xchgq %0, %1"
            :"=r"(*old)
            :"m"(*target), "0"(value)
            :"memory");
    }
        
    template <typename T>
    static T ExchangeAdd(volatile T* target, T value)
    {
        __asm__ __volatile__
        (
            "lock; xaddq %0, %1;"
            :"=r"(value)
            :"m"(*target), "0"(value)
        );
        return value;
    }
        
    template <typename T>
    static bool CompareExchange(volatile T* target, T compare, T exchange, T* old)
    {
        bool result;
        __asm__ __volatile__(
            "lock; cmpxchgq %2,%3\n\t"
            "setz %1"
            : "=a"(*old), "=q"(result)
            : "r"(exchange), "m"(*target), "0"(compare)
            : "memory");
        return result;
    }
        
    static bool CompareExchange(
        volatile double* target,
        double compare,
        double exchange,
        double* old)
    {
        bool result;
        unsigned long long exchange_ll;
        memcpy(&exchange_ll, &exchange, sizeof(exchange_ll));
        __asm__ __volatile__(
            "lock; cmpxchgq %2,%3\n\t"
            "setz %1"
            : "=a"(*old), "=q"(result)
            : "r"(exchange_ll), "m"(*target), "0"(compare)
            : "memory");
        return result;
    }
};

} // end of namespace common



#endif // COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICGCCX64ASM_H

