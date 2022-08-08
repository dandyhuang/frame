//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 16:56
//  @file:      AtomicGccX86Asm.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICGCCX86ASM_H
#define COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICGCCX86ASM_H

#include <string.h>

#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicAsmWidth.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicDefault.h"

// X86/X64 common implementation
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicGccX86X64Asm.h"

/*
 * If PIC is defined then ebx is used as the `thunk' reg
 * However cmpxchg8b requires ebx
 * In this case we backup the value into esi before cmpxchg8b and then
 * restore ebx from esi after cmpxchg8b.
 */
#undef ATOMIC_BREG
#undef XCHG_ATOMIC_BREG
#if defined(__PIC__) && !defined(__CYGWIN__)
#define ATOMIC_BREG "S"
#define ATOMIC_BACKUP_BREG "xchgl %%ebx,%%esi\n\t"
#define ATOMIC_RESTORE_BREG ATOMIC_BACKUP_BREG
#else
#define ATOMIC_BREG "b"
#define ATOMIC_BACKUP_BREG ""
#define ATOMIC_RESTORE_BREG ""
#endif

    
namespace common {

// for eight bytes types
template <>
struct AtomicAsmWidth<8> :
    public AtomicDefaultGet,
    public AtomicCompareExchangeEmulated<AtomicAsmWidth<8> >
{
    template <typename T>
    static bool CompareExchange(volatile T* target, T compare, T exchange, T* old)
    {
        bool result;
        __asm__ __volatile__(
            ATOMIC_BACKUP_BREG
            "lock; cmpxchg8b (%%edi)\n\t"
            ATOMIC_RESTORE_BREG
            "setz %1\n"
            : "=A"(*old), "=q"(result)
            : ATOMIC_BREG((unsigned)exchange),
            "c"((unsigned)((unsigned long long)exchange >> 32)),
            "D"(target),
            "0"(compare)
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
            ATOMIC_BACKUP_BREG
            "lock; cmpxchg8b (%%edi)\n\t"
            ATOMIC_RESTORE_BREG
            "setz %1\n"
            : "=A"(*old), "=q"(result)
            : ATOMIC_BREG((unsigned)exchange_ll),
            "c"((unsigned)(exchange_ll >> 32)),
            "D"(target),
            "0"(compare)
            : "memory");
        return result;
    }
};

} // end of namespace common



#undef ATOMIC_BREG
#undef ATOMIC_BACKUP_BREG
#undef ATOMIC_RESTORE_BREG

#endif // COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICGCCX86ASM_H
