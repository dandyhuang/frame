//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 17:03
//  @file:      AtomicMscIntrinsic.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICMSCINTRINSIC_H
#define COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICMSCINTRINSIC_H

#include <intrin.h>
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicDefault.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicFunctionForward.h"


namespace common {

///////////////////////////////////////////////////////////////////////////////
// 8 bit
        
struct AtomicMscIntrinsic :
    public AtomicDefaultGet,
    public AtomicDefaultSet,
    public AtomicCompareExchangeEmulated<AtomicMscIntrinsic>
{
    using AtomicDefaultGet::Get;
    using AtomicDefaultSet::Set;
        
    using AtomicCompareExchangeEmulated<AtomicMscIntrinsic>::Increment;
    using AtomicCompareExchangeEmulated<AtomicMscIntrinsic>::Add;
    using AtomicCompareExchangeEmulated<AtomicMscIntrinsic>::Sub;
    using AtomicCompareExchangeEmulated<AtomicMscIntrinsic>::Decrement;
        
    __if_exists(_InterlockedExchange8)
    {
        static char Exchange(volatile char* target, char value)
        {
            return _InterlockedExchange8(target, value);
        }
        ATOMIC_FORWARD_BINARY_FUNCTION(Exchange, signed char, char)
        ATOMIC_FORWARD_BINARY_FUNCTION(Exchange, unsigned char, char)
    }
        
    ///////////////////////////////////////////////////////////////////////////////
    // 16 bit
    __if_exists(_InterlockedExchange16)
    {
        static short Exchange(volatile short* target, short value)
        {
            return _InterlockedExchange16(target, value);
        }
        ATOMIC_FORWARD_BINARY_FUNCTION(Exchange, unsigned short, short)
    }
        
    // Increment
    __if_exists(_InterlockedIncrement16)
    {
        static short Increment(volatile short* target)
        {
            return _InterlockedIncrement16(target);
        }
        ATOMIC_FORWARD_UNARY_FUNCTION(Increment, unsigned short, short)
    }
        
    // Decrement
    __if_exists(_InterlockedDecrement16)
    {
        static short Decrement(volatile short* target)
        {
            return _InterlockedDecrement16(target);
        }
        ATOMIC_FORWARD_UNARY_FUNCTION(Decrement, unsigned short, short)
    }
        
    ///////////////////////////////////////////////////////////////////////////////
    // 32 bit
        
    // Exchange
    static long Exchange(volatile long* target, long value)
    {
        return _InterlockedExchange(target, value);
    }
        
    ATOMIC_FORWARD_BINARY_FUNCTION(Exchange, unsigned long, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(Exchange, int, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(Exchange, unsigned int, long)
        
    // Increment
    static long Increment(volatile long* target)
    {
        return _InterlockedIncrement(target);
    }
    ATOMIC_FORWARD_UNARY_FUNCTION(Increment, unsigned long, long)
    ATOMIC_FORWARD_UNARY_FUNCTION(Increment, int, long)
    ATOMIC_FORWARD_UNARY_FUNCTION(Increment, unsigned int, long)
        
    // Decrement
    static long Decrement(volatile long* target)
    {
        return _InterlockedDecrement(target);
    }
    ATOMIC_FORWARD_UNARY_FUNCTION(Decrement, unsigned long, long)
    ATOMIC_FORWARD_UNARY_FUNCTION(Decrement, int, long)
    ATOMIC_FORWARD_UNARY_FUNCTION(Decrement, unsigned int, long)
        
    // Add
    static long ExchangeAdd(volatile long* target, long value)
    {
        return _InterlockedExchangeAdd(target, value);
    }
    ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeAdd, unsigned long, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeAdd, int, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeAdd, unsigned int, long)
        
    static long Add(volatile long* target, long value)
    {
        return _InterlockedExchangeAdd(target, value) + value;
    }
    ATOMIC_FORWARD_BINARY_FUNCTION(Add, unsigned long, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(Add, int, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(Add, unsigned int, long)
        
    // Sub
    static long ExchangeSub(volatile long* target, long value)
    {
        return ExchangeAdd(target, -value);
    }
    ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeSub, unsigned long, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeSub, int, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeSub, unsigned int, long)
        
    static long Sub(volatile long* target, long value)
    {
        return ExchangeSub(target, value) - value;
    }
    ATOMIC_FORWARD_BINARY_FUNCTION(Sub, unsigned long, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(Sub, int, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(Sub, unsigned int, long)
        
    // And
    static long ExchangeAnd(volatile long* target, long value)
    {
        return _InterlockedAnd(target, value);
    }
    ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeAnd, unsigned long, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeAnd, int, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeAnd, unsigned int, long)
        
    static long And(volatile long* target, long value)
    {
        return _InterlockedAnd(target, value) & value;
    }
    ATOMIC_FORWARD_BINARY_FUNCTION(And, unsigned long, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(And, int, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(And, unsigned int, long)
        
    // Or
    static long ExchangeOr(volatile long* target, long value)
    {
        return _InterlockedOr(target, value);
    }
    ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeOr, unsigned long, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeOr, int, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeOr, unsigned int, long)
        
    static long Or(volatile long* target, long value)
    {
        return _InterlockedOr(target, value) | value;
    }
    ATOMIC_FORWARD_BINARY_FUNCTION(Or, unsigned long, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(Or, int, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(Or, unsigned int, long)
        
    // Xor
    static long ExchangeXor(volatile long* target, long value)
    {
        return _InterlockedXor(target, value);
    }
    ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeXor, unsigned long, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeXor, int, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeXor, unsigned int, long)
        
    // Xor
    static long Xor(volatile long* target, long value)
    {
        return _InterlockedXor(*target, value) ^ value;
    }
    ATOMIC_FORWARD_BINARY_FUNCTION(Xor, unsigned long, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(Xor, int, long)
    ATOMIC_FORWARD_BINARY_FUNCTION(Xor, unsigned int, long)
        
    // CompareExchange
    static bool CompareExchange(volatile long* target, long compare, long exchange, long* old)
    {
        *old = _InterlockedCompareExchange(target, exchange, compare);
        return compare == old;
    }
    ATOMIC_FORWARD_CMPXCHG(CompareExchange, unsigned long, long)
    ATOMIC_FORWARD_CMPXCHG(CompareExchange, int, long)
    ATOMIC_FORWARD_CMPXCHG(CompareExchange, unsigned int, long)
    ATOMIC_FORWARD_CMPXCHG(CompareExchange, float, long)
        
    ///////////////////////////////////////////////////////////////////////////////
    // 64 bit
    /*
        static long long Get(const volatile long long* target)
        {
            return *target;
        }
        ATOMIC_FORWARD_UNARY_FUNCTION(Get, const unsigned long long, const long long)
        ATOMIC_FORWARD_UNARY_FUNCTION(Get, const double, const long long)
        
        static void Set(volatile long long* target, long long value)
        {
            *target = value;
        }
        
        ATOMIC_FORWARD_BINARY_VOID_FUNCTION(Set, unsigned long long, long long)
        ATOMIC_FORWARD_BINARY_VOID_FUNCTION(Set, double, long long)
    */
    __if_exists(_InterlockedExchange64)
    {
        static long long Exchange(volatile long long* target, long long value)
        {
            return _InterlockedExchange64(target, value);
        }
        ATOMIC_FORWARD_BINARY_FUNCTION(Exchange, unsigned long long, long long)
    }
        
    __if_exists(_InterlockedIncrement64)
    {
        static long long Increment(volatile long long* target)
        {
            return _InterlockedIncrement64(target);
        }
        ATOMIC_FORWARD_UNARY_FUNCTION(Increment, unsigned long long, long long)
    }
        
    __if_exists(_InterlockedDecrement64)
    {
        static long long Decrement(volatile long long* target)
        {
            return _InterlockedDecrement64(target);
        }
        ATOMIC_FORWARD_UNARY_FUNCTION(Decrement, unsigned long long, long long)
    }
        
    // Add & sub
    __if_exists(_InterlockedExchangeAdd64)
    {
        static long long ExchangeAdd(volatile long long* target, long long value)
        {
            return _InterlockedExchangeAdd64(target, value);
        }
        static long long ExchangeSub(volatile long long* target, long long value)
        {
            return _InterlockedExchangeAdd64(target, -value);
        }
        ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeAdd, unsigned long long, long long)
        ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeSub, unsigned long long, long long)
    }
        
    __if_exists(_InterlockedAdd64)
    {
        static long long Add(volatile long long* target, long long value)
        {
            return _InterlockedAdd64(target, value);
        }
        static long long Sub(volatile long long* target, long long value)
        {
            return _InterlockedAdd64(target, -value);
        }
        ATOMIC_FORWARD_BINARY_FUNCTION(Add, unsigned long long, long long)
        ATOMIC_FORWARD_BINARY_FUNCTION(Sub, unsigned long long, long long)
    }
        
    __if_not_exists(_InterlockedAdd64)
    {
        __if_exists(_InterlockedExchangeAdd64)
        {
            static long long Add(volatile long long* target, long long value)
            {
                return _InterlockedExchangeAdd64(target, value) + value;
            }
            static long long Sub(volatile long long* target, long long value)
            {
                return _InterlockedExchangeAdd64(target, value) - value;
            }
            ATOMIC_FORWARD_BINARY_FUNCTION(Add, unsigned long long, long long)
            ATOMIC_FORWARD_BINARY_FUNCTION(Sub, unsigned long long, long long)
        }
    }
        
    // And
    __if_exists(_InterlockedAnd64)
    {
        static long long ExchangeAnd(volatile long long* target, long long value)
        {
            return _InterlockedAnd64(target, value);
        }
        ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeAnd, unsigned long long, long long)
        
        static long long And(volatile long long* target, long long value)
        {
            return _InterlockedAnd64(target, value) & value;
        }
        ATOMIC_FORWARD_BINARY_FUNCTION(And, unsigned long long, long long)
    }
        
    // Or
    __if_exists(_InterlockedOr64)
    {
        static long long ExchangeOr(volatile long long* target, long long value)
        {
            return _InterlockedOr64(target, value);
        }
        ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeOr, unsigned long long, long long)
        
        static long long Or(volatile long long* target, long long value)
        {
            return _InterlockedOr64(*target, value) | value;
        }
        ATOMIC_FORWARD_BINARY_FUNCTION(Or, unsigned long long, long long)
    }
        
    // Xor
    __if_exists(_InterlockedXor64)
    {
        static long long ExchangeXor(volatile long long* target, long long value)
        {
            return _InterlockedXor64(target, value);
        }
        ATOMIC_FORWARD_BINARY_FUNCTION(ExchangeXor, unsigned long long, long long)
        
        static long long Xor(volatile long long* target, long long value)
        {
            return _InterlockedXor64(target, value) ^ value;
        }
        ATOMIC_FORWARD_BINARY_FUNCTION(Xor, unsigned long long, long long)
    }
        
    // cmpxchg
    __if_exists(_InterlockedCompareExchange64)
    {
        static bool CompareExchange(volatile long long* target,
                                    long long compare,
                                    long long exchange,
                                    long long* old)
        {
            *old = _InterlockedCompareExchange64(target, exchange, compare);
            return compare == old;
        }
        ATOMIC_FORWARD_CMPXCHG(CompareExchange, unsigned long long, long long)
        ATOMIC_FORWARD_CMPXCHG(CompareExchange, double, long long)
    }
        
    ///////////////////////////////////////////////////////////////////////////////
    // For pointers
    template <typename T>
    static T* CompareExchange(T* volatile* target, T* compare, T* exchange)
    {
        return _InterlockedCompareExchangePointer(target, exchange, compare);
    }
        
    template <typename T>
    static T* Exchange(T* volatile* target, T* exchange)
    {
        return _InterlockedExchangePointer(target, exchange);
    }
        
    // for float
    static float Exchange(volatile float* target, float value)
    {
        AtomicCompareExchangeEmulated<AtomicMscIntrinsic>::Exchange(target, value, &value);
        return value;
    }
    static double Exchange(volatile double* target, double value)
    {
        AtomicCompareExchangeEmulated<AtomicMscIntrinsic>::Exchange(target, value, &value);
        return value;
    }
};

} // end of namespace common



#endif // COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICMSCINTRINSIC_H
