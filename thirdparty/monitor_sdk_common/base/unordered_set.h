// Copyright (c) 2013, The COMMON Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef COMMON_BASE_UNORDERED_SET_H
#define COMMON_BASE_UNORDERED_SET_H
#pragma once

#ifdef COMMON_CXX11_ENABLED

// Using std::unordered_*
#include <unordered_set>

#else // Non-C++11

#include <tr1/unordered_set>

namespace std {
using ::std::tr1::unordered_set;
using ::std::tr1::unordered_multiset;
} // namespace std

#endif // COMMON_CXX11_ENABLED

#endif // COMMON_BASE_UNORDERED_SET_H
