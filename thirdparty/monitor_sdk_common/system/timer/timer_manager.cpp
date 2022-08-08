//********************************************************************
//  Copyright (c) 2013 dandy.com, Inc. All Rights Reserved
//
//  @date:      2013-05-16 19:13
//  @file:      timer_manager.cpp
//  @author:
//  @brief:
//
//********************************************************************


#include <limits.h>
#include <map>
#include <queue>
#include <vector>

#include "thirdparty/monitor_sdk_common/base/closure2.h"
#include "thirdparty/monitor_sdk_common/base/singleton2.h"
#include "thirdparty/monitor_sdk_common/base/stdint.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/atomic/atomic.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/base_thread.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/condition_variable.h"
#include "thirdparty/monitor_sdk_common/system/concurrency/mutex.h"
#include "thirdparty/monitor_sdk_common/system/timer/timer_manager.h"


namespace common {

TimerManager::TimerManager(const ::std::string& name) :
    m_name(name),
    m_running_timer(0)
{
    StartThread();
}

TimerManager::TimerManager() : m_name("unnamed"), m_running_timer(0)
{
    StartThread();
}

uint64_t TimerManager::AddTimer(
    int64_t interval, bool is_period,
    CallbackClosure* closure,
    const CallbackFunction& callback)
{
    assert(interval >= 0);
    assert(!IsStoped());

    MutexLocker lock(m_mutex);
    uint64_t id = NewTimerId();
    TimerEntry& timer = m_timers[id];
    timer.interval = interval;
    timer.is_period = is_period;
    timer.closure = closure;
    timer.callback = callback;
    timer.is_enabled = true;

    SetNextTimeout(id, interval, 0);

    return id;
}

uint64_t TimerManager::AddOneshotTimer(int64_t interval,
                                       CallbackClosure* closure)
{
    assert(closure != NULL);
    assert(closure->IsSelfDelete());
    return AddTimer(interval, false, closure, NULL);
}

uint64_t TimerManager::AddOneshotTimer(int64_t interval,
                                       const CallbackFunction& callback)
{
    return AddTimer(interval, false, NULL, callback);
}

uint64_t TimerManager::AddPeriodTimer(int64_t interval,
                                      CallbackClosure* closure)
{
    assert(closure != NULL);
    assert(!closure->IsSelfDelete());
    return AddTimer(interval, true, closure, NULL);
}

uint64_t TimerManager::AddPeriodTimer(int64_t interval,
                                      const CallbackFunction& callback)
{
    return AddTimer(interval, true, NULL, callback);
}

bool TimerManager::AsyncRemoveTimerInLock(uint64_t id)
{
    TimerMap::iterator it = m_timers.find(id);
    if (it != m_timers.end()) {
        delete it->second.closure;
        m_timers.erase(it);
        return true;
    }
    return false;
}

bool TimerManager::AsyncRemoveTimer(uint64_t id, bool* is_running)
{
    assert(!IsStoped());
    MutexLocker lock(m_mutex);
    bool success = AsyncRemoveTimerInLock(id);
    if (success && is_running != NULL)
        *is_running = m_running_timer != 0 && (m_running_timer == id);
    return success;
}

bool TimerManager::RemoveTimer(uint64_t id)
{
    assert(!IsStoped());

    if (id == 0) { // 0 is an invalid id.
        return false;
    }
    // to avoid deadlock, we should not wait if this function is called in
    // timer thread.
    // AsyncRemoveTimer is safe and semantic correct in this condition.
    if (ThisThread::GetId() == this->GetId())
        return AsyncRemoveTimer(id);

    for (;;) {
        {
            MutexLocker locker(m_mutex);
            if (m_running_timer != id)
                return AsyncRemoveTimerInLock(id);
        }
        // busy wait is ok for this rare case
        ThisThread::Sleep(1);
    }
}

bool TimerManager::ModifyTimer(uint64_t id, int64_t interval,
                               CallbackClosure* closure)
{
    assert(!IsStoped());
    assert(closure != NULL);

    MutexLocker lock(m_mutex);
    TimerEntry* entry = FindEntry(id);
    if (entry) {
        entry->interval = interval;
        if (entry->closure != closure) { // check for safety
            delete entry->closure; // release old closure
            entry->closure = closure;
        }
        entry->callback = NULL; // if callback type is Function
        entry->revision++;
        if (entry->is_enabled) {
            SetNextTimeout(id, interval, entry->revision);
        }
        return true;
    }
    return false;
}

bool TimerManager::ModifyTimer(uint64_t id, int64_t interval,
                               const CallbackFunction& callback)
{
    assert(!IsStoped());

    MutexLocker lock(m_mutex);
    TimerEntry* entry = FindEntry(id);
    if (entry) {
        entry->interval = interval;
        entry->callback = callback;
        delete entry->closure; // release closure if exist
        entry->closure = NULL;
        entry->revision++;
        if (entry->is_enabled) {
            SetNextTimeout(id, interval, entry->revision);
        }
        return true;
    }
    return false;
}

bool TimerManager::ModifyTimer(uint64_t id, int64_t interval)
{
    assert(!IsStoped());

    MutexLocker lock(m_mutex);
    TimerEntry* entry = FindEntry(id);
    if (entry) {
        entry->interval = interval;
        entry->revision++;
        if (entry->is_enabled) {
            SetNextTimeout(id, interval, entry->revision);
        }
        return true;
    }
    return false;
}

bool TimerManager::DisableTimer(uint64_t id)
{
    assert(!IsStoped());

    MutexLocker lock(m_mutex);
    TimerEntry* entry = FindEntry(id);
    if (entry) {
        entry->is_enabled = false;
        entry->revision++;
        return true;
    }
    return false;
}

bool TimerManager::EnableTimer(uint64_t id)
{
    assert(!IsStoped());

    MutexLocker lock(m_mutex);
    TimerEntry* entry = FindEntry(id);
    if (entry) {
        entry->is_enabled = true;
        SetNextTimeout(id, entry->interval, entry->revision);
        return true;
    }
    return false;
}

// -------------------------------------------------------
// the functions below are declared private
// mutex is not needed because they are aready under lock.
// -------------------------------------------------------
TimerManager::TimerEntry* TimerManager::FindEntry(uint64_t id)
{
    // assert(m_mutex.IsLocked());

    TimerMap::iterator it = m_timers.find(id);
    if (it != m_timers.end())
        return &it->second;
    return NULL;
}

bool TimerManager::GetLatestTime(int64_t* time) const
{
    // assert(m_mutex.IsLocked());

    if (m_timeouts.empty())
        return false;

    *time = m_timeouts.top().time;
    return true;
}

bool TimerManager::GetLatestTimeout(int64_t* time) const
{
    // assert(m_mutex.IsLocked());

    if (GetLatestTime(time)) {
        // convert to relative time
        int64_t now = GetCurrentTime();
        if (*time < now)
            *time = 0;
        else
            *time -= now;
        return true;
    }

    return false;
}

uint64_t TimerManager::NewTimerId()
{
    static volatile uint64_t timer_id = 0;
    return AtomicIncrement(&timer_id);
}

void TimerManager::PushNextTimeout(uint64_t id, int64_t interval, uint8_t revision)
{
    Timeout timeout = { GetCurrentTime() + interval, id, revision };
    m_timeouts.push(timeout);
}

void TimerManager::SetNextTimeout(uint64_t id, int64_t interval, uint8_t revision)
{
    // assert(m_mutex.IsLocked());

    int64_t prev_top_time;

    if (ThisThread::GetId() == GetId()) {
        PushNextTimeout(id, interval, revision);
    } else if (GetLatestTime(&prev_top_time)) {
        PushNextTimeout(id, interval, revision);
        int64_t new_top_time = m_timeouts.top().time;
        // reschedule timer only if the new time is the most early
        if (new_top_time < prev_top_time)
            m_cond.Signal();
    } else {
        // timer manager is empty, worker thread is waiting infinitely
        PushNextTimeout(id, interval, revision);
        m_cond.Signal();
    }
}

// -------------------------------------------------------

void TimerManager::Entry()
{
    for (;;) {
        {
            MutexLocker lock(m_mutex);
            if (IsStopRequested())
                return;

            int64_t time;
            if (GetLatestTimeout(&time))
                m_cond.Wait(m_mutex, static_cast<int>(time));
            else
                m_cond.Wait(m_mutex);
        }
        Dispatch();
    }
}

void TimerManager::StartThread()
{
    if (!BaseThread::Start())
    {
        ::std::string msg = strerror(errno);
        throw ::std::runtime_error("Can't start timer thread: " + msg);
    }
}

bool TimerManager::IsStoped() const
{
    // NOTE: BaseThread::Running is not reliable, because thread maybe already
    // created but not runing
    return GetId() <= 0;
}

void TimerManager::Stop()
{
    if (!IsStoped()) {
        {
            MutexLocker lock(m_mutex);
            SendStopRequest();
            m_cond.Signal();
        }
        if (IsJoinable()) {
            Join();
        }
        Clear();
    }
}

bool TimerManager::DequeueTimeoutEntry(uint64_t* id, TimerEntry* entry)
{
    for (;;) {
        MutexLocker locker(m_mutex);

        if (m_timeouts.empty() || m_timeouts.top().time > GetCurrentTime())
            return false;

        Timeout timeout = m_timeouts.top();
        m_timeouts.pop();

        TimerMap::iterator it = m_timers.find(timeout.timer_id);
        if (it == m_timers.end())
            continue;

        // ignore outdated timeouts
        if (!it->second.is_enabled || timeout.revision != it->second.revision)
            continue;

        *id = timeout.timer_id;
        *entry = it->second;

        if (entry->is_period) {
            // Move the closure into entry temporary during running,
            // make sure closure will not be deleted in case ModifyTimer is called
            // with a difference closure.
            it->second.closure = NULL;
        } else {
            m_timers.erase(it);
        }

        m_running_timer = *id;
        break;
    }

    return true;
}

void TimerManager::Dispatch()
{
    uint64_t id;

    // Make a copy. It's necessary because the entry in the map maybe changed
    // by it's callback.
    TimerEntry entry;

    while (DequeueTimeoutEntry(&id, &entry)) {
        // Run the callback in unlocked state
        if (entry.callback) {
            entry.callback(id);
        } else if (entry.closure) {
            entry.closure->Run(id);
        }

        MutexLocker locker(m_mutex);
        m_running_timer = 0;
        if (entry.is_period) {
            TimerMap::iterator it = m_timers.find(id);
            if (it != m_timers.end()) {
                // Restore closure if necessary
                if (entry.closure) {
                    if (it->second.closure == NULL) {
                        it->second.closure = entry.closure;
                    }
                    else if (entry.closure != it->second.closure) {
                        // The entry in the map has be assigned with a new,
                        // make the old closure useless now
                        delete entry.closure;
                    }
                }

                if (it->second.is_enabled) {
                    SetNextTimeout(it->first, it->second.interval,
                                   it->second.revision);
                }
            } else {
                // Timer has already been removed.
                delete entry.closure;
            }
        }
    }
}

void TimerManager::Clear()
{
    MutexLocker locker(m_mutex);
    while (!m_timeouts.empty())
        m_timeouts.pop();

    TimerMap::iterator it;
    for (it = m_timers.begin(); it != m_timers.end(); ++it) {
        delete it->second.closure;
    }
    m_timers.clear();
}

TimerManager::~TimerManager()
{
    Stop();
}

void TimerManager::GetStats(TimerManager::Stats* stats) const
{
    MutexLocker lock(m_mutex);
    int64_t time = 0;
    stats->oneshot_timer_num = 0;
    stats->period_timer_num = 0;
    stats->estimate_runover_time = 0;
    TimerMap::const_iterator it;
    for (it = m_timers.begin(); it != m_timers.end(); ++it) {
        if (it->second.is_period) {
            stats->period_timer_num++;
            time = LLONG_MAX;
        } else {
            if (it->second.interval > time)
                time = it->second.interval;
            stats->oneshot_timer_num++;
        }
    }
    stats->estimate_runover_time = time;
}

TimerManager& TimerManager::DefaultInstance()
{
    return Singleton<TimerManager>::Instance("default");
}

} // end of namespace common

