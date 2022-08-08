//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:01
//  @file:      process.h
//  @author:
//  @brief:
//
//********************************************************************


#ifndef COMMON_SYSTEM_PROCESS_H
#define COMMON_SYSTEM_PROCESS_H

#ifdef __unix__
#include <stdint.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#define _CRT_NONSTDC_NO_WARNINGS 1
#define _POSIX
#include <process.h>
typedef int pid_t;
typedef unsigned int uid_t;
#endif
typedef pid_t ProcessId;

#include <time.h>
#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/system/eintr_ignored.h"

namespace common {

class IpAddress;

class ThisProcess
{
public:
    static pid_t GetId()
    {
#ifdef _MSC_VER
        return _getpid();
#else
        return getpid();
#endif
    }

    /// obtain executable file path
    static ::std::string BinaryPath();
    /// obtain executable file name
    static ::std::string BinaryName();

    /// obtain directory of binary executable file
    static ::std::string BinaryDirectory();

    // obtain start time in seconds since Epoch
    static time_t StartTime();
    // obtain start cpu time since system start
    static time_t StartTimeDiff();

    // obtain elapsed seconds since start
    static time_t ElapsedTime();
};

// common APIs:
//
// pid_t getpid();
// int system(const char *command);

class Process
{
public:
    Process();
    explicit Process(ProcessId process_id);

    // construct Process object attached to the current process
    static Process GetCurrentProcess();

    // construct Process object attached to the specified process_id
    static Process GetProcessById(ProcessId process_id);

    // Start a process by specifying the name of an application and
    // a set of command-line arguments
    // return the Process object associated with the spawn process
    // file_name is the name of the executable file, if file_name can't be
    // found in the PATH environment, you must specify the full path of it
    // arguments is the argument list of the executable file
    static Process Start(const ::std::string& file_name,
                         const ::std::string& arguments = "");

    // Get the process id of this Process object
    ProcessId GetId() const;

    // Returns true if the associated process is the current calling process
    // Otherwise returns false
    bool IsCurrent() const;

    // Returns true if the m_pid is not equal to 0, otherwise returns false
    bool IsValid() const;

    // Terminate the assocaited process
    // If it terminates successfully, returns true
    // otherwise returns false
    bool Terminate();

    // Wait indefinitely for the associated process to exit
    // If the process exits successfully, put the exit_code
    // to the exit_code and returns true, otherwise returns false
    bool WaitForExit(int* exit_code);

    // Wait the specified number of milliseconds for the
    // associated process to exit. If the process exits successfully,
    // put the exit code to the exit_code and returns true, otherwise,
    // returns false
    bool TimedWaitForExit(int64_t timeout_milliseconds,
                          int *exit_code);

    // Send the signal to the associated process
    bool SendSignal(int signal);

    /// obtain executable file path
    ::std::string BinaryPath();
    /// obtain executable file name
    ::std::string BinaryName();
    /// abtain cmdline
    ::std::string BinaryCmdline();

    /// obtain directory of binary executable file
    ::std::string BinaryDirectory();

    // obtain start time in seconds since Epoch
    time_t StartTime();

    // obtain start cpu time since system start
    time_t StartTimeDiff();

    // obtain elapsed seconds since start
    time_t ElapsedTime();

    // obtain process stat
    //    D    Uninterruptible sleep (usually IO)
    //    R    Running or runnable (on run queue)
    //    S    Interruptible sleep (waiting for an event to complete)
    //    T    Stopped, either by a job control signal or because it is being traced.
    //    W    paging (not valid since the 2.6.xx kernel)
    //    X    dead (should never be seen)
    //    Z    Defunct ("zombie") process, terminated but not reaped by its parent.
    //    G    Gone, not found.
    char Stat();

    pid_t GetParentId();

    bool GetRealUid(uid_t* real_uid);

    bool GetEffectiveUid(uid_t* effective_uid);

    bool GetSetUid(uid_t* set_uid);

    bool GetFileSystemUid(uid_t* file_sys_uid);

    bool GetUids(uid_t* real_uid, uid_t* effective_uid, uid_t* set_uid, uid_t* file_sys_uid);

    bool HasPort(const IpAddress& addr, uint16_t port);

    pid_t GetSessionId();
private:
    // Internal use only
    // combine the file_name and arguements to argv vector and passed it to execvp
    static Process Start(const ::std::vector< ::std::string>& argv);

    // associated process's ProcessId
    ProcessId m_pid;
};

} // end of namespace common

#endif // COMMON_SYSTEM_PROCESS_H

