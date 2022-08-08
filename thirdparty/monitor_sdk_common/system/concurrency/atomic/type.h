//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 17:10
//  @file:      type.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_ATOMIC_TYPE_H
#define COMMON_SYSTEM_CONCURRENCY_ATOMIC_TYPE_H

#include "thirdparty/monitor_sdk_common/base/deprecate.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/atomic.h"


namespace common {
    
template <typename T>
class Atomic
{
    typedef Atomic<T> ThisType;
public:
    Atomic() : m_value()
    {
    }
    
    // implicit
    Atomic(T value) : m_value(value) // NOLINT(runtime/explicit)
    {
    }
    
    // generated copy ctor is ok
    
    operator T() const
    {
        return AtomicGet(&m_value);
    }
    
    T Value() const
    {
        return AtomicGet(&m_value);
    }
    
    ThisType& operator=(T value)
    {
        AtomicSet(&m_value, value);
        return *this;
    }
    
    T operator++()
    {
        return Increment();
    }
    
    T operator++(int)
    {
        return ExchangeAddWith(1);
    }
    
    T operator--()
    {
        return Decrement();
    }
    
    T operator--(int)
    {
        return ExchangeSubWith(1);
    }
    
    T operator+=(T value)
    {
        return AddWith(value);
    }
    
    T operator-=(T value)
    {
        return SubWith(value);
    }
    
    T operator&=(T value)
    {
        return AndWith(value);
    }
    
    T operator|=(T value)
    {
        return OrWith(value);
    }
    
    T operator^=(T value)
    {
        return XorWith(value);
    }
    
public:
    T Exchange(const T value)
    {
        return AtomicExchange(&m_value, value);
    }
    
    T Increment()
    {
        return AtomicIncrement(&m_value);
    }
    
    T Decrement()
    {
        return AtomicDecrement(&m_value);
    }
    
    T ExchangeAddWith(T value)
    {
        return AtomicExchangeAdd(&m_value, value);
    }
    
    T ExchangeSubWith(T value)
    {
        return AtomicExchangeSub(&m_value, value);
    }
    
    T ExchangeAndWith(T value)
    {
        return AtomicExchangeAnd(&m_value, value);
    }
    
    T ExchangeOrWith(T value)
    {
        return AtomicExchangeOr(&m_value, value);
    }
    
    T ExchangeXorWith(T value)
    {
        return AtomicExchangeXor(&m_value, value);
    }
    
    T AddWith(T value)
    {
        return AtomicAdd(&m_value, value);
    }
    
    T SubWith(T value)
    {
        return AtomicSub(&m_value, value);
    }
    
    T AndWith(T value)
    {
        return AtomicAnd(&m_value, value);
    }
    
    T OrWith(T value)
    {
        return AtomicOr(&m_value, value);
    }
    
    T XorWith(T value)
    {
        return AtomicXor(&m_value, value);
    }
    
    bool CompareExchange(T compare, T exchange, T* old)
    {
        return AtomicCompareExchange(&m_value, compare, exchange, old);
    }
    
    bool CompareExchange(T compare, T exchange)
    {
        return AtomicCompareExchange(&m_value, compare, exchange);
    }
    
    
private:
    volatile T m_value;
};
    
template <typename T>
class Atomic<T*>
{
    typedef Atomic<T*> ThisType;
public:
    Atomic() : m_value()
    {
    }
    
    // implicit
    Atomic(T* value) : m_value(value) // NOLINT(runtime/explicit)
    {
    }
    
    // generated copy ctor is ok
    
    operator T*() const
    {
        return AtomicGet(&m_value);
    }
    
    T* Value() const
    {
        return AtomicGet(&m_value);
    }
    
    ThisType& operator=(T* value)
    {
        m_value = value;
        return *this;
    }
    
    T* Exchange(T* exchange)
    {
        return AtomicExchange(&m_value, exchange);
    }
    
    bool CompareExchange(T* compare, T* exchange, T** old)
    {
        return AtomicCompareExchange(&m_value, compare, exchange, old);
    }
    
    DEPRECATED_BY(bool CompareExchange(T* compare, T* exchange, T** old))
    bool CompareExchange(T* compare, T* exchange, T*& old)
    {
        return AtomicCompareExchange(&m_value, compare, exchange, &old);
    }
private:
    T* volatile m_value;
};
    
} // end of namespace common



#endif // COMMON_SYSTEM_CONCURRENCY_ATOMIC_TYPE_H
