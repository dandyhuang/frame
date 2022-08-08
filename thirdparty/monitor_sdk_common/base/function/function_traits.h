//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-14 13:39
//  @file:      function_traits.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_FUNCTION_FUNCTION_TRAITS_H
#define COMMON_BASE_FUNCTION_FUNCTION_TRAITS_H
#pragma once

#include "thirdparty/monitor_sdk_common/base/type_traits.h"

// GLOBAL_NOLINT(readability/casting)
// GLOBAL_NOLINT(whitespace/parens)

namespace common {

// forward declaration Function
template <typename Signature>
class Function;

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

// FunctionTraits<>
//
// The FunctionTraits<> template determines the type of function, and also
// creates a NormalizedType used to select the InvokerN classes.  It turns out
// that syntactically, you only really have 2 variations when invoking a
// funciton pointer: normal, and method.  One is invoked func_ptr(arg1). The
// other is invoked (*obj_->method_ptr(arg1)).
//
// However, in the type system, there are many more distinctions. In standard
// C++, there's all variations of const, and volatile on the function pointer.
// In Windows, there are additional calling conventions (eg., __stdcall,
// __fastcall, etc.). FunctionTraits<> handles categorizing each of these into
// a normalized signature.
//
// Having a NormalizedSignature signature, reduces the combinatoric
// complexity of defintions for the InvokerN<> later.  Even though there are
// only 2 syntactic variations on invoking a function, without normalizing the
// signature, there would need to be one specialization of InvokerN for each
// unique (function_type, bound_arg, unbound_args) tuple in order to match all
// function signatures.
//
// By normalizing the function signature, we reduce function_type to exactly 2.

template <typename Signature>
struct FunctionTraits;

// Function: Arity 0.
template <typename R>
struct FunctionTraits<R (*)()>
{
    typedef R (*NormalizedSignature)();
    typedef TypeTraits::FalseType IsMethod;
};

// Method: Arity 0.
template <typename R, typename T>
struct FunctionTraits<R (T::*)()>
{
    typedef R (T::*NormalizedSignature)();
    typedef TypeTraits::TrueType IsMethod;

    // Target type for each bound parameter.
    typedef T B1;
};

// Const Method: Arity 0.
template <typename R, typename T>
struct FunctionTraits<R (T::*)() const>
{
    typedef R (T::*NormalizedSignature)();
    typedef TypeTraits::TrueType IsMethod;
};

#ifdef _WIN32
// __stdcall Function: Arity 0.
template <typename R>
struct FunctionTraits<R (__stdcall *)()>
{
    typedef R (*NormalizedSignature)();
    typedef TypeTraits::FalseType IsMethod;
};

// __fastcall Function: Arity 0.
template <typename R>
struct FunctionTraits<R (__fastcall *)()>
{
    typedef R (*NormalizedSignature)();
    typedef TypeTraits::FalseType IsMethod;
};

#endif // _WIN32

// Function Object: Arity 0.
template <typename R>
struct FunctionTraits<Function<R ()> >
{
    typedef R (*NormalizedSignature)();
    typedef TypeTraits::FalseType IsMethod;
};

// Function: Arity 1.
template <typename R, typename X1>
struct FunctionTraits<R (*)(X1)>
{
    typedef R (*NormalizedSignature)(X1);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
};

// Method: Arity 1.
template <typename R, typename T, typename X1>
struct FunctionTraits<R (T::*)(X1)>
{
    typedef R (T::*NormalizedSignature)(X1);
    typedef TypeTraits::TrueType IsMethod;

    // Target type for each bound parameter.
    typedef T B1;
    typedef X1 B2;
};

// Const Method: Arity 1.
template <typename R, typename T, typename X1>
struct FunctionTraits<R (T::*)(X1) const>
{
    typedef R (T::*NormalizedSignature)(X1);
    typedef TypeTraits::TrueType IsMethod;
};

#ifdef _WIN32
// __stdcall Function: Arity 1.
template <typename R, typename X1>
struct FunctionTraits<R (__stdcall *)(X1)>
{
    typedef R (*NormalizedSignature)(X1);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
};

// __fastcall Function: Arity 1.
template <typename R, typename X1>
struct FunctionTraits<R (__fastcall *)(X1)>
{
    typedef R (*NormalizedSignature)(X1);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
};

#endif // _WIN32

// Function Object: Arity 1.
template <typename R, typename X1>
struct FunctionTraits<Function<R (X1)> >
{
    typedef R (*NormalizedSignature)(X1);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
};

// Function: Arity 2.
template <typename R, typename X1, typename X2>
struct FunctionTraits<R (*)(X1, X2)>
{
    typedef R (*NormalizedSignature)(X1, X2);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
};

// Method: Arity 2.
template <typename R, typename T, typename X1, typename X2>
struct FunctionTraits<R (T::*)(X1, X2)>
{
    typedef R (T::*NormalizedSignature)(X1, X2);
    typedef TypeTraits::TrueType IsMethod;

    // Target type for each bound parameter.
    typedef T B1;
    typedef X1 B2;
    typedef X2 B3;
};

// Const Method: Arity 2.
template <typename R, typename T, typename X1, typename X2>
struct FunctionTraits<R (T::*)(X1, X2) const>
{
    typedef R (T::*NormalizedSignature)(X1, X2);
    typedef TypeTraits::TrueType IsMethod;
};

#ifdef _WIN32
// __stdcall Function: Arity 2.
template <typename R, typename X1, typename X2>
struct FunctionTraits<R (__stdcall *)(X1, X2)>
{
    typedef R (*NormalizedSignature)(X1, X2);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
};

// __fastcall Function: Arity 2.
template <typename R, typename X1, typename X2>
struct FunctionTraits<R (__fastcall *)(X1, X2)>
{
    typedef R (*NormalizedSignature)(X1, X2);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
};

#endif // _WIN32

// Function Object: Arity 2.
template <typename R, typename X1, typename X2>
struct FunctionTraits<Function<R (X1, X2)> >
{
    typedef R (*NormalizedSignature)(X1, X2);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
};

// Function: Arity 3.
template <typename R, typename X1, typename X2, typename X3>
struct FunctionTraits<R (*)(X1, X2, X3)>
{
    typedef R (*NormalizedSignature)(X1, X2, X3);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
};

// Method: Arity 3.
template <typename R, typename T, typename X1, typename X2, typename X3>
struct FunctionTraits<R (T::*)(X1, X2, X3)>
{
    typedef R (T::*NormalizedSignature)(X1, X2, X3);
    typedef TypeTraits::TrueType IsMethod;

    // Target type for each bound parameter.
    typedef T B1;
    typedef X1 B2;
    typedef X2 B3;
    typedef X3 B4;
};

// Const Method: Arity 3.
template <typename R, typename T, typename X1, typename X2, typename X3>
struct FunctionTraits<R (T::*)(X1, X2, X3) const>
{
    typedef R (T::*NormalizedSignature)(X1, X2, X3);
    typedef TypeTraits::TrueType IsMethod;
};

#ifdef _WIN32
// __stdcall Function: Arity 3.
template <typename R, typename X1, typename X2, typename X3>
struct FunctionTraits<R (__stdcall *)(X1, X2, X3)>
{
    typedef R (*NormalizedSignature)(X1, X2, X3);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
};

// __fastcall Function: Arity 3.
template <typename R, typename X1, typename X2, typename X3>
struct FunctionTraits<R (__fastcall *)(X1, X2, X3)>
{
    typedef R (*NormalizedSignature)(X1, X2, X3);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
};

#endif // _WIN32

// Function Object: Arity 3.
template <typename R, typename X1, typename X2, typename X3>
struct FunctionTraits<Function<R (X1, X2, X3)> >
{
    typedef R (*NormalizedSignature)(X1, X2, X3);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
};

// Function: Arity 4.
template <typename R, typename X1, typename X2, typename X3, typename X4>
struct FunctionTraits<R (*)(X1, X2, X3, X4)>
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
};

// Method: Arity 4.
template <typename R, typename T, typename X1, typename X2, typename X3,
    typename X4>
struct FunctionTraits<R (T::*)(X1, X2, X3, X4)>
{
    typedef R (T::*NormalizedSignature)(X1, X2, X3, X4);
    typedef TypeTraits::TrueType IsMethod;

    // Target type for each bound parameter.
    typedef T B1;
    typedef X1 B2;
    typedef X2 B3;
    typedef X3 B4;
    typedef X4 B5;
};

// Const Method: Arity 4.
template <typename R, typename T, typename X1, typename X2, typename X3,
    typename X4>
struct FunctionTraits<R (T::*)(X1, X2, X3, X4) const>
{
    typedef R (T::*NormalizedSignature)(X1, X2, X3, X4);
    typedef TypeTraits::TrueType IsMethod;
};

#ifdef _WIN32
// __stdcall Function: Arity 4.
template <typename R, typename X1, typename X2, typename X3, typename X4>
struct FunctionTraits<R (__stdcall *)(X1, X2, X3, X4)>
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
};

// __fastcall Function: Arity 4.
template <typename R, typename X1, typename X2, typename X3, typename X4>
struct FunctionTraits<R (__fastcall *)(X1, X2, X3, X4)>
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
};

#endif // _WIN32

// Function Object: Arity 4.
template <typename R, typename X1, typename X2, typename X3, typename X4>
struct FunctionTraits<Function<R (X1, X2, X3, X4)> >
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
};

// Function: Arity 5.
template <typename R, typename X1, typename X2, typename X3, typename X4,
    typename X5>
struct FunctionTraits<R (*)(X1, X2, X3, X4, X5)>
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4, X5);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
    typedef X5 B5;
};

// Method: Arity 5.
template <typename R, typename T, typename X1, typename X2, typename X3,
    typename X4, typename X5>
struct FunctionTraits<R (T::*)(X1, X2, X3, X4, X5)>
{
    typedef R (T::*NormalizedSignature)(X1, X2, X3, X4, X5);
    typedef TypeTraits::TrueType IsMethod;

    // Target type for each bound parameter.
    typedef T B1;
    typedef X1 B2;
    typedef X2 B3;
    typedef X3 B4;
    typedef X4 B5;
    typedef X5 B6;
};

// Const Method: Arity 5.
template <typename R, typename T, typename X1, typename X2, typename X3,
    typename X4, typename X5>
struct FunctionTraits<R (T::*)(X1, X2, X3, X4, X5) const>
{
    typedef R (T::*NormalizedSignature)(X1, X2, X3, X4, X5);
    typedef TypeTraits::TrueType IsMethod;
};

#ifdef _WIN32
// __stdcall Function: Arity 5.
template <typename R, typename X1, typename X2, typename X3, typename X4,
    typename X5>
struct FunctionTraits<R (__stdcall *)(X1, X2, X3, X4, X5)>
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4, X5);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
    typedef X5 B5;
};

// __fastcall Function: Arity 5.
template <typename R, typename X1, typename X2, typename X3, typename X4,
    typename X5>
struct FunctionTraits<R (__fastcall *)(X1, X2, X3, X4, X5)>
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4, X5);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
    typedef X5 B5;
};

#endif // _WIN32

// Function Object: Arity 5.
template <typename R, typename X1, typename X2, typename X3, typename X4,
    typename X5>
struct FunctionTraits<Function<R (X1, X2, X3, X4, X5)> >
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4, X5);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
    typedef X5 B5;
};

// Function: Arity 6.
template <typename R, typename X1, typename X2, typename X3, typename X4,
    typename X5, typename X6>
struct FunctionTraits<R (*)(X1, X2, X3, X4, X5, X6)>
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4, X5, X6);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
    typedef X5 B5;
    typedef X6 B6;
};

// Method: Arity 6.
template <typename R, typename T, typename X1, typename X2, typename X3,
    typename X4, typename X5, typename X6>
struct FunctionTraits<R (T::*)(X1, X2, X3, X4, X5, X6)>
{
    typedef R (T::*NormalizedSignature)(X1, X2, X3, X4, X5, X6);
    typedef TypeTraits::TrueType IsMethod;

    // Target type for each bound parameter.
    typedef T B1;
    typedef X1 B2;
    typedef X2 B3;
    typedef X3 B4;
    typedef X4 B5;
    typedef X5 B6;
    typedef X6 B7;
};

// Const Method: Arity 6.
template <typename R, typename T, typename X1, typename X2, typename X3,
    typename X4, typename X5, typename X6>
struct FunctionTraits<R (T::*)(X1, X2, X3, X4, X5, X6) const>
{
    typedef R (T::*NormalizedSignature)(X1, X2, X3, X4, X5, X6);
    typedef TypeTraits::TrueType IsMethod;
};

#ifdef _WIN32
// __stdcall Function: Arity 6.
template <typename R, typename X1, typename X2, typename X3, typename X4,
    typename X5, typename X6>
struct FunctionTraits<R (__stdcall *)(X1, X2, X3, X4, X5, X6)>
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4, X5, X6);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
    typedef X5 B5;
    typedef X6 B6;
};

// __fastcall Function: Arity 6.
template <typename R, typename X1, typename X2, typename X3, typename X4,
    typename X5, typename X6>
struct FunctionTraits<R (__fastcall *)(X1, X2, X3, X4, X5, X6)>
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4, X5, X6);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
    typedef X5 B5;
    typedef X6 B6;
};

#endif // _WIN32

// Function Object: Arity 6.
template <typename R, typename X1, typename X2, typename X3, typename X4,
    typename X5, typename X6>
struct FunctionTraits<Function<R (X1, X2, X3, X4, X5, X6)> >
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4, X5, X6);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
    typedef X5 B5;
    typedef X6 B6;
};

// Function: Arity 7.
template <typename R, typename X1, typename X2, typename X3, typename X4,
    typename X5, typename X6, typename X7>
struct FunctionTraits<R (*)(X1, X2, X3, X4, X5, X6, X7)>
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4, X5, X6, X7);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
    typedef X5 B5;
    typedef X6 B6;
    typedef X7 B7;
};

// Method: Arity 7.
template <typename R, typename T, typename X1, typename X2, typename X3,
    typename X4, typename X5, typename X6, typename X7>
struct FunctionTraits<R (T::*)(X1, X2, X3, X4, X5, X6, X7)>
{
    typedef R (T::*NormalizedSignature)(X1, X2, X3, X4, X5, X6, X7);
    typedef TypeTraits::TrueType IsMethod;

    // Target type for each bound parameter.
    typedef T B1;
    typedef X1 B2;
    typedef X2 B3;
    typedef X3 B4;
    typedef X4 B5;
    typedef X5 B6;
    typedef X6 B7;
    typedef X7 B8;
};

// Const Method: Arity 7.
template <typename R, typename T, typename X1, typename X2, typename X3,
    typename X4, typename X5, typename X6, typename X7>
struct FunctionTraits<R (T::*)(X1, X2, X3, X4, X5, X6, X7) const>
{
    typedef R (T::*NormalizedSignature)(X1, X2, X3, X4, X5, X6, X7);
    typedef TypeTraits::TrueType IsMethod;
};

#ifdef _WIN32
// __stdcall Function: Arity 7.
template <typename R, typename X1, typename X2, typename X3, typename X4,
    typename X5, typename X6, typename X7>
struct FunctionTraits<R (__stdcall *)(X1, X2, X3, X4, X5, X6, X7)>
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4, X5, X6, X7);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
    typedef X5 B5;
    typedef X6 B6;
    typedef X7 B7;
};

// __fastcall Function: Arity 7.
template <typename R, typename X1, typename X2, typename X3, typename X4,
    typename X5, typename X6, typename X7>
struct FunctionTraits<R (__fastcall *)(X1, X2, X3, X4, X5, X6, X7)>
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4, X5, X6, X7);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
    typedef X5 B5;
    typedef X6 B6;
    typedef X7 B7;
};

#endif // _WIN32

// Function Object: Arity 7.
template <typename R, typename X1, typename X2, typename X3, typename X4,
    typename X5, typename X6, typename X7>
struct FunctionTraits<Function<R (X1, X2, X3, X4, X5, X6, X7)> >
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4, X5, X6, X7);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
    typedef X5 B5;
    typedef X6 B6;
    typedef X7 B7;
};

// Function: Arity 8.
template <typename R, typename X1, typename X2, typename X3, typename X4,
    typename X5, typename X6, typename X7, typename X8>
struct FunctionTraits<R (*)(X1, X2, X3, X4, X5, X6, X7, X8)>
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4, X5, X6, X7, X8);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
    typedef X5 B5;
    typedef X6 B6;
    typedef X7 B7;
    typedef X8 B8;
};

// Method: Arity 8.
template <typename R, typename T, typename X1, typename X2, typename X3,
    typename X4, typename X5, typename X6, typename X7, typename X8>
struct FunctionTraits<R (T::*)(X1, X2, X3, X4, X5, X6, X7, X8)>
{
    typedef R (T::*NormalizedSignature)(X1, X2, X3, X4, X5, X6, X7, X8);
    typedef TypeTraits::TrueType IsMethod;

    // Target type for each bound parameter.
    typedef T B1;
    typedef X1 B2;
    typedef X2 B3;
    typedef X3 B4;
    typedef X4 B5;
    typedef X5 B6;
    typedef X6 B7;
    typedef X7 B8;
    typedef X8 B9;
};

// Const Method: Arity 8.
template <typename R, typename T, typename X1, typename X2, typename X3,
    typename X4, typename X5, typename X6, typename X7, typename X8>
struct FunctionTraits<R (T::*)(X1, X2, X3, X4, X5, X6, X7, X8) const>
{
    typedef R (T::*NormalizedSignature)(X1, X2, X3, X4, X5, X6, X7, X8);
    typedef TypeTraits::TrueType IsMethod;
};

#ifdef _WIN32
// __stdcall Function: Arity 8.
template <typename R, typename X1, typename X2, typename X3, typename X4,
    typename X5, typename X6, typename X7, typename X8>
struct FunctionTraits<R (__stdcall *)(X1, X2, X3, X4, X5, X6, X7, X8)>
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4, X5, X6, X7, X8);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
    typedef X5 B5;
    typedef X6 B6;
    typedef X7 B7;
    typedef X8 B8;
};

// __fastcall Function: Arity 8.
template <typename R, typename X1, typename X2, typename X3, typename X4,
    typename X5, typename X6, typename X7, typename X8>
struct FunctionTraits<R (__fastcall *)(X1, X2, X3, X4, X5, X6, X7, X8)>
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4, X5, X6, X7, X8);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
    typedef X5 B5;
    typedef X6 B6;
    typedef X7 B7;
    typedef X8 B8;
};

#endif // _WIN32

// Function Object: Arity 8.
template <typename R, typename X1, typename X2, typename X3, typename X4,
    typename X5, typename X6, typename X7, typename X8>
struct FunctionTraits<Function<R (X1, X2, X3, X4, X5, X6, X7, X8)> >
{
    typedef R (*NormalizedSignature)(X1, X2, X3, X4, X5, X6, X7, X8);
    typedef TypeTraits::FalseType IsMethod;

    // Target type for each bound parameter.
    typedef X1 B1;
    typedef X2 B2;
    typedef X3 B3;
    typedef X4 B4;
    typedef X5 B5;
    typedef X6 B6;
    typedef X7 B7;
    typedef X8 B8;
};

}  // namespace internal

} // end of namespace common
#endif // COMMON_BASE_FUNCTION_FUNCTION_TRAITS_H
