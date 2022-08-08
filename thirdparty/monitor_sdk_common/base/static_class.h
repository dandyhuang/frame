// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef COMMON_BASE_STATIC_CLASS_H
#define COMMON_BASE_STATIC_CLASS_H
#pragma once

#include "thirdparty/monitor_sdk_common/base/cxx11.h"

// COMMON_DECLARE_STATIC_CLASS Mark a class that all members a static.

#ifdef COMMON_CXX11_ENABLED
#define COMMON_DECLARE_STATIC_CLASS(Name) \
    private: \
        Name() = delete; \
        ~Name() = delete
#else
#define COMMON_DECLARE_STATIC_CLASS(Name) \
    private: \
        Name(); \
        ~Name()
#endif

#endif // COMMON_BASE_STATIC_CLASS_H
