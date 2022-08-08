//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 18:59
//  @file:      check_error.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "thirdparty/monitor_sdk_common/system/check_error.h"

#ifdef _WIN32
#include "thirdparty/monitor_sdk_common/base/common_windows.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/glog/raw_logging.h"

namespace common {

static void ReportError(const char* function_name, DWORD error_code)
{
    abort();
}

void CheckWindowsError(const char* function_name, bool error)
{
    if (!error)
        ReportError(function_name, ::GetLastError());
}

bool CheckWindowsWaitError(const char* function_name, unsigned int code)
{
    switch (code)
    {
    case WAIT_TIMEOUT:
        return false;
    case WAIT_FAILED:
        ReportError(function_name, ::GetLastError());
    }
    return true;
}

void CheckNtError(const char* function_name, long ntstatus) // NOLINT(runtime/int)
{
    if (ntstatus)
        ReportError(function_name, ntstatus);
}

void ReportErrnoError(const char* function_name, int error)
{
    const char* msg = strerror(error);
    RAW_LOG(FATAL, "%s: Fatal error, %s", function_name, msg);
}

void ReportPosixError(const char* function_name)
{
    ReportErrnoError(function_name, errno);
}

bool ReportPosixTimedError(const char* function_name)
{
    int error = errno;
    if (error == ETIMEDOUT)
        return false;
    ReportErrnoError(function_name, error);
    return true;
}

bool ReportPthreadTimedError(const char* function_name, int error)
{
    if (error == ETIMEDOUT)
        return false;
    ReportErrnoError(function_name, error);
    return false;
}

bool ReportPthreadTryLockError(const char* function_name, int error)
{
    if (error == EBUSY || error == EAGAIN)
        return false;
    ReportErrnoError(function_name, error);
    return false;
}

} // end of namespace common

#endif

#ifdef __unix__

namespace common {

void HandleErrnoError(const char* function_name, int error)
{
    const char* msg = strerror(error);
    fprintf(stderr, "%s: Fatal error, %s", function_name, msg);
    abort();
}

void HandlePosixError(const char* function_name)
{
    HandleErrnoError(function_name, errno);
}

bool HandlePosixTimedError(const char* function_name)
{
    int error = errno;
    if (error == ETIMEDOUT)
        return false;
    HandleErrnoError(function_name, error);
    return true;
}

bool HandlePthreadTimedError(const char* function_name, int error)
{
    if (error == ETIMEDOUT)
        return false;
    HandleErrnoError(function_name, error);
    return false;
}

bool HandlePthreadTryLockError(const char* function_name, int error)
{
    if (error == EBUSY || error == EAGAIN)
        return false;
    HandleErrnoError(function_name, error);
    return false;
}

} // end of namespace common

#endif // __unix__
