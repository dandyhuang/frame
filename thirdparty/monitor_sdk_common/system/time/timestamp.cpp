//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 19:20
//  @file:      timestamp.cpp
//  @brief:     
//
//********************************************************************

#include "thirdparty/monitor_sdk_common/system/time/timestamp.h"
#include <stddef.h>
#include "thirdparty/monitor_sdk_common/system/concurrency/sequence_lock.h"

#ifdef _WIN32

#include "thirdparty/monitor_sdk_common/base/common_windows.h"

namespace common {

// Number of 100 nanosecond units from 1/1/1601 to 1/1/1970
#define EPOCH_BIAS  116444736000000000i64

int64_t GetTimeStampInUs()
{
    ULARGE_INTEGER uli;
    GetSystemTimeAsFileTime(reinterpret_cast<FILETIME*>(&uli));
    return (uli.QuadPart - EPOCH_BIAS) / 10;
}

int64_t GetTimeStampInMs()
{
    return GetTimeStampInUs() / 1000;
}

} // end of namespace common

#endif

#ifdef __unix__

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/utsname.h>

namespace common {

/// get system time, in microseconds
int64_t SystemGetTimeStampInUs()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t result = tv.tv_sec;
    result *= 1000000;
    result += tv.tv_usec;
    return result;
}

/// read cpu timestamp count
static inline int64_t rdtsc()
{
    int64_t tsc;
#if defined __i386__
    __asm__ __volatile__("rdtsc" : "=A" (tsc));
#elif defined __x86_64
    unsigned int a, d;
    __asm__ __volatile__("rdtsc" : "=a" (a), "=d" (d));
    tsc = static_cast<int64_t>(a) | (static_cast<int64_t>(d) << 32);
#else
#error unsupported platform
#endif
    return tsc;
}

/// @brief get both timestamp and tsc
/// @retval true no context switch, timestamp and tsc same timeslice
/// @retval false context switched, only timestamp is trustable
static bool get_timestamp_and_tsc(int64_t* timestamp, int64_t* tsc)
{
    int64_t tsc1 = rdtsc();
    *timestamp = SystemGetTimeStampInUs();
    int64_t tsc2 = rdtsc();

    // the difference is typical several hundreds if no context switching
    if (tsc2 > tsc1 && tsc2 - tsc1 < 50000)
    {
        // no context switched between 2 rdtsc() call
        // rounded avarage value
        *tsc = tsc1 + (tsc2 - tsc1 + 1) / 2;
        return true;
    }
    else
    {
        // context switched between 2 rdtsc() callings,
        // can not decide which tsc is useable
        return false;
    }
}

// using tsc to optimize timestamp, adjust if necessary
int64_t FastGetTimeStampInUs()
{
    static ::common::SequenceLock s_timestamp_lock;

    static int64_t s_last_timestamp = 0; // last sample time
    static int64_t s_last_tsc = 0; // last sample time tsc
    static int64_t s_readjust_tsc_interval = 0;
    static int64_t s_shifted_us_per_tsc = 0;

    int64_t last_timestamp;
    int64_t last_tsc;
    int64_t readjust_tsc_interval = 0;
    int64_t shifted_us_per_tsc = 0;

    int seq;
    do {
        seq = s_timestamp_lock.BeginRead();
        last_timestamp = s_last_timestamp;
        last_tsc = s_last_tsc;
        readjust_tsc_interval = s_readjust_tsc_interval;
        shifted_us_per_tsc = s_shifted_us_per_tsc;
    } while (!s_timestamp_lock.EndRead(seq));

    int64_t tsc;
    int64_t timestamp;

    if (last_timestamp != 0)
    {
        const int64_t READJUST_INTERVAL = 1000; // 1ms

        // using integer multiple and shift to eliminate float overhead
        const int64_t SHIFT = 32;

        tsc = rdtsc();
        if (tsc > last_tsc && tsc - last_tsc < readjust_tsc_interval)
        {
            int64_t tsc_diff = tsc - last_tsc;
            int64_t shifted_diff = tsc_diff * shifted_us_per_tsc + (1LL << (SHIFT - 1));
            int64_t timestamp_diff = shifted_diff >> SHIFT;
            timestamp = last_timestamp + timestamp_diff;
        }
        else
        {
            // quite long after last gettimestamp() call, readjust
            if (get_timestamp_and_tsc(&timestamp, &tsc))
            {
                if (timestamp > last_timestamp && tsc > last_tsc)
                {
                    // recalculate tsc ratio
                    double timestamp_diff = timestamp - last_timestamp;
                    double tsc_diff = tsc - last_tsc;
                    double us_per_tsc = timestamp_diff / tsc_diff;
                    shifted_us_per_tsc = static_cast<int64_t>(us_per_tsc * (1LL << SHIFT) + 0.5);
                    if (timestamp - last_timestamp > READJUST_INTERVAL)
                    {
                        readjust_tsc_interval =
                            static_cast<int64_t>(tsc_diff * READJUST_INTERVAL / timestamp_diff);
                    }
                    else
                    {
                        readjust_tsc_interval = static_cast<int64_t>(2 * tsc_diff);
                    }

                    // Try update last adjusted time, if writer lock can't be
                    // obtain, it means other thread is updating, it's not a
                    // problem.
                    common::SequenceLock::TryWriterLocker try_locker(&s_timestamp_lock);
                    if (try_locker.IsLocked() && timestamp > s_last_timestamp)
                    {
                        s_last_timestamp = timestamp;
                        s_last_tsc = tsc;
                        s_readjust_tsc_interval = readjust_tsc_interval;
                        s_shifted_us_per_tsc = shifted_us_per_tsc;
                    }
                }
            }
        }
    }
    else
    {
        // first call, last timestamp is unknown
        if (get_timestamp_and_tsc(&timestamp, &tsc))
        {
            common::SequenceLock::TryWriterLocker try_locker(&s_timestamp_lock);
            if (try_locker.IsLocked() && timestamp > s_last_timestamp)
            {
                s_last_timestamp = timestamp;
                s_last_tsc = tsc;
            }
        }
    }

    return timestamp;
}

// if kernel support vsyscall64, time call will be 10 times faster.
//  check it.
int get_vsyscall64_value()
{
    int result = -1;
    int fd = open("/proc/sys/kernel/vsyscall64", O_RDONLY, 0);
    if (fd >= 0)
    {
        char buf[2];
        int num_read = read(fd, buf, 2);
        if (num_read >= 1)
        {
            switch (buf[0])
            {
            case '0':
            case '1':
            case '2':
                result = buf[0] - '0';
            }
        }
        close(fd);
    }
    return result;
}

static bool HasFastGetTimeOfDay()
{
#ifdef __x86_64__
    // For x86_64 application, as we know, gettimeofday become fast since at
    // least 2.6.32. It cost 47ms to call 1000000 times on this machine
    // (Intel(R) Xeon(R) CPU L5630  @ 2.13GHz).
    utsname u;
    if (uname(&u) < 0)
        return false;
    int major, minor, patch;
    if (sscanf(u.release, "%d.%d.%d", &major, &minor, &patch) != 3)
        return false;
    int version = major * 10000 + minor * 100 + patch;
    if (version < 20632) // 2.6.32
        return false;
    if (strcmp(u.machine, "x86_64") != 0)
        return false;
    return true;
#endif

    // But 32 bit is still not fast enough, It cost about 120ms to call
    // 1000000 times on the same above machine.
    return false;
}

static int fast_gettimeofday = HasFastGetTimeOfDay() ? 1 : -1;

int64_t GetTimeStampInUs()
{
    // Check if fast_gettimeofday has not been initialized. 0 means not
    // initialized.
    if (__builtin_expect(fast_gettimeofday == 0, 0))
        fast_gettimeofday = HasFastGetTimeOfDay() ? 1 : -1;

    if (fast_gettimeofday > 0)
        return SystemGetTimeStampInUs();
    else
        return FastGetTimeStampInUs();
}

int64_t GetTimeStampInMs()
{
    int64_t timestamp = GetTimeStampInUs();
    return timestamp / 1000;
}

} // end of namespace common

#endif // __unix__

