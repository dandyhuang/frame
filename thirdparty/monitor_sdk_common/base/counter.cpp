//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-13 13:42
//  @file:      counter.cpp
//  @author:    
//  @brief:     
//
//********************************************************************


#include <algorithm>
#include "thirdparty/monitor_sdk_common/base/counter.h"
#include "thirdparty/glog/logging.h"

namespace common
{

RateCounter::RateCounter(uint32_t sample_interval_in_seconds)
                            : m_sample_interval(sample_interval_in_seconds),
                              m_start_idx(0), m_sum(0)
{
    CHECK_NE(m_sample_interval, 0u);
    m_interval_start = static_cast<uint32_t>(time(NULL));
    m_samples.resize(m_sample_interval, 0);
}

void RateCounter::Reset()
{
    MutexLocker locker(&m_mutex);
    std::fill(m_samples.begin(), m_samples.end(), 0);
    m_start_idx = 0;
    m_sum = 0;
    m_interval_start = static_cast<uint32_t>(time(NULL));
}

float RateCounter::GetRate()
{
    // Align interval to now
    Add(0);
    MutexLocker locker(&m_mutex);
    return static_cast<float>(m_sum)/m_sample_interval;
}

void RateCounter::Increment()
{
    Add(1);
}

void RateCounter::Add(uint32_t delta)
{
    MutexLocker locker(&m_mutex);
    uint32_t now = static_cast<uint32_t>(time(NULL));

    // System clock may be adjusted back
    if (now < m_interval_start)
    {
        LOG(WARNING) <<
                "Time(NULL) is less than interval start, system clock may be adjusted back.";
        // Reset the counter. m_interval_start == 0 will go to the next block
        // under if (now >= m_interval_start + 2 * m_sample_interval - 1)
        m_interval_start = 0;
    }

    // All old data should be retired
    if (now >= m_interval_start + 2 * m_sample_interval - 1)
    {
        std::fill(m_samples.begin(), m_samples.end(), 0);
        m_start_idx = 0;
        m_samples[0] = delta;
        m_sum = delta;
        m_interval_start = now;
        return;
    }

    for (uint32_t i = 0;; ++i)
    {
        // Retire old data
        if (i + m_interval_start + m_sample_interval <= now)
        {
            size_t idx = (m_start_idx + i) % m_samples.size();
            CHECK_GE(m_sum, m_samples[idx]);
            m_sum -= m_samples[idx];
            m_samples[idx] = 0;
        }
        else
        {
            m_interval_start = m_interval_start + i;
            m_start_idx = (m_start_idx + i) % m_samples.size();
            size_t cur_idx = (m_start_idx + now - m_interval_start) % m_samples.size();
            m_samples[cur_idx] += delta;
            m_sum += delta;
            break;
        }
    }
}

StatCounter::StatCounter(uint32_t max_sample_count)
                            : m_max_sample_count(max_sample_count),
                              m_sample_count(0), m_cur_idx(0), m_max_value(INT32_MIN),
                              m_min_value(INT32_MAX), m_sum(0)

{
    CHECK_NE(m_max_sample_count, 0u);
    m_samples.resize(m_max_sample_count, 0);
}

void StatCounter::Reset()
{
    MutexLocker locker(&m_mutex);
    std::fill(m_samples.begin(), m_samples.end(), 0);
    m_cur_idx = 0;
    m_sum = 0;
    m_max_value = INT32_MIN;
    m_min_value = INT32_MAX;
}

int32_t StatCounter::GetAverage() const
{
    MutexLocker locker(&m_mutex);
    if (m_sample_count == 0)
    {
        return 0;
    }
    return m_sum/static_cast<int32_t>(m_sample_count);
}

int32_t StatCounter::GetMax() const
{
    MutexLocker locker(&m_mutex);
    return m_max_value;
}

int32_t StatCounter::GetMin() const
{
    MutexLocker locker(&m_mutex);
    return m_min_value;
}

void StatCounter::AddSample(int32_t sample)
{
    MutexLocker locker(&m_mutex);
    m_max_value = std::max(m_max_value, sample);
    m_min_value = std::min(m_min_value, sample);
    // We need go through all elements except the one will be replaced this time
    if (m_samples[m_cur_idx] >= m_max_value)
    {
        m_max_value = sample;
        for (size_t i = 0; i < m_sample_count; ++i)
        {
            if (i == m_cur_idx)
            {
                continue;
            }
            m_max_value = std::max(m_max_value, m_samples[i]);
        }
    }
    if (m_samples[m_cur_idx] <= m_min_value)
    {
        m_min_value = sample;
        for (size_t i = 0; i < m_sample_count; ++i)
        {
            if (i == m_cur_idx)
            {
                continue;
            }
            m_min_value = std::min(m_min_value, m_samples[i]);
        }
    }

    m_sum = m_sum - m_samples[m_cur_idx] + sample;
    m_samples[m_cur_idx] = sample;
    m_cur_idx = (m_cur_idx + 1) % m_max_sample_count;
    m_sample_count = std::min(m_sample_count + 1, m_max_sample_count);
}

} // end of namespace common
