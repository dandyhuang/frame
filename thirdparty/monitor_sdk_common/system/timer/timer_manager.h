//********************************************************************
//  Copyright (c) 2013 Vivo.com, Inc. All Rights Reserved
//
//  @date:      2013-05-16 19:11
//  @file:      timer_manager.h
//  @author:    
//  @brief:     
//
//********************************************************************


#ifndef COMMON_SYSTEM_TIMER_TIMER_MANAGER_H
#define COMMON_SYSTEM_TIMER_TIMER_MANAGER_H
#pragma once

#include <functional>
#include <map>
#include <queue>
#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/base/closure2.h"
#include "thirdparty/monitor_sdk_common/base/deprecate.h"
#include "thirdparty/monitor_sdk_common/base/function.h"
#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/base/uncopyable.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/base_thread.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/condition_variable.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"
#include "thirdparty/monitor_sdk_common/system/time/timestamp.h"

namespace common {

class TimerManager : private BaseThread
{
    COMMON_DECLARE_UNCOPYABLE(TimerManager);
public:
    explicit TimerManager(const ::std::string& name);
    TimerManager();
    ~TimerManager();

    struct Stats
    {
        size_t  oneshot_timer_num;
        size_t  period_timer_num;
        int64_t estimate_runover_time;
    };

public:
    /// timer callback closure, input param: Timer ID.
    typedef Closure<void, uint64_t> CallbackClosure;
    DEPRECATED_BY(CallbackClosure) typedef CallbackClosure CallBackClosure;

    /// timer callback closure, input param: Timer ID.
    typedef Function<void (uint64_t timer_id)> CallbackFunction;

    /// @brief add a one-shot timer
    /// @param interval  timer interval, in milliseconds
    /// @param closure callback closure, should be self delete
    /// @return unique timer id
    uint64_t AddOneshotTimer(int64_t interval, CallbackClosure* closure);

    /// @brief add a one-shot timer
    /// @param interval  timer interval, in milliseconds
    /// @param closure callback closure, should be self delete
    /// @return unique timer id
    uint64_t AddOneshotTimer(int64_t interval, const CallbackFunction& callback);

    /// @brief add a periodly run timer
    /// @param interval  timer interval, in milliseconds
    /// @param closure callback closure, should be permanent
    /// @return unique timer id
    uint64_t AddPeriodTimer(int64_t interval, CallbackClosure* closure);

    /// @brief add a periodly run timer
    /// @param interval  timer interval, in milliseconds
    /// @param closure callback closure, should be permanent
    /// @return unique timer id
    uint64_t AddPeriodTimer(int64_t interval, const CallbackFunction& callback);

    /// @brief remove a timer asynchronous
    bool AsyncRemoveTimer(uint64_t id, bool* is_running = NULL);

    /// @brief remove a timer synchronous, if timer is running, wait for complete
    bool RemoveTimer(uint64_t id);

    /// @brief modify a timer using new interval and callback closure
    /// @param interval new timer interval, in milliseconds
    /// @param closure new closure, should be different with old closure.
    /// old closure will be deleted.
    bool ModifyTimer(uint64_t id, int64_t interval, CallbackClosure* closure);

    /// @brief modify a timer using new interval and callback closure
    /// @param interval new timer interval, in milliseconds
    /// @param closure new closure, should be different with old closure.
    /// old closure will be deleted.
    bool ModifyTimer(uint64_t id, int64_t interval, const CallbackFunction& callback);

    /// @brief modify a timer using new time interval
    bool ModifyTimer(uint64_t id, int64_t interval);

    /// @brief disable a timer
    bool DisableTimer(uint64_t id);

    /// @brief enable a timer
    bool EnableTimer(uint64_t id);

    /// @brief stats info of timer manager
    void GetStats(Stats* stats) const;

    /// @brief stop the timer dispatch thread and clear all timers
    void Stop();

    /// @brief Whether Stop has been called
    bool IsStoped() const;

    /// @brief remove all timers in current timer manager
    void Clear();

    /// singleton interface to obtain a global shared default instance
    static TimerManager& DefaultInstance();

private:
    struct TimerEntry
    {
        TimerEntry() :
            closure(NULL), interval(0),
            is_enabled(false), is_period(false),
            revision(0) {}
        CallbackClosure* closure; ///< callback closure
        CallbackFunction callback; ///< callback function
        int64_t interval;     ///< timer interval
        bool    is_enabled;   ///< whether timer is enabled
        bool    is_period;    ///< whether timer run periodly
        uint8_t revision;     ///< modified revision
    };

    struct Timeout
    {
        int64_t  time;        ///< time when the timer triggers
        uint64_t timer_id;    ///< timer id
        uint8_t  revision;    ///< timer revision, old revisions will be discarded
        bool operator > (const Timeout& rhs) const
        {
            return time > rhs.time;
        }
    };

    typedef ::std::map<uint64_t, TimerEntry> TimerMap;

private:
    /// @brief timer thread start
    void StartThread();

    /// @brief timer thread entrance.
    virtual void Entry();

    TimerEntry* FindEntry(uint64_t id);

    bool DequeueTimeoutEntry(uint64_t* id, TimerEntry* entry);

    bool AsyncRemoveTimerInLock(uint64_t id);

    /// @brief process timeouts
    void  Dispatch();

    /// @brief auto generate a new timer id
    uint64_t NewTimerId();

    static int64_t GetCurrentTime()
    {
        return GetTimeStampInMs();
    }

    bool GetLatestTime(int64_t* time) const;

    /// @brief get the latest timeout in the timers
    bool GetLatestTimeout(int64_t* time) const;

    /// @brief add next timeout to the timeout list.
    void SetNextTimeout(uint64_t id, int64_t interval, uint8_t revision);

    void PushNextTimeout(uint64_t id, int64_t interval, uint8_t revision);

    /// @brief internal use. add a timer.
    uint64_t AddTimer(
        int64_t interval, bool is_period,
        CallbackClosure* closure,
        const CallbackFunction& callback);

private:
    const ::std::string m_name;
    mutable Mutex m_mutex;
    ConditionVariable  m_cond;
    TimerMap m_timers; ///< times container
    /// timeouts queue, latest timeout first.
    ::std::priority_queue<Timeout, ::std::vector<Timeout>, ::std::greater<Timeout> > m_timeouts;
    volatile uint64_t m_running_timer;
};

} // end of namespace common


#endif // COMMON_SYSTEM_TIMER_TIMER_MANAGER_H
