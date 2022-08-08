//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:54
//  @file:      strict_bool.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_STRICT_BOOL_H
#define COMMON_BASE_STRICT_BOOL_H
#pragma once

#include "thirdparty/monitor_sdk_common/base/static_assert.h"
#include "thirdparty/monitor_sdk_common/base/type_traits.h"

namespace common {

class strict_bool
{
    typedef bool (strict_bool::*SafeBool)() const;

public:
    strict_bool() : m_value(false) {}

    template <typename T>
    strict_bool(T src,
                typename TypeTraits::EnableIf<TypeTraits::IsSame<T, bool>::Value>::Type* dummy = 0) :
        m_value(src)
    {
    }

    strict_bool& operator=(bool src)
    {
        m_value = src;
        return *this;
    }

    // operator SafeBool() const { return m_value ? &strict_bool::operator! : 0; }
    template <typename T>
    operator T() const
    {
        typedef typename TypeTraits::EnableIf<TypeTraits::IsSame<T, bool>::Value>::Type Type;
        return value();
    }

    bool operator!() const
    {
        return !m_value;
    }
    bool value() const
    {
        return m_value;
    }

    template <typename T>
    typename TypeTraits::EnableIf<TypeTraits::IsSame<T, bool>::Value, bool>::Type
    operator==(T rhs)
    {
        return value() == rhs;
    }

    bool operator==(strict_bool rhs) const
    {
        return value() == rhs.value();
    }

    template <typename T>
    typename TypeTraits::EnableIf<TypeTraits::IsSame<T, bool>::Value, bool>::Type
    operator!=(T rhs)
    {
        return value() != rhs;
    }

    bool operator!=(strict_bool rhs) const
    {
        return value() != rhs.value();
    }

private:
    bool m_value;
};

template <typename T>
typename TypeTraits::EnableIf<TypeTraits::IsSame<T, bool>::Value, bool>::Type
operator==(T lhs, const strict_bool& rhs)
{
    return rhs.value() == lhs;
}

template <typename T>
typename TypeTraits::EnableIf<TypeTraits::IsSame<T, bool>::Value, bool>::Type
operator!=(T lhs, const strict_bool& rhs)
{
    return rhs.value() != lhs;
}

} // end of namespace common

#endif // COMMON_BASE_STRICT_BOOL_H
