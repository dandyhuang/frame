//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 17:08
//  @file:      AtomicMscX86Asm.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICMSCX86ASM_H
#define COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICMSCX86ASM_H

#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicAsmWidth.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/AtomicDefault.h"

// GLOBAL_NOLINT(whitespace/braces)

// NOTE:
// the following embedded assembly functions has no explicit return statement
// because of this x86/x64 call convention:
// interger/pointer return value is in the al/ax/eax/edx:eax/rax register

    
namespace common {

// for one byte types
template <>
struct AtomicAsmWidth<1> :
    public AtomicDefaultGet,
    public AtomicDefaultSet,
    public AtomicCompareExchangeEmulated<AtomicAsmWidth<1> >
{
    using AtomicDefaultSet::Set;
        
    template <typename T>
    static void Exchange(volatile T* target, T value, T* old)
    {
        __asm
        {
            mov ecx, target
            mov al, value
            lock xchg [ecx], al
            mov ecx, old
            mov [ecx], al
        }
    }
        
    template <typename T>
    static T ExchangeAdd(volatile T* target, T value)
    {
        __asm
        {
            mov ecx, target
            mov al, value
            lock xadd [ecx], al
        }
    }
        
    template <typename T>
    static T CompareExchange(volatile T* target, T compare, T exchange, T* result)
    {
        __asm
        {
            mov ecx, target
            mov dl, exchange
            mov al, compare
            lock cmpxchg [ecx], dl
            mov ecx, result
            mov [ecx], al
            setz al
        }
    }
};
        
// for two bytes types
template <>
struct AtomicAsmWidth<2> :
    public AtomicDefaultGet,
    public AtomicDefaultSet,
    public AtomicCompareExchangeEmulated<AtomicAsmWidth<2> >
{
    using AtomicDefaultSet::Set;
        
    template <typename T>
    static void Exchange(volatile T* target, T value, T* old)
    {
        __asm
        {
            mov ecx, target
            mov ax, value
            lock xchg [ecx], ax
            mov ecx, old
            mov [ecx], ax
        }
    }
        
    template <typename T>
    static T ExchangeAdd(volatile T* target, T value)
    {
        __asm
        {
            mov ecx, target
            mov ax, value
            lock xadd [ecx], ax
        }
    }
        
    template <typename T>
    static T CompareExchange(volatile T* target, T compare, T exchange, T* result)
    {
        __asm
        {
            mov ecx, target
            mov dx, exchange
            mov ax, compare
            lock cmpxchg [ecx], dx
            mov ecx, result
            mov [ecx], ax
            setz al
        }
    }
};
        
// for four bytes types
template <>
struct AtomicAsmWidth<4> :
    public AtomicDefaultGet,
    public AtomicDefaultSet,
    public AtomicCompareExchangeEmulated<AtomicAsmWidth<4> >
{
    using AtomicDefaultSet::Set;
        
    template <typename T>
    static void Exchange(volatile T* target, T value, T* old)
    {
        __asm
        {
            mov ecx, target
            mov eax, value
            lock xchg [ecx], eax
            mov ecx, old
            mov [ecx], eax
        }
    }
        
    template <typename T>
    static T ExchangeAdd(volatile T* target, T value)
    {
        __asm
        {
            mov ecx, target
            mov eax, value
            lock xadd [ecx], eax
        }
    }
        
    template <typename T>
    static bool CompareExchange(volatile T* target, T compare, T exchange, T* result)
    {
        __asm
        {
            mov ecx, target
            mov edx, exchange
            mov eax, compare
            lock cmpxchg [ecx], edx
            mov ecx, result
            mov [ecx], eax
            setz al
        }
    }
};
        
// for eight bytes types
// IA32 does not support normal 64 bit atomic operation, using cmpxchg8b
// to implement them
template <>
struct AtomicAsmWidth<8> :
    public AtomicDefaultGet,
    public AtomicCompareExchangeEmulated<AtomicAsmWidth<8> >
{
    template <typename T>
    static bool CompareExchange(volatile T* target, T compare, T exchange, T* result)
    {
        __asm
        {
            // esi and ebx will be protected by the compiler
            mov esi, target
            mov edx, dword ptr compare + 4
            mov eax, dword ptr compare
            mov ecx, dword ptr exchange + 4
            mov ebx, dword ptr exchange
            lock cmpxchg8b [esi]
            mov ecx, result
            mov [ecx], eax
            mov [ecx+4], edx
            setz al
        }
    }
};

} // end of namespace common


#endif // COMMON_SYSTEM_CONCURRENCY_ATOMIC_ATOMICMSCX86ASM_H
