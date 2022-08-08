//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 16:51
//  @file:      AtomicFunctionForward.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICFUNCTIONFORWARD_H
#define COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICFUNCTIONFORWARD_H

#define ATOMIC_FORWARD_UNARY_FUNCTION(Name, FromType, ToType) \
    static FromType Name(volatile FromType* target) \
    { \
        return static_cast<FromType>(Name(reinterpret_cast<volatile ToType*>(target))); \
    }

#define ATOMIC_FORWARD_BINARY_FUNCTION(Name, FromType, ToType) \
    static FromType Name(volatile FromType* target, FromType value) \
    { \
        return static_cast<FromType>( \
            Name( \
                reinterpret_cast<volatile ToType*>(target), \
                static_cast<ToType>(value) \
            ) \
        ); \
    }

#define ATOMIC_FORWARD_BINARY_VOID_FUNCTION(Name, FromType, ToType) \
    static void Name(volatile FromType* target, FromType value) \
    { \
        Name( \
            reinterpret_cast<volatile ToType*>(target), \
            static_cast<ToType>(value) \
        ); \
    }

#define ATOMIC_FORWARD_TERNARY_FUNCTION(Name, FromType, ToType) \
    static FromType Name(volatile FromType* target, FromType op1, FromType op2) \
    { \
        return static_cast<FromType>( \
            Name( \
                reinterpret_cast<volatile ToType*>(target), \
                static_cast<ToType>(op1), \
                static_cast<ToType>(op2) \
            ) \
        ); \
    }

#define ATOMIC_FORWARD_CMPXCHG(Name, FromType, ToType) \
    static bool Name(volatile FromType* target, FromType op1, FromType op2, FromType* op3) \
    { \
        return Name( \
            reinterpret_cast<volatile ToType*>(target), \
            static_cast<ToType>(op1), \
            static_cast<ToType>(op2), \
            reinterpret_cast<ToType*>(op3) \
        ); \
    }

#endif // COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICFUNCTIONFORWARD_H
