//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-12-09
//  @file:      cpu_usage.cpp
//  @author:    
//  @brief:     
//
//********************************************************************

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <assert.h>
#include <stdio.h>
#include "thirdparty/monitor_sdk_common/system/cpu/cpu_usage.h"
#include "thirdparty/monitor_sdk_common/system/system_information.h"
#include "thirdparty/monitor_sdk_common/base/string/algorithm.h"
#include "thirdparty/monitor_sdk_common/base/string/number.h"
#include "glog/logging.h"

#ifdef _WIN32 // windows platform

#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "Psapi.lib")


namespace common {

uint64_t FileTimeToUtc(const FILETIME* ftime)
{
    LARGE_INTEGER li;
    assert(ftime);
    li.LowPart = ftime->dwLowDateTime;
    li.HighPart = ftime->dwHighDateTime;
    return li.QuadPart;
}

int GetProcessorNumber()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return static_cast<int>(info.dwNumberOfProcessors);
}

bool GetCpuUsageSinceLastCall(int32_t pid, double* cpu)
{
    //上一次的时间
    static int64_t last_time = 0;
    static int64_t last_system_time = 0;
    static int processor_count = -1;

    FILETIME now;
    FILETIME creation_time;
    FILETIME exit_time;
    FILETIME kernel_time;
    FILETIME user_time;
    int64_t system_time;
    int64_t time;
    int64_t system_time_delta;
    int64_t time_delta;

    processor_count = GetProcessorNumber();
    GetSystemTimeAsFileTime(&now);

    HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    if (GetProcessTimes(handle, &creation_time, &exit_time, &kernel_time, &user_time) == 0)
    {
        // We don't assert here because in some cases (such as in the TaskManager)
        // we may call this function on a process that has just exited but we have
        // not yet received the notification.
        CloseHandle(handle);
        return false;
    }
    CloseHandle(handle);

    system_time = (FileTimeToUtc(&kernel_time) + FileTimeToUtc(&user_time)) /
                    processor_count;
    time = FileTimeToUtc(&now);

    if (last_time == 0)
    {
        // First call, just set the last values.
        last_system_time = system_time;
        last_time = time;
        return false;
    }

    system_time_delta = system_time - last_system_time;
    time_delta = time - last_time;

    assert(time_delta != 0);
    if (time_delta == 0)
        return false;

    // We add time_delta / 2 so the result is rounded.
    *cpu = static_cast<double>(system_time_delta * 100 + time_delta / 2) / time_delta;
    last_system_time = system_time;
    last_time = time;
    return true;
}

} // namespace common

#else // linux platform

#include <sys/utsname.h>
#define LINUX_VERSION(x, y, z)   (0x10000*(x) + 0x100*(y) + z)

#ifndef AT_CLKTCK
#define AT_CLKTCK       17 // frequency of times()
#endif
#ifndef NOTE_NOT_FOUND
#define NOTE_NOT_FOUND  42
#endif


namespace common {

static inline int GetLinuxVersion()
{
    static struct utsname uts;
    int x = 0, y = 0, z = 0;    /* cleared in case sscanf() < 3 */

    if (uname(&uts) == -1)
        return -1;
    if (sscanf(uts.release, "%d.%d.%d", &x, &y, &z) == 3)
        return LINUX_VERSION(x, y, z);
    return 0;
}

static inline bool IsPathValid(char* path)
{
    struct stat statbuf;
    if (stat(path, &statbuf))
    {
        LOG(ERROR) << "stat error, permission problem, path=" << path
            << " , error=" << strerror(errno);
        return false;
    }

    return true;
}

static bool OpenAndReadFile(char* filename, char* buffer, int buff_len, int* bytes)
{
    int fd = open(filename, O_RDONLY, 0);
    if (fd == -1)
    {
        LOG(ERROR) << "open file error, filename=" << filename;
        return false;
    }

    *bytes = read(fd, buffer, buff_len - 1);
    close(fd);
    if(*bytes <= 0)
    {
        LOG(ERROR) << "read file error, bytes=" << *bytes;
        return false;
    }

    return true;
}

static void SkipBracket(char* buffer, char** skiped_start_point)
{
    *skiped_start_point = strchr(buffer, '(') + 1;
    char* d = strrchr(*skiped_start_point, ')');
    *skiped_start_point = d + 2;  // skip ") "
}

// For ELF executables, notes are pushed before environment and args
static unsigned long FindElfNote(unsigned long name)
{
    unsigned long *ep = (unsigned long *)environ;
    while (*ep) ep++;
    ep++;
    while (*ep)
    {
        if (ep[0] == name) return ep[1];
        ep += 2;
    }
    return NOTE_NOT_FOUND;
}

static unsigned long GetHertz()
{
    // Check the linux kernel version support
    if (GetLinuxVersion() <= LINUX_VERSION(2, 4, 0))
        return 0;
    unsigned long hertz = FindElfNote(AT_CLKTCK);
    return hertz != NOTE_NOT_FOUND ? hertz : 0;
}

static bool uptime(double *uptime_secs, double *idle_secs)
{
    int fd = open("/proc/uptime", O_RDONLY);
    if (fd == -1)
        return false;
    char buffer[2048] = {0};
    lseek(fd, 0L, SEEK_SET);
    int bytes = read(fd, buffer, sizeof(buffer) - 1);
    close(fd);
    if (bytes < 0)
        return false;
    buffer[bytes] = '\0';

    double up = 0, idle = 0;
    char* savelocale = setlocale(LC_NUMERIC, NULL);
    setlocale(LC_NUMERIC, "C");
    if (sscanf(buffer, "%lf %lf", &up, &idle) < 2)
    {
        setlocale(LC_NUMERIC, savelocale);
        return false;
    }
    setlocale(LC_NUMERIC, savelocale);
    if (uptime_secs) *uptime_secs = up;
    if (idle_secs) *idle_secs = idle;
    return true;
}

bool GetCpuUsageSinceLastCall(pid_t pid, double* cpu)
{
    char path[PATH_MAX];
    sprintf(path, "/proc/%d", pid);
    if (!IsPathValid(path))
    {
        return false;
    }

    int buff_len = 1024;
    char buffer[1024] = {0};
    int bytes = 0;
    char filename[PATH_MAX];
    sprintf(filename, "%s/%s", path, "stat");
    bool file_ret = OpenAndReadFile(filename, buffer, buff_len, &bytes);
    if (!file_ret)
    {
        return false;
    }
    buffer[bytes] = '\0';

    char* s = strchr(buffer, '(') + 1;
    char* d = strrchr(s, ')');
    s = d + 2;  // skip ") "

    std::vector<std::string> fields;
    SplitString(s, " ", &fields);
    uint64_t utime, stime, cutime, cstime, start_time;
    bool ret = StringToNumber(fields[11], &utime);
    ret = ret && StringToNumber(fields[12], &stime);
    ret = ret && StringToNumber(fields[13], &cutime);
    ret = ret && StringToNumber(fields[14], &cstime);
    ret = ret && StringToNumber(fields[19], &start_time);
    if (!ret)
        return false;

    double uptime_secs, idle_secs;
    if (!uptime(&uptime_secs, &idle_secs))
        return false;

    uint64_t seconds_since_boot = static_cast<unsigned long>(uptime_secs);
    // frequency of times()
    unsigned long hertz = GetHertz();
    if (hertz == 0) hertz = 100;
    // seconds of process life
    uint64_t seconds = seconds_since_boot - start_time / hertz;
    uint64_t total_time = utime + stime + cutime + cstime;

    // scaled %cpu, 999 means 99.9%
    *cpu = 0;
    if (seconds)
        *cpu = (total_time * 1000ULL / hertz) / seconds;
    *cpu = *cpu / 10;

    return true;
}

// For system cpu usage
bool GetTotalCpuTime(uint64_t* total_cpu_time)
{
    // Check the linux kernel version support
    if (GetLinuxVersion() < LINUX_VERSION(2, 6, 24))
    {
        return false;
    }

    // --- 0. check /proc permission
    char path[PATH_MAX];
    sprintf(path, "/%s", "proc");
    if (!IsPathValid(path))
    {
        return false;
    }

    // --- 1. read /proc/stat
    int buff_len = 1024;
    char buffer[1024] = {0};
    int bytes = 0;
    char filename[PATH_MAX];
    sprintf(filename, "/%s/%s", "proc", "stat");
    bool file_ret = OpenAndReadFile(filename, buffer, buff_len, &bytes);
    if (!file_ret)
    {
        return false;
    }
    buffer[bytes] = '\0';

    char* s = buffer;
    char* d = strchr(buffer, '\n');

    // --- 2. split string (user nice system idle iowait irq softirq stealstolen guest)
    std::vector<std::string> fields;
    SplitString(StringPiece(s, d - s), " ", &fields);

    uint64_t user, nice, system, idle, iowait, irq, softirq, stealstolen, guest;
    bool ret = StringToNumber(fields[1], &user);
    ret = ret && StringToNumber(fields[2], &nice);
    ret = ret && StringToNumber(fields[3], &system);
    ret = ret && StringToNumber(fields[4], &idle);
    ret = ret && StringToNumber(fields[5], &iowait);
    ret = ret && StringToNumber(fields[6], &irq);
    ret = ret && StringToNumber(fields[7], &softirq);
    ret = ret && StringToNumber(fields[8], &stealstolen);
    ret = ret && StringToNumber(fields[9], &guest);
    if (!ret)
    {
        LOG(ERROR) << "get param error, fields size=" << fields.size();
        return false;
    }

    *total_cpu_time= user + nice + system + idle + iowait +
                     irq + softirq + stealstolen + guest;

    return true;
}

bool GetProcessCpuTime(pid_t pid, uint64_t* process_cpu_time)
{
    // Check the linux kernel version support
    if (GetLinuxVersion() < LINUX_VERSION(2, 6, 24))
    {
        return false;
    }

    // --- 0. check /proc/pid permission
    char path[PATH_MAX];
    sprintf(path, "/proc/%d", pid);
    if (!IsPathValid(path))
    {
        return false;
    }

    int buff_len = 1024;
    char buffer[1024] = {0};
    int bytes = 0;
    char filename[PATH_MAX];
    sprintf(filename, "%s/%s", path, "stat");
    bool file_ret = OpenAndReadFile(filename, buffer, buff_len, &bytes);
    if (!file_ret)
    {
        return false;
    }
    buffer[bytes] = '\0';

    // skip ") "
    char* s = buffer;
    SkipBracket(buffer, &s);

    std::vector<std::string> fields;
    SplitString(s, " ", &fields);
    uint64_t utime, stime, cutime, cstime, start_time;
    bool ret = StringToNumber(fields[11], &utime);
    ret = ret && StringToNumber(fields[12], &stime);
    ret = ret && StringToNumber(fields[13], &cutime);
    ret = ret && StringToNumber(fields[14], &cstime);
    ret = ret && StringToNumber(fields[19], &start_time);
    if (!ret)
    {
        LOG(ERROR) << "get param error, fields size=" << fields.size();
        return false;
    }

    *process_cpu_time = utime + stime + cutime + cstime;
    return true;
}

bool GetThreadCpuTime(pid_t pid, int tid, uint64_t* thread_cpu_time)
{
    // Check the linux kernel version support
    if (GetLinuxVersion() < LINUX_VERSION(2, 6, 24))
    {
        return false;
    }

    // --- 0. check /proc/pid/task/tid permission
    char path[PATH_MAX];
    sprintf(path, "/proc/%d/task/%d", pid, tid);
    if (!IsPathValid(path))
    {
        return false;
    }

    // --- 1. read /proc/pid/task/tid/stat
    int buff_len = 1024;
    char buffer[1024] = {0};
    int bytes = 0;
    char filename[PATH_MAX];
    sprintf(filename, "%s/%s", path, "stat");

    bool file_ret = OpenAndReadFile(filename, buffer, buff_len, &bytes);
    if (!file_ret)
    {
        return false;
    }
    buffer[bytes] = '\0';

    // skip ") "
    char* s = buffer;
    SkipBracket(buffer, &s);

    std::vector<std::string> fields;
    SplitString(s, " ", &fields);
    uint64_t utime, stime;
    bool ret = StringToNumber(fields[11], &utime);
    ret = ret && StringToNumber(fields[12], &stime);
    if (!ret)
    {
        LOG(ERROR) << "get param error, fields size=" << fields.size();
        return false;
    }

    *thread_cpu_time = utime + stime;
    return true;
}

bool GetProcessCpuUsage(int32_t pid, uint64_t sample_period, double* cpu)
{
    // Check the linux kernel version support
    if (GetLinuxVersion() < LINUX_VERSION(2, 6, 24))
    {
        return false;
    }

    // first time
    uint64_t total_cpu_time_first = 0;
    bool ret = GetTotalCpuTime(&total_cpu_time_first);
    if (!ret)
    {
        LOG(ERROR) << "First GetTotalCpuTime error";
        return false;
    }

    uint64_t process_cpu_time_first = 0;
    ret = GetProcessCpuTime(pid, &process_cpu_time_first);
    if (!ret)
    {
        LOG(ERROR) << "First GetProcessCpuTime error";
        return false;
    }

    // usleep
    usleep(sample_period * 1000);

    // second time
    uint64_t total_cpu_time_second = 0;
    ret = GetTotalCpuTime(&total_cpu_time_second);
    if (!ret)
    {
        LOG(ERROR) << "Second GetTotalCpuTime error";
        return false;
    }

    uint64_t process_cpu_time_second = 0;
    ret = GetProcessCpuTime(pid, &process_cpu_time_second);
    if (!ret)
    {
        LOG(ERROR) << "Second GetProcessCpuTime error";
        return false;
    }

    *cpu = 0;
    if (total_cpu_time_second == total_cpu_time_first)
    {
        LOG(ERROR) << "GetTotalCpuTime same";
        return false;
    }

    *cpu = 100 * static_cast<double>(process_cpu_time_second - process_cpu_time_first) /
        (total_cpu_time_second - total_cpu_time_first) * GetLogicalCpuNumber();

    return true;
}

bool GetThreadCpuUsage(pid_t pid, int tid, uint64_t sample_period, double* cpu)
{
    // Check the linux kernel version support
    if (GetLinuxVersion() < LINUX_VERSION(2, 6, 24))
    {
        return false;
    }

    // first time
    uint64_t total_cpu_time_first = 0;
    bool ret = GetTotalCpuTime(&total_cpu_time_first);
    if (!ret)
    {
        LOG(ERROR) << "First GetTotalCpuTime error";
        return false;
    }

    uint64_t thread_cpu_time_first = 0;
    ret = GetThreadCpuTime(pid, tid, &thread_cpu_time_first);
    if (!ret)
    {
        LOG(ERROR) << "First GetThreadCpuTime error";
        return false;
    }

    // usleep
    usleep(sample_period * 1000);

    // second time
    uint64_t total_cpu_time_second = 0;
    ret = GetTotalCpuTime(&total_cpu_time_second);
    if (!ret)
    {
        LOG(ERROR) << "Second GetTotalCpuTime error";
        return false;
    }

    uint64_t thread_cpu_time_second = 0;
    ret = GetThreadCpuTime(pid, tid, &thread_cpu_time_second);
    if (!ret)
    {
        LOG(ERROR) << "Second GetThreadCpuTime error";
        return false;
    }

    *cpu = 0;
    if (total_cpu_time_second == total_cpu_time_first)
    {
        LOG(ERROR) << "GetTotalCpuTime same";
        return false;
    }

    *cpu = 100 * static_cast<double>(thread_cpu_time_second - thread_cpu_time_first) /
        (total_cpu_time_second - total_cpu_time_first) * GetLogicalCpuNumber();

    return true;
}


} // namespace common


#endif // end platform ifdef

bool GetCpuUsage(pid_t pid, double* cpu)
{
    return common::GetCpuUsageSinceLastCall(pid, cpu);
}
