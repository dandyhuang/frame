//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:02
//  @file:      process.cpp
//  @author:
//  @brief:
//
//********************************************************************


#include "thirdparty/monitor_sdk_common/system/process.h"

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#ifndef _WIN32
#include <sys/wait.h>
#else
#include <windows.h>
#endif
#include <vector>
#include <iostream>

#include "thirdparty/monitor_sdk_common/base/scoped_ptr.h"
#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "thirdparty/monitor_sdk_common/storage/file/file_stream.h"
#include "thirdparty/monitor_sdk_common/system/io/directory.h"
#include "thirdparty/monitor_sdk_common/system/io/file.h"
#include "thirdparty/monitor_sdk_common/system/io/path.h"
#include "thirdparty/monitor_sdk_common/system/io/textfile.h"
#include "thirdparty/monitor_sdk_common/system/time/time_utils.h"
#include "thirdparty/monitor_sdk_common/system/net/ip_address.h"
#include "thirdparty/glog/logging.h"

namespace common {

namespace {

std::string ReadField(const char* proc_file, unsigned int field_pos)
{
    std::string data;
    if (!io::textfile::LoadToString(proc_file, &data)) {
        return "";
    }
    std::vector<std::string> fields;
    SplitString(data, " ",  &fields);
    return fields[field_pos];
}

} // namespace

#ifdef _WIN32
static time_t s_process_start_time = time(NULL);
#endif

time_t ThisProcess::StartTime()
{
#ifdef _WIN32
    return s_process_start_time;
#else
    int hz = sysconf(_SC_CLK_TCK);
    double uptime;
    StringToNumber(ReadField("/proc/uptime", 0), &uptime);
    double start_time;
    StringToNumber(ReadField("/proc/self/stat", 21), &start_time);
    return static_cast<time_t>(time(NULL) - uptime + start_time / hz);
#endif
}

time_t ThisProcess::StartTimeDiff()
{
    time_t start_time_diff;
    StringToNumber(ReadField("/proc/self/stat", 21), &start_time_diff);
    return start_time_diff;
}

time_t ThisProcess::ElapsedTime()
{
    time_t start_time = ThisProcess::StartTime();
    if (start_time < 0)
        return -1;
    return time(NULL) - start_time;
}

std::string ThisProcess::BinaryPath()
{
#ifdef _WIN32
    char name_module[MAX_PATH] = {0};
    GetModuleFileName(NULL, name_module, sizeof(name_module));
    return name_module;
#else
    char path[PATH_MAX] = {0};
    ssize_t length = readlink("/proc/self/exe", path, sizeof(path));
    if (length > 0)
        return std::string(path, length);
#endif
    return "<unknown binary path>";
}

#ifndef _WIN32

std::string ThisProcess::BinaryName()
{
    std::string binary_name;

    char path[PATH_MAX] = {0};
    FILE* fp = fopen("/proc/self/cmdline", "r");
    if (fp != NULL) {
        if (fgets(path, sizeof(path) - 1, fp))
            binary_name = io::path::GetBaseName(path);
        fclose(fp);
    }
    // If fopen failed or the process is defunct,
    // read binary name from exe softlink.
    if (binary_name.empty()) {
        binary_name = io::path::GetBaseName(BinaryPath());
    }
    return binary_name;
}

std::string ThisProcess::BinaryDirectory()
{
    std::string path = BinaryPath();
    return io::path::GetDirectory(path);
}

/////////////////////////////////////////////////////
static void ResetChildSignalsToDefault()
{
    signal(SIGHUP, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGILL, SIG_DFL);
    signal(SIGABRT, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGBUS, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    signal(SIGSYS, SIG_DFL);
}

Process::Process() : m_pid(0)
{
}

Process::Process(ProcessId process_id) : m_pid(process_id)
{
}

ProcessId Process::GetId() const
{
    return m_pid;
}

bool Process::IsCurrent() const
{
    return m_pid == getpid();
}

bool Process::IsValid() const
{
    return m_pid != 0;
}

bool Process::Terminate()
{
    if (m_pid <= 1) {
        LOG(ERROR) << "process pid_t is less than 1, m_pid: " << m_pid;
        return false;
    }

    // most sleep 1000 ms
    static unsigned kMaxSleepInMS = 1000;
    unsigned sleep_ms = 4;

    // first we use SIGTERM to kill the process
    bool result = kill(m_pid, SIGTERM) == 0;

    if (result) {
        int tries = 60;

        bool exited = false;
        while (tries > 0) {
            pid_t pid = COMMON_EINTR_IGNORED(waitpid(m_pid, NULL, WNOHANG));
            if (pid == m_pid) {
                exited = true;
                break;
            } else if (-1 == pid) {
                if (ECHILD == errno) {
                    exited = true;
                    break;
                }
                LOG(ERROR) << "error waitpid for " << m_pid;
            }

            usleep(sleep_ms * 1000);
            if (sleep_ms < kMaxSleepInMS)
                sleep_ms *= 2;
            --tries;
        }

        if (!exited) {
            result = kill(m_pid, SIGKILL) == 0;
        }
    }

    if (!result) {
        LOG(ERROR) << "Unable to terminate process: " << m_pid;
    } else {
        m_pid = 0;
    }

    return result;
}

bool Process::WaitForExit(int* exit_code)
{
    CHECK(!IsCurrent()) << " can't wait itself for exit";
    int status;
    if (COMMON_EINTR_IGNORED(waitpid(m_pid, &status, 0)) == -1) {
        return false;
    }

    if (WIFEXITED(status)) {
        m_pid = 0;
        *exit_code = WEXITSTATUS(status);
        return true;
    }

    return false;
}

bool Process::TimedWaitForExit(int64_t timeout_milliseconds,
                               int* exit_code)
{
    CHECK(!IsCurrent()) << " can't wait itself for exit";
    int status = -1;
    pid_t pid = COMMON_EINTR_IGNORED(waitpid(m_pid, &status, WNOHANG));
    int64_t wakeup_time = TimeUtils::Milliseconds() + timeout_milliseconds;
    static const int64_t kMaxSleepInMS = 1 << 18;
    int64_t max_sleep_time_usecs = 1 << 10;

    while (pid == 0) {
        int64_t now = TimeUtils::Milliseconds();
        if (now > wakeup_time)
            break;

        int64_t sleep_time_in_us = (wakeup_time - now) * 1000;
        if (sleep_time_in_us > max_sleep_time_usecs)
            sleep_time_in_us = max_sleep_time_usecs;

        usleep(sleep_time_in_us);
        pid = COMMON_EINTR_IGNORED(waitpid(m_pid, &status, WNOHANG));

        if (max_sleep_time_usecs < kMaxSleepInMS)
            max_sleep_time_usecs *= 2;
    }

    if (pid == -1 || status == -1) {
        *exit_code = -1;
        return false;
    }

    if (WIFSIGNALED(status)) {
        m_pid = 0;
        *exit_code = -1;
        return true;
    }

    if (WIFEXITED(status)) {
        m_pid = 0;
        *exit_code = WEXITSTATUS(status);
        return true;
    }

    *exit_code = -1;
    return false;
}

bool Process::SendSignal(int signal)
{
    if (m_pid <= 1) {
        LOG(ERROR) << "process pid_t is less than 1, m_pid: " << m_pid;
        return false;
    }
    return kill(m_pid, signal) == 0;
}

// static
Process Process::Start(const std::string& file_name, const std::string& arguments)
{
    CHECK(file_name.find(" ") == std::string::npos) << "file_name can't contain whitespace";
    std::string cmd_line = file_name;
    cmd_line += " ";
    cmd_line += arguments;
    std::vector<std::string> argv;
    SplitString(cmd_line, " ", &argv);
    return Start(argv);
}

// static
Process Process::Start(const std::vector<std::string>& argv)
{
    if (argv.empty()) {
        CHECK(false) << "Start process argv is empty";
        return Process();
    }
    pid_t pid = fork();
    if (pid < 0) {
        CHECK(false) << "Fork child process " << argv[0] << " error";
    } else if (pid == 0) {
        // child process
        scoped_array<char*> argv_cstr(new char*[argv.size() + 1]);
        ResetChildSignalsToDefault();

        for (size_t i = 0; i < argv.size(); ++i)
            argv_cstr[i] = const_cast<char*>(argv[i].c_str());
        argv_cstr[argv.size()] = NULL;

        execvp(argv_cstr[0], argv_cstr.get());

        LOG(ERROR) << "Failed to execvp " << argv_cstr[0]
                   << ", Error: " << strerror(errno);
        _Exit(127);
    } else {
        LOG(INFO) << "Start process " << argv[0] << ", pid " << pid;
        return Process(pid);
    }

    CHECK(false) << "Start process not success";
    return Process();
}

// static
Process Process::GetCurrentProcess()
{
    return Process(getpid());
}

// static
Process Process::GetProcessById(ProcessId process_id)
{
    return Process(process_id);
}

::std::string Process::BinaryPath()
{
#ifdef _WIN32
    return "<unknown binary path>"
#else
    char path[PATH_MAX] = {0};
    std::string proc_exe_path = "/proc/" + NumberToString(m_pid) + "/exe";
    ssize_t length = readlink(proc_exe_path.c_str(), path, sizeof(path));
    if (length > 0)
        return std::string(path, length);
#endif
    return "<unknown binary path>";
}

::std::string Process::BinaryName()
{
    std::string binary_name;

    char path[PATH_MAX] = {0};
    std::string proc_cmd_path = "/proc/" + NumberToString(m_pid) + "/cmdline";
    FILE* fp = fopen(proc_cmd_path.c_str(), "r");
    if (fp != NULL) {
        fgets(path, sizeof(path) - 1, fp);
        fclose(fp);
        binary_name = io::path::GetBaseName(path);
    }
    // If fopen failed or the process is defunct,
    // read binary name from exe softlink.
    if (binary_name.empty()) {
        binary_name = io::path::GetBaseName(BinaryPath());
    }
    return binary_name;
}

::std::string Process::BinaryCmdline()
{
    std::string proc_cmd_path = "/proc/" + NumberToString(m_pid) + "/cmdline";
    std::string cmdline;
    if (!io::textfile::LoadToString(proc_cmd_path, &cmdline)) {
        return "";
    }
    return cmdline;
}

::std::string Process::BinaryDirectory()
{
    std::string path = BinaryPath();
    return io::path::GetDirectory(path);
}

time_t Process::StartTime()
{
#ifdef _WIN32
    return -1;
#else
    int hz = sysconf(_SC_CLK_TCK);
    double uptime;
    StringToNumber(ReadField("/proc/uptime", 0), &uptime);
    double start_time;
    std::string proc_stat_path = "/proc/" + NumberToString(m_pid) + "/stat";
    std::string value = ReadField(proc_stat_path.c_str(), 21);
    if (value.empty()) {
        return 0;
    }
    StringToNumber(value, &start_time);
    return static_cast<time_t>(time(NULL) - uptime + start_time / hz);
#endif
}

time_t Process::StartTimeDiff()
{
    std::string proc_stat_path = "/proc/" + NumberToString(m_pid) + "/stat";
    time_t start_time_diff;
    std::string value = ReadField(proc_stat_path.c_str(), 21);
    if (value.empty()) {
        return 0;
    }
    StringToNumber(value, &start_time_diff);
    return start_time_diff;
}

time_t Process::ElapsedTime()
{
    return time(NULL) - StartTime();
}

char Process::Stat()
{
    std::string proc_stat_path = "/proc/" + NumberToString(m_pid) + "/stat";
    if (!io::file::Exists(proc_stat_path)) {
        return 'G';
    }
    ::std::string stat = ReadField(proc_stat_path.c_str(), 2);
    return stat[0];
}

pid_t Process::GetParentId() {
    std::string proc_stat_path = "/proc/" + NumberToString(m_pid) + "/stat";
    if (!io::file::Exists(proc_stat_path)) {
        return -1;
    }
    pid_t p_pid = -1;
    std::string value = ReadField(proc_stat_path.c_str(), 3);
    if (value.empty()) {
        return -1;
    }
    StringToNumber(value, &p_pid);
    return p_pid;
}

bool Process::GetUids(uid_t* real_uid, uid_t* effective_uid, uid_t* set_uid, uid_t* file_sys_uid) {
    std::string proc_status_path = "/proc/" + NumberToString(m_pid) + "/status";
    std::vector<std::string> status_lines;
    if (!io::textfile::ReadLines(proc_status_path, &status_lines)) {
        LOG(ERROR) << "unable to open file:" << proc_status_path;
        return false;
    }
    std::vector<std::string>::const_iterator uid_line_iter = status_lines.begin();
    bool is_found = false;
    for (; uid_line_iter != status_lines.end(); ++uid_line_iter) {
        if (StringStartsWith(*uid_line_iter, "Uid:")) {
            is_found = true;
            break;
        }
    }
    if (!is_found) {
        LOG(ERROR) << "unable to find uid config in status file:" << proc_status_path;
        return false;
    }
    std::string uid_line = StringTrimLeft(*uid_line_iter, "Uid:");
    std::vector< ::std::string> uid_list;
    SplitString(uid_line, "\t", &uid_list);
    if ( !(StringToNumber(StringTrim(uid_list[0]), real_uid) &&
            StringToNumber(StringTrim(uid_list[1]), effective_uid) &&
            StringToNumber(StringTrim(uid_list[2]), set_uid) &&
            StringToNumber(StringTrim(uid_list[3]), file_sys_uid)) ) {
        LOG(ERROR) << "uable to convert uids to uid_t:" << uid_line;
        return false;
    }
    return true;
}

bool Process::GetRealUid(uid_t* real_uid) {
    uid_t effective_uid, set_uid, file_sys_uid;
    return GetUids(real_uid, &effective_uid, &set_uid, &file_sys_uid);
}

bool Process::GetEffectiveUid(uid_t* effective_uid) {
    uid_t real_uid, set_uid, file_sys_uid;
    return GetUids(&real_uid, effective_uid, &set_uid, &file_sys_uid);
}

bool Process::GetSetUid(uid_t* set_uid) {
    uid_t real_uid, effective_uid, file_sys_uid;
    return GetUids(&real_uid, &effective_uid, set_uid, &file_sys_uid);
}

bool Process::GetFileSystemUid(uid_t* file_sys_uid) {
    uid_t real_uid, effective_uid, set_uid;
    return GetUids(&real_uid, &effective_uid, &set_uid, file_sys_uid);
}

bool Process::HasPort(const IpAddress& addr, uint16_t port)
{
    ::std::vector< ::std::string> proc_net_tcp;
    if (!io::textfile::ReadLines("/proc/net/tcp", &proc_net_tcp)) {
        return false;
    }
    ::std::string port_string(UInt16ToHexString(port));
    StringToUpper(&port_string);
    ::std::vector< ::std::string> line_set;
    ::std::string socket_inode = "";
    for (::std::vector< ::std::string>::const_iterator iter = proc_net_tcp.begin();
            iter != proc_net_tcp.end(); ++iter) {
        line_set.clear();
        SplitString(*iter, " ", &line_set);
        if (line_set.size() > 9 && (line_set[1] == addr.ToHexString() + ":" + port_string)) {
            socket_inode = line_set[9];
            VLOG_IF(2, socket_inode == "0") << "socket_inode is '0' from [" << *iter <<  "]";
            break;
        }
    }
    VLOG(2) << "port[" << port << "] - socket_inode[" << socket_inode << "]";
    if (socket_inode.empty()) {
        return false;
    }

    ::std::string socket_string = "socket:[" + socket_inode + "]";
    ::std::vector< ::std::string> proc_fds;
    if(!io::directory::GetAll("/proc/" + NumberToString(m_pid) + "/fd/", &proc_fds)) {
        return false;
    }
    char path[PATH_MAX] = {0};
    for (::std::vector< ::std::string>::const_iterator iter = proc_fds.begin();
            iter != proc_fds.end(); ++iter) {
        memset(path, 0, PATH_MAX);
        ssize_t length = readlink(iter->c_str(), path, sizeof(path));
        VLOG(2) << "pid[" << m_pid << "]" << ::std::string(path, length);
        if (length > 0 && ::std::string(path, length) == socket_string) {
            return true;
        }
    }
    return false;
}

pid_t Process::GetSessionId()
{
    std::string proc_stat_path = "/proc/" + NumberToString(m_pid) + "/stat";
    if (!io::file::Exists(proc_stat_path)) {
        return -1;
    }
    pid_t p_pid = -1;
    std::string value = ReadField(proc_stat_path.c_str(), 5);
    if (value.empty()) {
        return -1;
    }
    StringToNumber(value, &p_pid);
    return p_pid;
}

#endif // _WIN32

} // end of namespace common

