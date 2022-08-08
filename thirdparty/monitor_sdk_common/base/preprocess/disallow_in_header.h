// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-03-02
// Description:

#ifndef COMMON_BASE_PREPROCESS_DISALLOW_IN_HEADER_H
#define COMMON_BASE_PREPROCESS_DISALLOW_IN_HEADER_H
#pragma once

#include "thirdparty/monitor_sdk_common/base/static_assert.h"

/// disallow macro be used in header files
///
/// @example
/// #define SOMEMACRO() COMMON_PP_DISALLOW_IN_HEADER_FILE()
/// A compile error will be issued if SOMEMACRO() is used in header files
#ifdef __GNUC__
# define COMMON_PP_DISALLOW_IN_HEADER_FILE() \
    COMMON_STATIC_ASSERT(__INCLUDE_LEVEL__ == 0, "This macro can not be used in header files");
#else
# define COMMON_PP_DISALLOW_IN_HEADER_FILE()
#endif

#endif // COMMON_BASE_PREPROCESS_DISALLOW_IN_HEADER_H
