//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-06-21 19:05
//  @file:      sp_convertible.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_SMART_PTR_SP_CONVERTIBLE_H
#define COMMON_BASE_SMART_PTR_SP_CONVERTIBLE_H

namespace common
{
namespace detail
{

template< class Y, class T > struct sp_convertible
{
    typedef char (&yes) [1];
    typedef char (&no)  [2];

    static yes f( T* );
    static no  f( ... );

    enum _vt { value = sizeof( (f)( static_cast<Y*>(0) ) ) == sizeof(yes) };
};

struct sp_empty
{
};

template< bool > struct sp_enable_if_convertible_impl;

template<> struct sp_enable_if_convertible_impl<true>
{
    typedef sp_empty type;
};

template<> struct sp_enable_if_convertible_impl<false>
{
};

template< class Y, class T > struct sp_enable_if_convertible: public sp_enable_if_convertible_impl< sp_convertible< Y, T >::value >
{
};

} // namespace detail
} // namespace common

#endif // COMMON_BASE_SMART_PTR_SP_CONVERTIBLE_H

