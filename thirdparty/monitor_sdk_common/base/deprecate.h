//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 13:44
//  @file:      deprecate.h
//  @author:    	
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_DEPRECATE_H
#define COMMON_BASE_DEPRECATE_H
#pragma once

#include "thirdparty/monitor_sdk_common/base/compiler_version.h"

/// define DEPRECATED and DEPRECATED_BY
#ifdef __DEPRECATED
# if defined __GNUC__
#  define DEPRECATED __attribute__((deprecated))
#  if GCC_VERSION_NUMBER() >= 40500
#   define DEPRECATED_BY(new_symbol) __attribute__((deprecated("please use '" #new_symbol "' instead")))
#   define DEPRECATED_MESSAGE(msg) __attribute__((deprecated(msg)))
#  else
#   define DEPRECATED_BY(new_symbol) DEPRECATED
#   define DEPRECATED_MESSAGE(msg) DEPRECATED
#  endif
# elif defined _MSC_VER
#  define DEPRECATED __declspec(deprecated)
#  define DEPRECATED_BY(new_symbol) __declspec(deprecated("deprecated, please use " #new_symbol " instead"))
#  define DEPRECATED_MESSAGE(msg) __declspec(deprecated(msg))
# else
#  define DEPRECATED
#  define DEPRECATED_BY(new_symbol)
#   define DEPRECATED_MESSAGE(msg)
# endif
#else
# define DEPRECATED
# define DEPRECATED_BY(x)
# define DEPRECATED_MESSAGE(x)
#endif // __DEPRECATED

#endif // COMMON_BASE_DEPRECATE_H
