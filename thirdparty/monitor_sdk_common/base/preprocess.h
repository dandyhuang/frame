// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef COMMON_BASE_PREPROCESS_H
#define COMMON_BASE_PREPROCESS_H

#include "thirdparty/monitor_sdk_common/base/preprocess/disallow_in_header.h"
#include "thirdparty/monitor_sdk_common/base/preprocess/join.h"
#include "thirdparty/monitor_sdk_common/base/preprocess/stringize.h"
#include "thirdparty/monitor_sdk_common/base/preprocess/varargs.h"

/// prevent macro substitution for function-like macros
/// if macro 'min()' was defined:
/// 'int min()' whill be substituted, but
/// 'int min COMMON_PP_PREVENT_MACRO_SUBSTITUTION()' will not be substituted.
#define COMMON_PP_PREVENT_MACRO_SUBSTITUTION

#endif // COMMON_BASE_PREPROCESS_H

