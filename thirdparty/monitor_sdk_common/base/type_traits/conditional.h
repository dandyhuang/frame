// Copyright (c) 2013, The COMMON Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-28

// GLOBAL_NOLINT(whitespace/newline)
// GLOBAL_NOLINT(readability/braces)

#ifndef COMMON_BASE_TYPE_TRAITS_CONDITIONAL_H
#define COMMON_BASE_TYPE_TRAITS_CONDITIONAL_H
#pragma once

namespace common {
namespace type_traits {

// Primary template.
/// Define a member typedef @c type to one of two argument types.
template<bool _Cond, typename _Iftrue, typename _Iffalse>
struct conditional { typedef _Iftrue type; };

// Partial specialization for false.
template<typename _Iftrue, typename _Iffalse>
struct conditional<false, _Iftrue, _Iffalse>
{ typedef _Iffalse type; };

} // namespace type_traits
} // namespace common

#endif // COMMON_BASE_TYPE_TRAITS_CONDITIONAL_H
