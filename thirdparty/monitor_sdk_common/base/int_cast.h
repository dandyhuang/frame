// Copyright (c) 2011, Vivo Inc.
// All rights reserved.
//

#ifndef COMMON_BASE_INT_CAST_H
#define COMMON_BASE_INT_CAST_H

#include <assert.h>
#include "thirdparty/monitor_sdk_common/base/static_assert.h"
#include "thirdparty/monitor_sdk_common/base/type_traits.h"

namespace common {

namespace details
{

template <bool FromSigned, bool ToSigned, bool Narrow>
struct Converter { };

// unsigned -> unsigned, not narrow
template <>
struct Converter<false, false, false>
{
    template <typename From, typename To>
    static To Convert(From s)
    {
        return s;
    }
};

// unsigned -> unsigned, narrow
template <>
struct Converter<false, false, true>
{
    template <typename From, typename To>
    static To Convert(From s)
    {
        To t = static_cast<To>(s);
        assert(t == s);
        return t;
    }
};

// signed -> signed, not narrow
template <>
struct Converter<true, true, false>
{
    template <typename From, typename To>
    static To Convert(From s)
    {
        return s;
    }
};

// signed -> signed, narrow
template <>
struct Converter<true, true, true>
{
    template <typename From, typename To>
    static To Convert(From s)
    {
        To t = static_cast<To>(s);
        assert(t == s);
        return t;
    }
};

// signed -> unsigned, not narrow
template <>
struct Converter<true, false, false>
{
    template <typename From, typename To>
    static To Convert(From s)
    {
        assert(s >= 0);
        return static_cast<To>(s);
    }
};

// signed -> unsigned, narrow
template <>
struct Converter<true, false, true>
{
    template <typename From, typename To>
    static To Convert(From s)
    {
        assert(s >= 0);
        To t = static_cast<To>(s);
        From t_as_s = static_cast<From>(t);
        assert(t_as_s == s); // reverse convert and check again
        (void) t_as_s;
        return t;
    }
};

// unsigned -> signed, not narrow
template <>
struct Converter<false, true, false>
{
    template <typename From, typename To>
    static To Convert(From s)
    {
        To t = static_cast<To>(s);
        assert(t >= 0);
        return t;
    }
};

// unsigned -> signed, narrow
template <>
struct Converter<false, true, true>
{
    template <typename From, typename To>
    static To Convert(From s)
    {
        To t = static_cast<To>(s);
        assert(t >= 0);
        From t_as_s = static_cast<From>(t);
        assert(t_as_s == s); // reverse convert and check again
        (void) t_as_s;
        return t;
    }
};

} // namespace details

template <typename To, typename From>
To int_cast(From s)
{
    COMMON_STATIC_ASSERT(TypeTraits::IsInteger<From>::Value, "From type must be integral");
    COMMON_STATIC_ASSERT(TypeTraits::IsInteger<To>::Value, "To type must be integral");
    const bool from_signed = TypeTraits::IsSignedInteger<From>::Value;
    const bool to_signed = TypeTraits::IsSignedInteger<To>::Value;
    const bool narrow = sizeof(To) < sizeof(From);
    return details::Converter<from_signed, to_signed, narrow>::template Convert<From, To>(s);
}

} // namespace common

#endif // COMMON_BASE_INT_CAST_H
