//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 18:04
//  @file:      this_thread.h
//  @author:    —Ó“ª∑…(yangyifei@baidu.com)	
//  @brief:     current thread scoped attributes and operations
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_THIS_THREAD_H
#define COMMON_SYSTEM_CONCURRENCY_THIS_THREAD_H
#pragma once

#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/thread_types.h"
#ifdef _WIN32
#undef Yield
#endif

namespace common {
/// thread scoped attribute and operations of current thread
class ThisThread
{
    ThisThread();
    ~ThisThread();
public:
    static void Exit();
    static void Yield();
    static void Sleep(int64_t time_in_ms);
    static int GetLastErrorCode();
    static ThreadHandleType GetHandle();
    static int GetId();
    static bool IsMain();
};

} // end of namespace common

#endif // COMMON_SYSTEM_CONCURRENCY_THIS_THREAD_H
