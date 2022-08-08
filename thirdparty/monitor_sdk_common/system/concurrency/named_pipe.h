//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-08-29 18:54
//  @file:      named_pipe.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_CONCURRENCY_NAMED_PIPE_H
#define COMMON_SYSTEM_CONCURRENCY_NAMED_PIPE_H

#if defined __unix__
#include "thirdparty/monitor_sdk_common/system/concurrency/named_pipe_unix.h"
#elif defined _WIN32
#include "thirdparty/monitor_sdk_common/system/concurrency/named_pipe_windows.h"
#else
#error Unknown platform
#endif

#endif // COMMON_SYSTEM_CONCURRENCY_NAMED_PIPE_H

