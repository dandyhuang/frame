//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 16:18
//  @file:      type_traits.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_TYPE_TRAITS_H
#define COMMON_BASE_TYPE_TRAITS_H
#pragma once

#include <features.h>

#if __GNUC_PREREQ(4, 1)
#define COMMON_HAS_STD_TR1_TYPE_TRAITS
#endif

#if __GNUC_PREREQ(4, 5) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#define COMMON_HAS_STD_TYPE_TRAITS
#endif

#if defined COMMON_HAS_STD_TYPE_TRAITS
#include "thirdparty/monitor_sdk_common/base/type_traits/std.h"
#elif defined COMMON_HAS_STD_TR1_TYPE_TRAITS
#include "thirdparty/monitor_sdk_common/base/type_traits/tr1.h"
#else
#include "thirdparty/monitor_sdk_common/base/type_traits/missing.h"
#endif

#undef COMMON_HAS_STD_TYPE_TRAITS
#undef COMMON_HAS_STD_TR1_TYPE_TRAITS

#include <stddef.h>
#include "thirdparty/monitor_sdk_common/base/static_assert.h"

// GLOBAL_NOLINT(runtime/int)
// GLOBAL_NOLINT(whitespace/newline)

namespace common {

namespace TypeTraits
{

template<typename T, T v>
struct IntegralConstant {
    static const T Value = v;
    typedef T ValueType;
    typedef IntegralConstant<T, v> Type;
};

template <typename T, T v> const T IntegralConstant<T, v>::Value;

typedef IntegralConstant<bool, true> TrueType;
typedef IntegralConstant<bool, false> FalseType;

template<typename> struct IsArray : public FalseType {};
template<typename T, size_t n> struct IsArray<T[n]> : public TrueType {};
template<typename T> struct IsArray<T[]> : public TrueType {};

namespace internal {

// Types YesType and NoType are guaranteed such that sizeof(YesType) <
// sizeof(NoType).
typedef char YesType;

struct NoType {
    YesType dummy[2];
};

// This class is an implementation detail for is_convertible, and you
// don't need to know how it works to use is_convertible. For those
// who care: we declare two different functions, one whose argument is
// of type To and one with a variadic argument list. We give them
// return types of different size, so we can use sizeof to trick the
// compiler into telling us which function it would have chosen if we
// had called it with an argument of type From.  See Alexandrescu's
// _Modern C++ Design_ for more details on this sort of trick.

struct ConvertHelper {
    template <typename To>
    static YesType Test(To);

    template <typename To>
    static NoType Test(...);

    template <typename From>
    static From Create();
};

// Used to determine if a type is a struct/union/class. Inspired by Boost's
// is_class type_trait implementation.
struct IsClassHelper {
    template <typename C>
    static YesType Test(void(C::*)(void));

    template <typename C>
    static NoType Test(...);
};

}  // namespace internal

// Inherits from true_type if From is convertible to To, false_type otherwise.
//
// Note that if the type is convertible, this will be a true_type REGARDLESS
// of whether or not the conversion would emit a warning.
template <typename From, typename To>
struct IsConvertible :
    IntegralConstant<bool,
        sizeof(internal::ConvertHelper::Test<To>(internal::ConvertHelper::Create<From>())) ==
        sizeof(internal::YesType)>
{
};

template <typename T>
struct IsClass :
    IntegralConstant<bool,
        sizeof(internal::IsClassHelper::Test<T>(0)) ==
        sizeof(internal::YesType)>
{
};

/// 去掉类型的 const 和引用属性
template <typename T> struct RemoveCvRef { typedef T Type; };
template <typename T> struct RemoveCvRef<const T> { typedef T Type; };
template <typename T> struct RemoveCvRef<volatile T> { typedef T Type; };
template <typename T> struct RemoveCvRef<const volatile T> { typedef T Type; };

// 判断类型是不是整数类型
template <typename T> struct IsInteger { static const bool Value = false; };
template <> struct IsInteger<char>               { static const bool Value = true; };
template <> struct IsInteger<signed char>        { static const bool Value = true; };
template <> struct IsInteger<unsigned char>      { static const bool Value = true; };
template <> struct IsInteger<short>              { static const bool Value = true; };
template <> struct IsInteger<unsigned short>     { static const bool Value = true; };
template <> struct IsInteger<int>                { static const bool Value = true; };
template <> struct IsInteger<unsigned int>       { static const bool Value = true; };
template <> struct IsInteger<long>               { static const bool Value = true; };
template <> struct IsInteger<unsigned long>      { static const bool Value = true; };
template <> struct IsInteger<long long>          { static const bool Value = true; };
template <> struct IsInteger<unsigned long long> { static const bool Value = true; };

// 判断类型是不是浮点数类型
template <typename T> struct IsFloat      { static const bool Value = false; };
template <> struct IsFloat<float>       { static const bool Value = true; };
template <> struct IsFloat<double>      { static const bool Value = true; };
template <> struct IsFloat<long double> { static const bool Value = true; };

// 判断类型是不是数值类型
template <typename T> struct IsNumberic
{
    static const bool Value = IsInteger<T>::Value || IsFloat<T>::Value;
};

// 判断类型是不是指针
template <typename T> struct IsPointer { static const bool Value = false; };
template <typename T> struct IsPointer<T*> { static const bool Value = true; };

// 判断类型是不是引用
template <typename T> struct IsReference { static const bool Value = false; };
template <typename T> struct IsReference<T&> { static const bool Value = true; };

/// 判断一个类是不是从另一个类派生出来的
template <typename Derived, typename Base>
struct IsDerivedFrom
{
    static const bool Value = sizeof(Test(static_cast<Derived*>(0))) == 1;
private:
    // 如果 Derived 是 Base 的派生类，那么能匹配前者，返回类型为 char，否则匹配后者
    static char Test(Base* p);
    static int Test(void* p);
};

///////////////////////////////////////////////////////////////////////////////
// Remove sign of integral types
template <typename T> struct RemoveSign {};

// char type is not integral, support it as an extension
template <> struct RemoveSign<char> { typedef unsigned char Type; };

// signed
template <> struct RemoveSign<signed char> { typedef unsigned char Type; };
template <> struct RemoveSign<short>       { typedef unsigned short Type; };
template <> struct RemoveSign<int>         { typedef unsigned int Type; };
template <> struct RemoveSign<long>        { typedef unsigned long Type; };
template <> struct RemoveSign<long long>   { typedef unsigned long long Type; };

// handle unsigned
template <> struct RemoveSign<unsigned char>       { typedef unsigned char Type; };
template <> struct RemoveSign<unsigned short>      { typedef unsigned short Type; };
template <> struct RemoveSign<unsigned int>        { typedef unsigned int Type; };
template <> struct RemoveSign<unsigned long>       { typedef unsigned long Type; };
template <> struct RemoveSign<unsigned long long>  { typedef unsigned long long Type; };


///////////////////////////////////////////////////////////////////////////////
// Add sign of integral types
template <typename T> struct AddSign {};

// char type is not integral, support it as an extension
template <> struct AddSign<char> { typedef signed char Type; };

// handle unsigned
template <> struct AddSign<unsigned char>       { typedef signed char Type; };
template <> struct AddSign<unsigned short>      { typedef short Type; };
template <> struct AddSign<unsigned int>        { typedef int Type; };
template <> struct AddSign<unsigned long>       { typedef long Type; };
template <> struct AddSign<unsigned long long>  { typedef long long Type; };

// signed
template <> struct AddSign<signed char> { typedef signed char Type; };
template <> struct AddSign<short>       { typedef short Type; };
template <> struct AddSign<int>         { typedef int Type; };
template <> struct AddSign<long>        { typedef long Type; };
template <> struct AddSign<long long>   { typedef long long Type; };

template <typename T> struct IsSignedInteger
{
    COMMON_STATIC_ASSERT(IsInteger<T>::Value);
    static const bool Value = !(T(-1) > 0);
};

template <typename T> struct IsUnsignedInteger
{
    COMMON_STATIC_ASSERT(IsInteger<T>::Value);
    static const bool Value = T(-1) > 0;
};

// whether tow types are same
template<typename A, typename B>
struct IsSame
{
    static const bool Value = false;
};

template<typename A>
struct IsSame<A, A>
{
    static const bool Value = true;
};

// Define a nested type if some predicate holds.
template<bool, typename T = void>
struct EnableIf
{
};

template<typename T>
struct EnableIf<true, T>
{
    typedef T Type;
};

} // end of namespace TypeTraits

} // end of namespace common

#endif // COMMON_BASE_TYPE_TRAITS_H

