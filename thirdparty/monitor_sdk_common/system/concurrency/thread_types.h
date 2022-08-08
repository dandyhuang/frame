//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 18:20
//  @file:      thread_types.h
//  @author:    	
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_THREAD_TYPES_H
#define COMMON_SYSTEM_CONCURRENCY_THREAD_TYPES_H
#pragma once

#if defined __unix__
# include <pthread.h>
#endif // __unix__

#include <string>

#include "thirdparty/monitor_sdk_common/base/compatible/errno.h"
#include "thirdparty/monitor_sdk_common/base/stdint.h"

namespace common {

#if defined _WIN32
    typedef void*       ThreadHandleType;
#elif __unix__
    typedef pthread_t   ThreadHandleType;
#endif

/// ThreadAttribute represent thread attribute.
/// Usage:
/// ThreadAttribute()
///     .SetName("ThreadPoolThread")
///     .SetStackSize(64 * 1024)
class ThreadAttribute
{
public:
    ThreadAttribute();
    ~ThreadAttribute();
    ThreadAttribute& SetName(const std::string& name);
    ThreadAttribute& SetStackSize(size_t size);
    ThreadAttribute& SetDetached(bool detached);
    ThreadAttribute& SetPriority(int priority);
private:
    std::string m_name;
#if defined __unix__
    pthread_attr_t m_attr;
#elif defined _WIN32
    size_t m_stack_size;
    int m_priority;
    int m_detached;
#endif
};

} // end of namespace common

#endif // COMMON_SYSTEM_CONCURRENCY_THREAD_TYPES_H
