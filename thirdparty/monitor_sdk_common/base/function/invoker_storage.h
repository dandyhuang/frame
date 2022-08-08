//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 13:40
//  @file:      invoker_storage.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_FUNCTION_INVOKER_STORAGE_H
#define COMMON_BASE_FUNCTION_INVOKER_STORAGE_H
#pragma once

#include "thirdparty/monitor_sdk_common/base/function/invoker_storage_base.h"
#include "thirdparty/monitor_sdk_common/base/function/param_traits.h"
#include "thirdparty/monitor_sdk_common/base/function/param_check.h"
#include "thirdparty/monitor_sdk_common/base/function/param_annotation.h"
#include "thirdparty/monitor_sdk_common/base/function/function_traits.h"

namespace common {

namespace internal {

// The method by which a function is invoked is determined by 3 different
// dimensions:
//
//   1) The type of function (normal or method).
//   2) The arity of the function.
//   3) The number of bound parameters.
//
// The templates below handle the determination of each of these dimensions.
// In brief:
//
//   FunctionTraits<> -- Provides a normalied signature, and other traits.
//   InvokerN<> -- Provides a DoInvoke() function that actually executes
//                 a calback.
//   InvokerStorageN<> -- Provides storage for the bound parameters, and
//                        typedefs to the above.
//
// More details about the design of each class is included in a comment closer
// to their defition.

// InvokerN<>
//
// The InvokerN templates contain a static DoInvoke() function that is the key
// to implementing type erasure in the Function() classes.
//
// DoInvoke() is a static function with a fixed signature that is independent
// of StorageType; its first argument is a pointer to the non-templated common
// baseclass of StorageType. This lets us store pointer to DoInvoke() in a
// function pointer that has knowledge of the specific StorageType, and thus
// no knowledge of the bound function and bound parameter types.
//
// As long as we ensure that DoInvoke() is only used with pointers there were
// upcasted from the correct StorageType, we can be sure that execution is
// safe.
//
// The InvokerN templates are the only point that knows the number of bound
// and unbound arguments.  This is intentional because it allows the other
// templates classes in the system to only have as many specializations as
// the max arity of function we wish to support.

template <typename StorageType, typename NormalizedSignature>
struct Invoker0;

// Function: Arity 0 -> 0.
template <
    typename StorageType,
    typename R
>
struct Invoker0<StorageType, R (*)()>
{
    static R DoInvoke(InvokerStorageBase* base)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function();
    }
};

// Function: Arity 1 -> 1.
template <
    typename StorageType,
    typename R, typename X1
>
struct Invoker0<StorageType, R (*)(X1)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X1>::ForwardType x1)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(x1);
    }
};

// Function: Arity 2 -> 2.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2
>
struct Invoker0<StorageType, R (*)(X1, X2)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X1>::ForwardType x1,
        typename internal::ParamTraits<X2>::ForwardType x2)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(x1, x2);
    }
};

// Function: Arity 3 -> 3.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3
>
struct Invoker0<StorageType, R (*)(X1, X2, X3)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X1>::ForwardType x1,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(x1, x2, x3);
    }
};

// Function: Arity 4 -> 4.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4
>
struct Invoker0<StorageType, R (*)(X1, X2, X3, X4)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X1>::ForwardType x1,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(x1, x2, x3, x4);
    }
};

// Function: Arity 5 -> 5.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5
>
struct Invoker0<StorageType, R (*)(X1, X2, X3, X4, X5)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X1>::ForwardType x1,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(x1, x2, x3, x4, x5);
    }
};

// Function: Arity 6 -> 6.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6
>
struct Invoker0<StorageType, R (*)(X1, X2, X3, X4, X5, X6)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X1>::ForwardType x1,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(x1, x2, x3, x4, x5, x6);
    }
};

// Function: Arity 7 -> 7.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7
>
struct Invoker0<StorageType, R (*)(X1, X2, X3, X4, X5, X6, X7)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X1>::ForwardType x1,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6,
        typename internal::ParamTraits<X7>::ForwardType x7)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(x1, x2, x3, x4, x5, x6, x7);
    }
};

// Function: Arity 8 -> 8.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7,
    typename X8
>
struct Invoker0<StorageType, R (*)(X1, X2, X3, X4, X5, X6, X7, X8)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X1>::ForwardType x1,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6,
        typename internal::ParamTraits<X7>::ForwardType x7,
        typename internal::ParamTraits<X8>::ForwardType x8)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(x1, x2, x3, x4, x5, x6, x7, x8);
    }
};

template <typename StorageType, typename NormalizedSignature>
struct Invoker1;

// Function: Arity 1 -> 0.
template <
    typename StorageType,
    typename R, typename X1
>
struct Invoker1<StorageType, R (*)(X1)>
{
    static R DoInvoke(InvokerStorageBase* base)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1));
    }
};

// Method: Arity 0 -> 0.
template <
    typename StorageType,
    typename R,
    typename T

>
struct Invoker1<StorageType, R (T::*)()>
{
    static R DoInvoke(InvokerStorageBase* base)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return ((*Unwrap(invoker->m_p1)).*invoker->function)();
    }
};

// Function: Arity 2 -> 1.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2
>
struct Invoker1<StorageType, R (*)(X1, X2)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X2>::ForwardType x2)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), x2);
    }
};

// Method: Arity 1 -> 1.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1
>
struct Invoker1<StorageType, R (T::*)(X1)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X1>::ForwardType x1)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return ((*Unwrap(invoker->m_p1)).*invoker->function)(x1);
    }
};

// Function: Arity 3 -> 2.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3
>
struct Invoker1<StorageType, R (*)(X1, X2, X3)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), x2, x3);
    }
};

// Method: Arity 2 -> 2.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2
>
struct Invoker1<StorageType, R (T::*)(X1, X2)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X1>::ForwardType x1,
        typename internal::ParamTraits<X2>::ForwardType x2)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return ((*Unwrap(invoker->m_p1)).*invoker->function)(x1, x2);
    }
};

// Function: Arity 4 -> 3.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4
>
struct Invoker1<StorageType, R (*)(X1, X2, X3, X4)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), x2, x3, x4);
    }
};

// Method: Arity 3 -> 3.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3
>
struct Invoker1<StorageType, R (T::*)(X1, X2, X3)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X1>::ForwardType x1,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return ((*Unwrap(invoker->m_p1)).*invoker->function)(x1, x2, x3);
    }
};

// Function: Arity 5 -> 4.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5
>
struct Invoker1<StorageType, R (*)(X1, X2, X3, X4, X5)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), x2, x3, x4, x5);
    }
};

// Method: Arity 4 -> 4.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4
>
struct Invoker1<StorageType, R (T::*)(X1, X2, X3, X4)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X1>::ForwardType x1,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return ((*Unwrap(invoker->m_p1)).*invoker->function)(x1, x2, x3, x4);
    }
};

// Function: Arity 6 -> 5.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6
>
struct Invoker1<StorageType, R (*)(X1, X2, X3, X4, X5, X6)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), x2, x3, x4, x5, x6);
    }
};

// Method: Arity 5 -> 5.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5
>
struct Invoker1<StorageType, R (T::*)(X1, X2, X3, X4, X5)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X1>::ForwardType x1,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return ((*Unwrap(invoker->m_p1)).*invoker->function)(x1, x2, x3, x4,
            x5);
    }
};

// Function: Arity 7 -> 6.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7
>
struct Invoker1<StorageType, R (*)(X1, X2, X3, X4, X5, X6, X7)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6,
        typename internal::ParamTraits<X7>::ForwardType x7)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), x2, x3, x4, x5, x6, x7);
    }
};

// Method: Arity 6 -> 6.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6
>
struct Invoker1<StorageType, R (T::*)(X1, X2, X3, X4, X5, X6)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X1>::ForwardType x1,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return ((*Unwrap(invoker->m_p1)).*invoker->function)(x1, x2, x3, x4,
            x5, x6);
    }
};

// Function: Arity 8 -> 7.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7,
    typename X8
>
struct Invoker1<StorageType, R (*)(X1, X2, X3, X4, X5, X6, X7, X8)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6,
        typename internal::ParamTraits<X7>::ForwardType x7,
        typename internal::ParamTraits<X8>::ForwardType x8)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), x2, x3, x4, x5, x6, x7,
            x8);
    }
};

// Method: Arity 7 -> 7.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7
>
struct Invoker1<StorageType, R (T::*)(X1, X2, X3, X4, X5, X6, X7)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X1>::ForwardType x1,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6,
        typename internal::ParamTraits<X7>::ForwardType x7)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return ((*Unwrap(invoker->m_p1)).*invoker->function)(x1, x2, x3, x4,
            x5, x6, x7);
    }
};

template <typename StorageType, typename NormalizedSignature>
struct Invoker2;

// Function: Arity 2 -> 0.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2
>
struct Invoker2<StorageType, R (*)(X1, X2)>
{
    static R DoInvoke(InvokerStorageBase* base)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2));
    }
};

// Method: Arity 1 -> 0.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1
>
struct Invoker2<StorageType, R (T::*)(X1)>
{
    static R DoInvoke(InvokerStorageBase* base)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2));
    }
};

// Function: Arity 3 -> 1.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3
>
struct Invoker2<StorageType, R (*)(X1, X2, X3)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X3>::ForwardType x3)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            x3);
    }
};

// Method: Arity 2 -> 1.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2
>
struct Invoker2<StorageType, R (T::*)(X1, X2)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X2>::ForwardType x2)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            x2);
    }
};

// Function: Arity 4 -> 2.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4
>
struct Invoker2<StorageType, R (*)(X1, X2, X3, X4)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            x3, x4);
    }
};

// Method: Arity 3 -> 2.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3
>
struct Invoker2<StorageType, R (T::*)(X1, X2, X3)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            x2, x3);
    }
};

// Function: Arity 5 -> 3.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5
>
struct Invoker2<StorageType, R (*)(X1, X2, X3, X4, X5)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            x3, x4, x5);
    }
};

// Method: Arity 4 -> 3.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4
>
struct Invoker2<StorageType, R (T::*)(X1, X2, X3, X4)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            x2, x3, x4);
    }
};

// Function: Arity 6 -> 4.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6
>
struct Invoker2<StorageType, R (*)(X1, X2, X3, X4, X5, X6)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            x3, x4, x5, x6);
    }
};

// Method: Arity 5 -> 4.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5
>
struct Invoker2<StorageType, R (T::*)(X1, X2, X3, X4, X5)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            x2, x3, x4, x5);
    }
};

// Function: Arity 7 -> 5.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7
>
struct Invoker2<StorageType, R (*)(X1, X2, X3, X4, X5, X6, X7)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6,
        typename internal::ParamTraits<X7>::ForwardType x7)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            x3, x4, x5, x6, x7);
    }
};

// Method: Arity 6 -> 5.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6
>
struct Invoker2<StorageType, R (T::*)(X1, X2, X3, X4, X5, X6)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            x2, x3, x4, x5, x6);
    }
};

// Function: Arity 8 -> 6.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7,
    typename X8
>
struct Invoker2<StorageType, R (*)(X1, X2, X3, X4, X5, X6, X7, X8)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6,
        typename internal::ParamTraits<X7>::ForwardType x7,
        typename internal::ParamTraits<X8>::ForwardType x8)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            x3, x4, x5, x6, x7, x8);
    }
};

// Method: Arity 7 -> 6.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7
>
struct Invoker2<StorageType, R (T::*)(X1, X2, X3, X4, X5, X6, X7)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X2>::ForwardType x2,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6,
        typename internal::ParamTraits<X7>::ForwardType x7)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            x2, x3, x4, x5, x6, x7);
    }
};

template <typename StorageType, typename NormalizedSignature>
struct Invoker3;

// Function: Arity 3 -> 0.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3
>
struct Invoker3<StorageType, R (*)(X1, X2, X3)>
{
    static R DoInvoke(InvokerStorageBase* base)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3));
    }
};

// Method: Arity 2 -> 0.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2
>
struct Invoker3<StorageType, R (T::*)(X1, X2)>
{
    static R DoInvoke(InvokerStorageBase* base)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3));
    }
};

// Function: Arity 4 -> 1.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4
>
struct Invoker3<StorageType, R (*)(X1, X2, X3, X4)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X4>::ForwardType x4)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), x4);
    }
};

// Method: Arity 3 -> 1.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3
>
struct Invoker3<StorageType, R (T::*)(X1, X2, X3)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X3>::ForwardType x3)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), x3);
    }
};

// Function: Arity 5 -> 2.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5
>
struct Invoker3<StorageType, R (*)(X1, X2, X3, X4, X5)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), x4, x5);
    }
};

// Method: Arity 4 -> 2.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4
>
struct Invoker3<StorageType, R (T::*)(X1, X2, X3, X4)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), x3, x4);
    }
};

// Function: Arity 6 -> 3.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6
>
struct Invoker3<StorageType, R (*)(X1, X2, X3, X4, X5, X6)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), x4, x5, x6);
    }
};

// Method: Arity 5 -> 3.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5
>
struct Invoker3<StorageType, R (T::*)(X1, X2, X3, X4, X5)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), x3, x4, x5);
    }
};

// Function: Arity 7 -> 4.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7
>
struct Invoker3<StorageType, R (*)(X1, X2, X3, X4, X5, X6, X7)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6,
        typename internal::ParamTraits<X7>::ForwardType x7)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), x4, x5, x6, x7);
    }
};

// Method: Arity 6 -> 4.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6
>
struct Invoker3<StorageType, R (T::*)(X1, X2, X3, X4, X5, X6)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), x3, x4, x5, x6);
    }
};

// Function: Arity 8 -> 5.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7,
    typename X8
>
struct Invoker3<StorageType, R (*)(X1, X2, X3, X4, X5, X6, X7, X8)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6,
        typename internal::ParamTraits<X7>::ForwardType x7,
        typename internal::ParamTraits<X8>::ForwardType x8)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), x4, x5, x6, x7, x8);
    }
};

// Method: Arity 7 -> 5.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7
>
struct Invoker3<StorageType, R (T::*)(X1, X2, X3, X4, X5, X6, X7)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X3>::ForwardType x3,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6,
        typename internal::ParamTraits<X7>::ForwardType x7)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), x3, x4, x5, x6, x7);
    }
};

template <typename StorageType, typename NormalizedSignature>
struct Invoker4;

// Function: Arity 4 -> 0.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4
>
struct Invoker4<StorageType, R (*)(X1, X2, X3, X4)>
{
    static R DoInvoke(InvokerStorageBase* base)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4));
    }
};

// Method: Arity 3 -> 0.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3
>
struct Invoker4<StorageType, R (T::*)(X1, X2, X3)>
{
    static R DoInvoke(InvokerStorageBase* base)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4));
    }
};

// Function: Arity 5 -> 1.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5
>
struct Invoker4<StorageType, R (*)(X1, X2, X3, X4, X5)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X5>::ForwardType x5)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4), x5);
    }
};

// Method: Arity 4 -> 1.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4
>
struct Invoker4<StorageType, R (T::*)(X1, X2, X3, X4)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X4>::ForwardType x4)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4), x4);
    }
};

// Function: Arity 6 -> 2.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6
>
struct Invoker4<StorageType, R (*)(X1, X2, X3, X4, X5, X6)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4), x5, x6);
    }
};

// Method: Arity 5 -> 2.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5
>
struct Invoker4<StorageType, R (T::*)(X1, X2, X3, X4, X5)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4), x4, x5);
    }
};

// Function: Arity 7 -> 3.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7
>
struct Invoker4<StorageType, R (*)(X1, X2, X3, X4, X5, X6, X7)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6,
        typename internal::ParamTraits<X7>::ForwardType x7)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4), x5, x6, x7);
    }
};

// Method: Arity 6 -> 3.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6
>
struct Invoker4<StorageType, R (T::*)(X1, X2, X3, X4, X5, X6)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4), x4, x5, x6);
    }
};

// Function: Arity 8 -> 4.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7,
    typename X8
>
struct Invoker4<StorageType, R (*)(X1, X2, X3, X4, X5, X6, X7, X8)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6,
        typename internal::ParamTraits<X7>::ForwardType x7,
        typename internal::ParamTraits<X8>::ForwardType x8)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4), x5, x6, x7, x8);
    }
};

// Method: Arity 7 -> 4.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7
>
struct Invoker4<StorageType, R (T::*)(X1, X2, X3, X4, X5, X6, X7)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X4>::ForwardType x4,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6,
        typename internal::ParamTraits<X7>::ForwardType x7)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4), x4, x5, x6, x7);
    }
};

template <typename StorageType, typename NormalizedSignature>
struct Invoker5;

// Function: Arity 5 -> 0.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5
>
struct Invoker5<StorageType, R (*)(X1, X2, X3, X4, X5)>
{
    static R DoInvoke(InvokerStorageBase* base)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5));
    }
};

// Method: Arity 4 -> 0.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4
>
struct Invoker5<StorageType, R (T::*)(X1, X2, X3, X4)>
{
    static R DoInvoke(InvokerStorageBase* base)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5));
    }
};

// Function: Arity 6 -> 1.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6
>
struct Invoker5<StorageType, R (*)(X1, X2, X3, X4, X5, X6)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X6>::ForwardType x6)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5), x6);
    }
};

// Method: Arity 5 -> 1.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5
>
struct Invoker5<StorageType, R (T::*)(X1, X2, X3, X4, X5)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X5>::ForwardType x5)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5), x5);
    }
};

// Function: Arity 7 -> 2.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7
>
struct Invoker5<StorageType, R (*)(X1, X2, X3, X4, X5, X6, X7)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X6>::ForwardType x6,
        typename internal::ParamTraits<X7>::ForwardType x7)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5), x6, x7);
    }
};

// Method: Arity 6 -> 2.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6
>
struct Invoker5<StorageType, R (T::*)(X1, X2, X3, X4, X5, X6)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5), x5, x6);
    }
};

// Function: Arity 8 -> 3.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7,
    typename X8
>
struct Invoker5<StorageType, R (*)(X1, X2, X3, X4, X5, X6, X7, X8)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X6>::ForwardType x6,
        typename internal::ParamTraits<X7>::ForwardType x7,
        typename internal::ParamTraits<X8>::ForwardType x8)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5), x6, x7, x8);
    }
};

// Method: Arity 7 -> 3.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7
>
struct Invoker5<StorageType, R (T::*)(X1, X2, X3, X4, X5, X6, X7)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X5>::ForwardType x5,
        typename internal::ParamTraits<X6>::ForwardType x6,
        typename internal::ParamTraits<X7>::ForwardType x7)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5), x5, x6, x7);
    }
};

template <typename StorageType, typename NormalizedSignature>
struct Invoker6;

// Function: Arity 6 -> 0.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6
>
struct Invoker6<StorageType, R (*)(X1, X2, X3, X4, X5, X6)>
{
    static R DoInvoke(InvokerStorageBase* base)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5), Unwrap(invoker->m_p6));
    }
};

// Method: Arity 5 -> 0.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5
>
struct Invoker6<StorageType, R (T::*)(X1, X2, X3, X4, X5)>
{
    static R DoInvoke(InvokerStorageBase* base)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5), Unwrap(invoker->m_p6));
    }
};

// Function: Arity 7 -> 1.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7
>
struct Invoker6<StorageType, R (*)(X1, X2, X3, X4, X5, X6, X7)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X7>::ForwardType x7)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5), Unwrap(invoker->m_p6), x7);
    }
};

// Method: Arity 6 -> 1.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6
>
struct Invoker6<StorageType, R (T::*)(X1, X2, X3, X4, X5, X6)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X6>::ForwardType x6)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5), Unwrap(invoker->m_p6), x6);
    }
};

// Function: Arity 8 -> 2.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7,
    typename X8
>
struct Invoker6<StorageType, R (*)(X1, X2, X3, X4, X5, X6, X7, X8)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X7>::ForwardType x7,
        typename internal::ParamTraits<X8>::ForwardType x8)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5), Unwrap(invoker->m_p6), x7, x8);
    }
};

// Method: Arity 7 -> 2.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7
>
struct Invoker6<StorageType, R (T::*)(X1, X2, X3, X4, X5, X6, X7)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X6>::ForwardType x6,
        typename internal::ParamTraits<X7>::ForwardType x7)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5), Unwrap(invoker->m_p6), x6, x7);
    }
};

template <typename StorageType, typename NormalizedSignature>
struct Invoker7;

// Function: Arity 7 -> 0.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7
>
struct Invoker7<StorageType, R (*)(X1, X2, X3, X4, X5, X6, X7)>
{
    static R DoInvoke(InvokerStorageBase* base)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5), Unwrap(invoker->m_p6),
            Unwrap(invoker->m_p7));
    }
};

// Method: Arity 6 -> 0.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6
>
struct Invoker7<StorageType, R (T::*)(X1, X2, X3, X4, X5, X6)>
{
    static R DoInvoke(InvokerStorageBase* base)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5), Unwrap(invoker->m_p6),
            Unwrap(invoker->m_p7));
    }
};

// Function: Arity 8 -> 1.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7,
    typename X8
>
struct Invoker7<StorageType, R (*)(X1, X2, X3, X4, X5, X6, X7, X8)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X8>::ForwardType x8)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5), Unwrap(invoker->m_p6),
            Unwrap(invoker->m_p7), x8);
    }
};

// Method: Arity 7 -> 1.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7
>
struct Invoker7<StorageType, R (T::*)(X1, X2, X3, X4, X5, X6, X7)>
{
    static R DoInvoke(InvokerStorageBase* base,
        typename internal::ParamTraits<X7>::ForwardType x7)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5), Unwrap(invoker->m_p6),
            Unwrap(invoker->m_p7), x7);
    }
};

template <typename StorageType, typename NormalizedSignature>
struct Invoker8;

// Function: Arity 8 -> 0.
template <
    typename StorageType,
    typename R, typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7,
    typename X8
>
struct Invoker8<StorageType, R (*)(X1, X2, X3, X4, X5, X6, X7, X8)>
{
    static R DoInvoke(InvokerStorageBase* base)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return invoker->function(Unwrap(invoker->m_p1), Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5), Unwrap(invoker->m_p6),
            Unwrap(invoker->m_p7), Unwrap(invoker->m_p8));
    }
};

// Method: Arity 7 -> 0.
template <
    typename StorageType,
    typename R,
    typename T,
    typename X1,
    typename X2,
    typename X3,
    typename X4,
    typename X5,
    typename X6,
    typename X7
>
struct Invoker8<StorageType, R (T::*)(X1, X2, X3, X4, X5, X6, X7)>
{
    static R DoInvoke(InvokerStorageBase* base)
    {
        StorageType* invoker = static_cast<StorageType*>(base);
        return

            ((*Unwrap(invoker->m_p1)).*invoker->function)(Unwrap(invoker->m_p2),
            Unwrap(invoker->m_p3), Unwrap(invoker->m_p4),
            Unwrap(invoker->m_p5), Unwrap(invoker->m_p6),
            Unwrap(invoker->m_p7), Unwrap(invoker->m_p8));
    }
};

// InvokerStorageN<>
//
// These are the actual storage classes for the Invokers.
//
// Though these types are "classes", they are being used as structs with
// all member variable public.  We cannot make it a struct because it inherits
// from a class which causes a compiler warning.  We cannot add a "Run()" method
// that forwards the unbound arguments because that would require we unwrap the
// Signature type like in InvokerN above to know the return type, and the arity
// of Run().
//
// An alternate solution would be to merge InvokerN and InvokerStorageN,
// but the generated code seemed harder to read.

template <
    typename Signature

>
class InvokerStorage0 : public InvokerStorageBase
{
public:
    typedef InvokerStorage0 StorageType;
    typedef FunctionTraits<Signature> TargetTraits;
    typedef Invoker0<StorageType,
        typename TargetTraits::NormalizedSignature> Invoker;
    typedef typename TargetTraits::IsMethod IsMethod;

    explicit InvokerStorage0(const Signature& f)
        : function(f)    {
    }

    virtual ~InvokerStorage0()
    {    }

    Signature function;
};

template <
    typename Signature,
    typename P1
>
class InvokerStorage1 : public InvokerStorageBase
{
public:
    typedef InvokerStorage1 StorageType;
    typedef FunctionTraits<Signature> TargetTraits;
    typedef Invoker1<StorageType,
        typename TargetTraits::NormalizedSignature> Invoker;
    typedef typename TargetTraits::IsMethod IsMethod;

    // For methods, we need to be careful for parameter 1, the this pointer of
    // the method.
    // We skip the scoped_refptr check because the binder itself takes care of
    // this.
    COMMON_STATIC_ASSERT(IsMethod::Value ||
                 !internal::IsRefCountedType<P1>::Value,
                 "p1 is refcounted type, should be passed by scoped_refptr");

    // We also disallow binding of an array as the method's target object.
    COMMON_STATIC_ASSERT(!IsMethod::Value || !TypeTraits::IsArray<P1>::Value,
                 "first bound argument to method cannot be array");
    InvokerStorage1(const Signature& f, const P1& p1)
        : function(f),
            m_p1(static_cast<typename ParamTraits<P1>::StorageType>(p1))    {
        MaybeRefCount<IsMethod, P1>::AddRef(m_p1);
    }

    virtual ~InvokerStorage1()
    {
        MaybeRefCount<IsMethod, P1>::Release(m_p1);
    }

    Signature function;
    typename ParamTraits<P1>::StorageType m_p1;
};

template <
    typename Signature,
    typename P1,
    typename P2
>
class InvokerStorage2 : public InvokerStorageBase
{
public:
    typedef InvokerStorage2 StorageType;
    typedef FunctionTraits<Signature> TargetTraits;
    typedef Invoker2<StorageType,
        typename TargetTraits::NormalizedSignature> Invoker;
    typedef typename TargetTraits::IsMethod IsMethod;

    // For methods, we need to be careful for parameter 1, the this pointer of
    // the method.
    // We skip the scoped_refptr check because the binder itself takes care of
    // this.
    COMMON_STATIC_ASSERT(IsMethod::Value ||
                 !internal::IsRefCountedType<P1>::Value,
                 "p1 is refcounted type, should be passed by scoped_refptr");

    // We also disallow binding of an array as the method's target object.
    COMMON_STATIC_ASSERT(!IsMethod::Value || !TypeTraits::IsArray<P1>::Value,
                 "first bound argument to method cannot be array");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P2>::Value,
                 "p2 is refcounted type, should be passed by scoped_refptr");
    InvokerStorage2(const Signature& f, const P1& p1, const P2& p2)
        : function(f),
            m_p1(static_cast<typename ParamTraits<P1>::StorageType>(p1)),
            m_p2(static_cast<typename ParamTraits<P2>::StorageType>(p2))    {
        MaybeRefCount<IsMethod, P1>::AddRef(m_p1);
    }

    virtual ~InvokerStorage2()
    {
        MaybeRefCount<IsMethod, P1>::Release(m_p1);
    }

    Signature function;
    typename ParamTraits<P1>::StorageType m_p1;
    typename ParamTraits<P2>::StorageType m_p2;
};

template <
    typename Signature,
    typename P1,
    typename P2,
    typename P3
>
class InvokerStorage3 : public InvokerStorageBase
{
public:
    typedef InvokerStorage3 StorageType;
    typedef FunctionTraits<Signature> TargetTraits;
    typedef Invoker3<StorageType,
        typename TargetTraits::NormalizedSignature> Invoker;
    typedef typename TargetTraits::IsMethod IsMethod;

    // For methods, we need to be careful for parameter 1, the this pointer of
    // the method.
    // We skip the scoped_refptr check because the binder itself takes care of
    // this.
    COMMON_STATIC_ASSERT(IsMethod::Value ||
                 !internal::IsRefCountedType<P1>::Value,
                 "p1 is refcounted type, should be passed by scoped_refptr");

    // We also disallow binding of an array as the method's target object.
    COMMON_STATIC_ASSERT(!IsMethod::Value || !TypeTraits::IsArray<P1>::Value,
                 "first bound argument to method cannot be array");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P2>::Value,
                 "p2 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P3>::Value,
                 "p3 is refcounted type, should be passed by scoped_refptr");
    InvokerStorage3(const Signature& f, const P1& p1, const P2& p2,
        const P3& p3)
        : function(f),
            m_p1(static_cast<typename ParamTraits<P1>::StorageType>(p1)),
            m_p2(static_cast<typename ParamTraits<P2>::StorageType>(p2)),
            m_p3(static_cast<typename ParamTraits<P3>::StorageType>(p3))    {
        MaybeRefCount<IsMethod, P1>::AddRef(m_p1);
    }

    virtual ~InvokerStorage3()
    {
        MaybeRefCount<IsMethod, P1>::Release(m_p1);
    }

    Signature function;
    typename ParamTraits<P1>::StorageType m_p1;
    typename ParamTraits<P2>::StorageType m_p2;
    typename ParamTraits<P3>::StorageType m_p3;
};

template <
    typename Signature,
    typename P1,
    typename P2,
    typename P3,
    typename P4
>
class InvokerStorage4 : public InvokerStorageBase
{
public:
    typedef InvokerStorage4 StorageType;
    typedef FunctionTraits<Signature> TargetTraits;
    typedef Invoker4<StorageType,
        typename TargetTraits::NormalizedSignature> Invoker;
    typedef typename TargetTraits::IsMethod IsMethod;

    // For methods, we need to be careful for parameter 1, the this pointer of
    // the method.
    // We skip the scoped_refptr check because the binder itself takes care of
    // this.
    COMMON_STATIC_ASSERT(IsMethod::Value ||
                 !internal::IsRefCountedType<P1>::Value,
                 "p1 is refcounted type, should be passed by scoped_refptr");

    // We also disallow binding of an array as the method's target object.
    COMMON_STATIC_ASSERT(!IsMethod::Value || !TypeTraits::IsArray<P1>::Value,
                 "first bound argument to method cannot be array");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P2>::Value,
                 "p2 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P3>::Value,
                 "p3 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P4>::Value,
                 "p4 is refcounted type, should be passed by scoped_refptr");
    InvokerStorage4(const Signature& f, const P1& p1, const P2& p2,
        const P3& p3, const P4& p4)
        : function(f),
            m_p1(static_cast<typename ParamTraits<P1>::StorageType>(p1)),
            m_p2(static_cast<typename ParamTraits<P2>::StorageType>(p2)),
            m_p3(static_cast<typename ParamTraits<P3>::StorageType>(p3)),
            m_p4(static_cast<typename ParamTraits<P4>::StorageType>(p4))    {
        MaybeRefCount<IsMethod, P1>::AddRef(m_p1);
    }

    virtual ~InvokerStorage4()
    {
        MaybeRefCount<IsMethod, P1>::Release(m_p1);
    }

    Signature function;
    typename ParamTraits<P1>::StorageType m_p1;
    typename ParamTraits<P2>::StorageType m_p2;
    typename ParamTraits<P3>::StorageType m_p3;
    typename ParamTraits<P4>::StorageType m_p4;
};

template <
    typename Signature,
    typename P1,
    typename P2,
    typename P3,
    typename P4,
    typename P5
>
class InvokerStorage5 : public InvokerStorageBase
{
public:
    typedef InvokerStorage5 StorageType;
    typedef FunctionTraits<Signature> TargetTraits;
    typedef Invoker5<StorageType,
        typename TargetTraits::NormalizedSignature> Invoker;
    typedef typename TargetTraits::IsMethod IsMethod;

    // For methods, we need to be careful for parameter 1, the this pointer of
    // the method.
    // We skip the scoped_refptr check because the binder itself takes care of
    // this.
    COMMON_STATIC_ASSERT(IsMethod::Value ||
                 !internal::IsRefCountedType<P1>::Value,
                 "p1 is refcounted type, should be passed by scoped_refptr");

    // We also disallow binding of an array as the method's target object.
    COMMON_STATIC_ASSERT(!IsMethod::Value || !TypeTraits::IsArray<P1>::Value,
                 "first bound argument to method cannot be array");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P2>::Value,
                 "p2 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P3>::Value,
                 "p3 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P4>::Value,
                 "p4 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P5>::Value,
                 "p5 is refcounted type, should be passed by scoped_refptr");
    InvokerStorage5(const Signature& f, const P1& p1, const P2& p2,
        const P3& p3, const P4& p4, const P5& p5)
        : function(f),
            m_p1(static_cast<typename ParamTraits<P1>::StorageType>(p1)),
            m_p2(static_cast<typename ParamTraits<P2>::StorageType>(p2)),
            m_p3(static_cast<typename ParamTraits<P3>::StorageType>(p3)),
            m_p4(static_cast<typename ParamTraits<P4>::StorageType>(p4)),
            m_p5(static_cast<typename ParamTraits<P5>::StorageType>(p5))    {
        MaybeRefCount<IsMethod, P1>::AddRef(m_p1);
    }

    virtual ~InvokerStorage5()
    {
        MaybeRefCount<IsMethod, P1>::Release(m_p1);
    }

    Signature function;
    typename ParamTraits<P1>::StorageType m_p1;
    typename ParamTraits<P2>::StorageType m_p2;
    typename ParamTraits<P3>::StorageType m_p3;
    typename ParamTraits<P4>::StorageType m_p4;
    typename ParamTraits<P5>::StorageType m_p5;
};

template <
    typename Signature,
    typename P1,
    typename P2,
    typename P3,
    typename P4,
    typename P5,
    typename P6
>
class InvokerStorage6 : public InvokerStorageBase
{
public:
    typedef InvokerStorage6 StorageType;
    typedef FunctionTraits<Signature> TargetTraits;
    typedef Invoker6<StorageType,
        typename TargetTraits::NormalizedSignature> Invoker;
    typedef typename TargetTraits::IsMethod IsMethod;

    // For methods, we need to be careful for parameter 1, the this pointer of
    // the method.
    // We skip the scoped_refptr check because the binder itself takes care of
    // this.
    COMMON_STATIC_ASSERT(IsMethod::Value ||
                 !internal::IsRefCountedType<P1>::Value,
                 "p1 is refcounted type, should be passed by scoped_refptr");

    // We also disallow binding of an array as the method's target object.
    COMMON_STATIC_ASSERT(!IsMethod::Value || !TypeTraits::IsArray<P1>::Value,
                 "first bound argument to method cannot be array");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P2>::Value,
                 "p2 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P3>::Value,
                 "p3 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P4>::Value,
                 "p4 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P5>::Value,
                 "p5 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P6>::Value,
                 "p6 is refcounted type, should be passed by scoped_refptr");
    InvokerStorage6(const Signature& f, const P1& p1, const P2& p2,
        const P3& p3, const P4& p4, const P5& p5, const P6& p6)
        : function(f),
            m_p1(static_cast<typename ParamTraits<P1>::StorageType>(p1)),
            m_p2(static_cast<typename ParamTraits<P2>::StorageType>(p2)),
            m_p3(static_cast<typename ParamTraits<P3>::StorageType>(p3)),
            m_p4(static_cast<typename ParamTraits<P4>::StorageType>(p4)),
            m_p5(static_cast<typename ParamTraits<P5>::StorageType>(p5)),
            m_p6(static_cast<typename ParamTraits<P6>::StorageType>(p6))    {
        MaybeRefCount<IsMethod, P1>::AddRef(m_p1);
    }

    virtual ~InvokerStorage6()
    {
        MaybeRefCount<IsMethod, P1>::Release(m_p1);
    }

    Signature function;
    typename ParamTraits<P1>::StorageType m_p1;
    typename ParamTraits<P2>::StorageType m_p2;
    typename ParamTraits<P3>::StorageType m_p3;
    typename ParamTraits<P4>::StorageType m_p4;
    typename ParamTraits<P5>::StorageType m_p5;
    typename ParamTraits<P6>::StorageType m_p6;
};

template <
    typename Signature,
    typename P1,
    typename P2,
    typename P3,
    typename P4,
    typename P5,
    typename P6,
    typename P7
>
class InvokerStorage7 : public InvokerStorageBase
{
public:
    typedef InvokerStorage7 StorageType;
    typedef FunctionTraits<Signature> TargetTraits;
    typedef Invoker7<StorageType,
        typename TargetTraits::NormalizedSignature> Invoker;
    typedef typename TargetTraits::IsMethod IsMethod;

    // For methods, we need to be careful for parameter 1, the this pointer of
    // the method.
    // We skip the scoped_refptr check because the binder itself takes care of
    // this.
    COMMON_STATIC_ASSERT(IsMethod::Value ||
                 !internal::IsRefCountedType<P1>::Value,
                 "p1 is refcounted type, should be passed by scoped_refptr");

    // We also disallow binding of an array as the method's target object.
    COMMON_STATIC_ASSERT(!IsMethod::Value || !TypeTraits::IsArray<P1>::Value,
                 "first bound argument to method cannot be array");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P2>::Value,
                 "p2 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P3>::Value,
                 "p3 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P4>::Value,
                 "p4 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P5>::Value,
                 "p5 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P6>::Value,
                 "p6 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P7>::Value,
                 "p7 is refcounted type, should be passed by scoped_refptr");
    InvokerStorage7(const Signature& f, const P1& p1, const P2& p2,
        const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7)
        : function(f),
            m_p1(static_cast<typename ParamTraits<P1>::StorageType>(p1)),
            m_p2(static_cast<typename ParamTraits<P2>::StorageType>(p2)),
            m_p3(static_cast<typename ParamTraits<P3>::StorageType>(p3)),
            m_p4(static_cast<typename ParamTraits<P4>::StorageType>(p4)),
            m_p5(static_cast<typename ParamTraits<P5>::StorageType>(p5)),
            m_p6(static_cast<typename ParamTraits<P6>::StorageType>(p6)),
            m_p7(static_cast<typename ParamTraits<P7>::StorageType>(p7))    {
        MaybeRefCount<IsMethod, P1>::AddRef(m_p1);
    }

    virtual ~InvokerStorage7()
    {
        MaybeRefCount<IsMethod, P1>::Release(m_p1);
    }

    Signature function;
    typename ParamTraits<P1>::StorageType m_p1;
    typename ParamTraits<P2>::StorageType m_p2;
    typename ParamTraits<P3>::StorageType m_p3;
    typename ParamTraits<P4>::StorageType m_p4;
    typename ParamTraits<P5>::StorageType m_p5;
    typename ParamTraits<P6>::StorageType m_p6;
    typename ParamTraits<P7>::StorageType m_p7;
};

template <
    typename Signature,
    typename P1,
    typename P2,
    typename P3,
    typename P4,
    typename P5,
    typename P6,
    typename P7,
    typename P8
>
class InvokerStorage8 : public InvokerStorageBase
{
public:
    typedef InvokerStorage8 StorageType;
    typedef FunctionTraits<Signature> TargetTraits;
    typedef Invoker8<StorageType,
        typename TargetTraits::NormalizedSignature> Invoker;
    typedef typename TargetTraits::IsMethod IsMethod;

    // For methods, we need to be careful for parameter 1, the this pointer of
    // the method.
    // We skip the scoped_refptr check because the binder itself takes care of
    // this.
    COMMON_STATIC_ASSERT(IsMethod::Value ||
                 !internal::IsRefCountedType<P1>::Value,
                 "p1 is refcounted type, should be passed by scoped_refptr");

    // We also disallow binding of an array as the method's target object.
    COMMON_STATIC_ASSERT(!IsMethod::Value || !TypeTraits::IsArray<P1>::Value,
                 "first bound argument to method cannot be array");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P2>::Value,
                 "p2 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P3>::Value,
                 "p3 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P4>::Value,
                 "p4 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P5>::Value,
                 "p5 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P6>::Value,
                 "p6 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P7>::Value,
                 "p7 is refcounted type, should be passed by scoped_refptr");
    COMMON_STATIC_ASSERT(!internal::IsRefCountedType<P8>::Value,
                 "p8 is refcounted type, should be passed by scoped_refptr");
    InvokerStorage8(const Signature& f, const P1& p1, const P2& p2,
        const P3& p3, const P4& p4, const P5& p5, const P6& p6, const P7& p7,
        const P8& p8)
        : function(f),
            m_p1(static_cast<typename ParamTraits<P1>::StorageType>(p1)),
            m_p2(static_cast<typename ParamTraits<P2>::StorageType>(p2)),
            m_p3(static_cast<typename ParamTraits<P3>::StorageType>(p3)),
            m_p4(static_cast<typename ParamTraits<P4>::StorageType>(p4)),
            m_p5(static_cast<typename ParamTraits<P5>::StorageType>(p5)),
            m_p6(static_cast<typename ParamTraits<P6>::StorageType>(p6)),
            m_p7(static_cast<typename ParamTraits<P7>::StorageType>(p7)),
            m_p8(static_cast<typename ParamTraits<P8>::StorageType>(p8))    {
        MaybeRefCount<IsMethod, P1>::AddRef(m_p1);
    }

    virtual ~InvokerStorage8()
    {
        MaybeRefCount<IsMethod, P1>::Release(m_p1);
    }

    Signature function;
    typename ParamTraits<P1>::StorageType m_p1;
    typename ParamTraits<P2>::StorageType m_p2;
    typename ParamTraits<P3>::StorageType m_p3;
    typename ParamTraits<P4>::StorageType m_p4;
    typename ParamTraits<P5>::StorageType m_p5;
    typename ParamTraits<P6>::StorageType m_p6;
    typename ParamTraits<P7>::StorageType m_p7;
    typename ParamTraits<P8>::StorageType m_p8;
};

}  // namespace internal

} // end of namespace common

#endif // COMMON_BASE_FUNCTION_INVOKER_STORAGE_H
