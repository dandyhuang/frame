//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 16:50
//  @file:      AtomicDefault.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICDEFAULT_H
#define COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICDEFAULT_H

#include <stddef.h>

    
namespace common {

// common get operation for aligned reading
struct AtomicDefaultGet
{
    template <typename T>
    static T Get(const volatile T* target)
    {
        return *target;
    }
};
        
// common get operation for aligned writing
struct AtomicDefaultSet
{
    template <typename T>
    static void Set(volatile T* target, T value)
    {
        *target = value;
    }
};
        
// Policy class, emulate operations by using compare and exchange
template <typename Provider>
struct AtomicCompareExchangeEmulated
{
    template <typename T>
    static void Exchange(volatile T* target, T value, T* old)
    {
        *old = *target;
        
        while (!Provider::CompareExchange(target, *old, value, old))
        {
        }
    }
        
    template <typename T>
    static void Set(volatile T* target, T value)
    {
        T old;
        Exchange(target, value, &old);
    }
        
    template <typename T>
    static T ExchangeAdd(volatile T* target, T value)
    {
        T old = *target;
        
        while (!Provider::CompareExchange(target, old, static_cast<T>(old + value), &old))
        {
        }
        
        return old;
    }
        
    template <typename T>
    static T ExchangeSub(volatile T* target, T value)
    {
        return ExchangeAdd(target, -value);
    }
        
    template <typename T>
    static T Add(volatile T* target, T value)
    {
        return ExchangeAdd(target, value) + value;
    }
        
    template <typename T>
    static T Sub(volatile T* target, T value)
    {
        return ExchangeSub(target, value) - value;
    }
        
    template <typename T>
    static T Increment(volatile T* target)
    {
        return Add(target, static_cast<T>(1));
    }
        
    template <typename T>
    static T Decrement(volatile T* target)
    {
        return Sub(target, static_cast<T>(1));
    }
        
    template <typename T>
    static T ExchangeAnd(volatile T* target, T value)
    {
        T old = *target;
        
        while (!Provider::CompareExchange(target, old, static_cast<T>(old & value), &old))
        {
        }
        
        return old;
    }
        
    template <typename T>
    static T ExchangeOr(volatile T* target, T value)
    {
        T old = *target;
        
        while (!Provider::CompareExchange(target, old, static_cast<T>(old | value), &old))
        {
        }
        
        return old;
    }
        
    template <typename T>
    static T ExchangeXor(volatile T* target, T value)
    {
        T old = *target;
        
        while (!Provider::CompareExchange(target, old, static_cast<T>(old ^ value), &old))
        {
        }
        
        return old;
    }
};

} // end of namespace common


#endif // COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICDEFAULT_H
