//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 16:12
//  @file:      array_size.h
//  @author:
//  @brief:
//
//********************************************************************

#ifndef COMMON_BASE_COUNTER_H
#define COMMON_BASE_COUNTER_H

#include <vector>
#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"

namespace common
{

// RateCounter returns average rate of samples in the interval
// of [now - sample_interval + 1, now]
// You can add samples via Increment and Add functions.
// Delta in Add function should be non-negative.
// Reset function will reset the counter explicitly before you reuse it
class RateCounter
{
public:
    explicit RateCounter(uint32_t sample_interval_in_seconds);
    void Reset();
    // Returns average rate in the interval of [now - sample_interval + 1, now]
    // i.e. sample total number in the interval divide sample_interval
    float GetRate();
    void Increment();
    void Add(uint32_t delta);

private:
    mutable Mutex m_mutex;
    uint32_t m_sample_interval;
    uint32_t m_interval_start;
    uint32_t m_start_idx;
    uint32_t m_sum;
    std::vector<uint32_t> m_samples;
};

// StatCounter support Average, Max and Min statistics info for serial samples
// You can add a sample via AddSample(int32_t)
class StatCounter
{
public:
    explicit StatCounter(uint32_t max_sample_count);
    void Reset();
    int32_t GetAverage() const;
    int32_t GetMax() const;
    int32_t GetMin() const;
    void AddSample(int32_t sample);

private:
    mutable Mutex m_mutex;
    uint32_t m_max_sample_count;
    uint32_t m_sample_count;
    uint32_t m_cur_idx;
    int32_t m_max_value;
    int32_t m_min_value;
    int32_t m_sum;
    std::vector<int32_t> m_samples;
};

} // end of namespace common

#endif // COMMON_BASE_COUNTER_H
