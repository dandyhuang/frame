// Copyright (c) 2013, The COMMON Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-28

// GLOBAL_NOLINT(whitespace/newline)
// GLOBAL_NOLINT(readability/braces)

#ifndef COMMON_BASE_TYPE_TRAITS_REMOVE_H
#define COMMON_BASE_TYPE_TRAITS_REMOVE_H
#pragma once

namespace common {
namespace type_traits {

// const-volatile modifications.

/// remove_const
template<typename _Tp>
struct remove_const
{ typedef _Tp     type; };

template<typename _Tp>
struct remove_const<_Tp const>
{ typedef _Tp     type; };

/// remove_volatile
template<typename _Tp>
struct remove_volatile
{ typedef _Tp     type; };

template<typename _Tp>
struct remove_volatile<_Tp volatile>
{ typedef _Tp     type; };

/// remove_cv
template<typename _Tp>
struct remove_cv
{
    typedef typename
        remove_const<typename remove_volatile<_Tp>::type>::type     type;
};

} // namespace type_traits
} // namespace common

#endif // COMMON_BASE_TYPE_TRAITS_REMOVE_H
