//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 18:57
//  @file:      check_error.h
//  @author:    
//  @brief:     common pthread error handler
//
//********************************************************************


#ifndef COMMON_SYSTEM_CHECK_ERROR_H
#define COMMON_SYSTEM_CHECK_ERROR_H
#pragma once

#ifdef _WIN32

#define CHECK_WINDOWS_ERROR(bool_value) \
    ::common::CheckWindowsError(__FUNCTION__, bool_value)

#define CHECK_WINDOWS_WAIT_ERROR(expr) \
    ::common::CheckWindowsWaitError(__FUNCTION__, expr)

#define CHECK_NT_ERROR(expr) \
    ::common::CheckNtError(__FUNCTION__, expr)

#define CHECK_ERRNO_ERROR(expr) { \
    LOG(WARNING) << "CHECK_ERRNO_ERROR not implement on win32, expr : " << expr; \
}

namespace common {

void CheckWindowsError(const char* function_name, bool error);
bool CheckWindowsWaitError(const char* function_name, unsigned int code);
void CheckNtError(const char* function_name, long ntstatus); // NOLINT(runtime/int)

} // namespace common

#endif

#ifdef __unix__
namespace common {

void HandleErrnoError(const char* function_name, int error);
inline void CheckErrnoError(const char* function_name, int error)
{
    if (error)
        HandleErrnoError(function_name, error);
}

void HandlePosixError(const char* function_name);
inline void CheckPosixError(const char* function_name, int result)
{
    if (result < 0)
        HandlePosixError(function_name);
}

bool HandlePosixTimedError(const char* function_name);
inline bool CheckPosixTimedError(const char* function_name, int result)
{
    if (result < 0)
        return HandlePosixTimedError(function_name);
    return true;
}

bool HandlePthreadTimedError(const char* function_name, int error);
inline bool CheckPthreadTimedError(const char* function_name, int error)
{
    if (error)
        return HandlePthreadTimedError(function_name, error);
    return true;
}

bool HandlePthreadTryLockError(const char* function_name, int error);
inline bool CheckPthreadTryLockError(const char* function_name, int error)
{
    if (error)
        return HandlePthreadTryLockError(function_name, error);
    return true;
}

#define CHECK_ERRNO_ERROR(expr) \
    CheckErrnoError(__PRETTY_FUNCTION__, (expr))

#define CHECK_POSIX_ERROR(expr) \
    CheckPosixError(__PRETTY_FUNCTION__, (expr))

#define CHECK_POSIX_TIMED_ERROR(expr) \
    CheckPosixTimedError(__PRETTY_FUNCTION__, (expr))

#define CHECK_PTHREAD_ERROR(expr) \
    CHECK_ERRNO_ERROR((expr))

#define CHECK_PTHREAD_TIMED_ERROR(expr) \
    CheckPthreadTimedError(__PRETTY_FUNCTION__, (expr))

#define CHECK_PTHREAD_TRYLOCK_ERROR(expr) \
    CheckPthreadTryLockError(__PRETTY_FUNCTION__, (expr))

} // end of namespace common

#endif


namespace common {

void ReportErrnoError(const char* function_name, int error);

void ReportPosixError(const char* function_name);

bool ReportPosixTimedError(const char* function_name);

bool ReportPthreadTimedError(const char* function_name, int error);

bool ReportPthreadTryLockError(const char* function_name, int error);

} // namespace common

#define COMMON_CHECK_ERRNO_ERROR(expr) \
    CheckErrnoError(__PRETTY_FUNCTION__, (expr))

#define COMMON_CHECK_POSIX_ERROR(expr) \
    CheckPosixError(__PRETTY_FUNCTION__, (expr))

#define COMMON_CHECK_POSIX_TIMED_ERROR(expr) \
    CheckPosixTimedError(__PRETTY_FUNCTION__, (expr))

#define COMMON_CHECK_PTHREAD_ERROR(expr) \
    COMMON_CHECK_ERRNO_ERROR((expr))

#define COMMON_CHECK_PTHREAD_TIMED_ERROR(expr) \
    CheckPthreadTimedError(__PRETTY_FUNCTION__, (expr))

#define COMMON_CHECK_PTHREAD_TRYLOCK_ERROR(expr) \
    CheckPthreadTryLockError(__PRETTY_FUNCTION__, (expr))


#endif // COMMON_SYSTEM_CHECK_ERROR_H
