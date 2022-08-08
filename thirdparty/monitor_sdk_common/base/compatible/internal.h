//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 14:26
//  @file:      internal.h
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_COMPATIBLE_INTERNAL_H
#define COMMON_BASE_COMPATIBLE_INTERNAL_H

#ifdef __cplusplus
# define COMPATIBLE_INLINE inline
# define STATIC_CAST(type, value) static_cast<type>(value)
# define DYNAMIC_CAST(type, value) dynamic_cast<type>(value)
# define CONST_CAST(type, value) const_cast<type>(value)
# define REINTERPRET_CAST(type, value) reinterpret_cast<type>(value)
# define EXTERN_C extern "C"
# define EXTERN_CXX extern "C++"
# define DEFINE_INTEGER_CONST(type, name, value) const type name = value
#else
# define COMPATIBLE_INLINE static __inline
# define STATIC_CAST(type, value) ((type)(value))
# define DYNAMIC_CAST(type, value) ((type)(value))
# define CONST_CAST(type, value) ((type)(value))
# define REINTERPRET_CAST(type, value) ((type)(value))
# define EXTERN_C extern
# define DEFINE_INTEGER_CONST(type, name, value) enum { name = (value) }
#endif

#endif // COMMON_BASE_COMPATIBLE_INTERNAL_H

