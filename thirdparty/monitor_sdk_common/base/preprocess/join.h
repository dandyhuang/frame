// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-03-02

#ifndef COMMON_BASE_PREPROCESS_JOIN_H
#define COMMON_BASE_PREPROCESS_JOIN_H
#pragma once

/// Helper macro to join 2 tokens
/// example: COMMON_PP_JOIN(UCHAR_MAX, SCHAR_MIN) -> 255(-128)
/// The following piece of macro magic joins the two
/// arguments together, even when one of the arguments is
/// itself a macro (see 16.3.1 in C++ standard). The key
/// is that macro expansion of macro arguments does not
/// occur in COMMON_PP_DO_JOIN2 but does in COMMON_PP_DO_JOIN.
#define COMMON_PP_JOIN(X, Y) COMMON_PP_DO_JOIN(X, Y)
#define COMMON_PP_DO_JOIN(X, Y) COMMON_PP_DO_JOIN2(X, Y)
#define COMMON_PP_DO_JOIN2(X, Y) X##Y

#endif // COMMON_BASE_PREPROCESS_JOIN_H
