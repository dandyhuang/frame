//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 12:57
//  @file:      platform_features.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_BASE_PLATFORM_FEATURES_H
#define COMMON_BASE_PLATFORM_FEATURES_H

#ifdef __GNUC__
/// support thread safe static variable initialization
#define HAS_THREAD_SAFE_STATICS 0

/// static const integral members need definition out of class
#define STATIC_CONST_MEMBER_NEED_DEFINATION 1

#endif

#if defined _MSC_VER
# define THREAD_LOCAL __declapsec(thread)
# define WEAK_SYMBOL __declapsec(selectany)
# define ALWAYS_INLINE __forceinline
#elif defined __GNUC__
# define THREAD_LOCAL __thread
# define WEAK_SYMBOL __attribute__((weak))
# define ALWAYS_INLINE inline __attribute__((always_inline))
#else
# error Unknown compiler
#endif

/// known alignment insensitive platforms
#if defined(__i386__) || \
    defined(__x86_64__) || \
    defined(_M_IX86) || \
    defined(_M_X64)
#define ALIGNMENT_INSENSITIVE_PLATFORM 1
#endif

/// define __attribute__ of gcc to null under non gcc
#ifndef __GNUC__
#define __attribute__(x)
#endif

// ���Ա�עһ����������Ϊ��
// void swap(int *p, int* q) __attribute__((nonnull(1, 2)));

// ��θ�ʽ��˵��
// int log_printf(const char* format, ...) __attribute__((format(1, 2)));

#endif // COMMON_BASE_PLATFORM_FEATURES_H
