// Copyright (c) 2013, The COMMON Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-28

// GLOBAL_NOLINT(whitespace/newline)

#ifndef COMMON_BASE_TYPE_TRAITS_ENABLE_IF_H
#define COMMON_BASE_TYPE_TRAITS_ENABLE_IF_H
#pragma once

namespace common {
namespace type_traits {

// Define a nested type if some predicate holds.
// Primary template.
template<bool, typename _Tp = void> struct enable_if {};

// Partial specialization for true.
template<typename _Tp>
struct enable_if<true, _Tp> { typedef _Tp type; };

} // namespace type_traits
} // namespace common

#endif // COMMON_BASE_TYPE_TRAITS_ENABLE_IF_H
