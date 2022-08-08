//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 14:27
//  @file:      stdbool.h
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_COMPATIBLE_STDBOOL_H
#define COMMON_BASE_COMPATIBLE_STDBOOL_H

#ifndef __cplusplus

#ifdef _MSC_VER
typedef unsigned char _Bool;
#define bool _Bool
#define false 0
#define true 1
#define __bool_true_false_are_defined 1
#else
#include <stdbool.h>
#endif

#endif // __cplusplus

#endif // COMMON_BASE_COMPATIBLE_STDBOOL_H
