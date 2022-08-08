//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 17:09
//  @file:      functions.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_ATOMIC_FUNCTIONS_H
#define COMMON_SYSTEM_CONCURRENCY_ATOMIC_FUNCTIONS_H

/*

Function style interface

///////////////////////////////////////////////////////////////////////////////
// return value
AtomicGet

///////////////////////////////////////////////////////////////////////////////
// Change value and return
AtomicSet
AtomicAdd
AtomicSub
AtomicAnd
AtomicOr
AtomicXor

T AtomicExchange<Operation>(T& target, T value)
Operation:
    atomically
    {
        target operation value;
        return target;
    }

///////////////////////////////////////////////////////////////////////////////
// change value and return old value

AtomicExchangeSet
AtomicExchangeAdd
AtomicExchangeSub
AtomicExchangeAnd
AtomicExchangeOr
AtomicExchangeXor

Prototype:
    T AtomicExchange<Operation>(T& target, T value)

Operation:
    atomically
    {
        T old = target;
        target operation value;
        return old;
    }


///////////////////////////////////////////////////////////////////////////////
// compare and change

Prototype:
    bool AtomicCompareExchange(T& value, T compare, T exchange, T& old)

Operation:
    atomically
    {
        old = value;
        if (value == compare)
        {
            value = exchange;
            return true;
        }
        return false;
    }

*/

// import implementation for each platform
#if defined _MSC_VER
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicMsc.h"
#elif defined __GNUC__
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicGcc.h"
#else
#error unsupported compiler
#endif

// convert implementation to global namespace
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicAdapter.h"

#endif // COMMON_SYSTEM_CONCURRENCY_ATOMIC_FUNCTIONS_H
