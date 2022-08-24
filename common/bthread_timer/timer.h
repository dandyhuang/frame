/***************************************************************************
 *
 * Copyright (c) 2021 Vivo.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

 /**
 * @file timer/timer.h
 * @author xiangping.du
 * @date 2021/09/26 10:46:07
 * @version 1.0
 * @brief
 *
 **/
#pragma once

#include "bthread/timer_thread.h"

namespace predictor {
namespace timer {
class Timer {
template<class Callable>
class TimerTask {
 public:
    TimerTask(bthread::TimerThread* ptd, Callable callable):
        _ptd(ptd), _callable(callable) {}
    void schedule() {
        _task_id = _ptd->schedule(Timer::routine<Callable>, this, _tp);
    }
    void set_tp(timespec tp) {
        _tp = tp;
    }
    void set_interval(int64_t interval_ms) {
        _interval_ms = interval_ms;
        set_tp(butil::milliseconds_from_now(interval_ms));
    }
    void run() {
        // should be a fast callable, ignore the timecost.
        _callable();
        repeat_if_need();
    }
    void repeat_if_need() {
        if (_interval_ms != -1) {
            _tp = butil::milliseconds_from_now(_interval_ms);
            schedule();
        }
    }
    bool deleteable() {
        return _interval_ms == -1;
    }

 private:
    bthread::TimerThread::TaskId _task_id;
    bthread::TimerThread* _ptd;
    Callable _callable;
    timespec _tp;
    int64_t _interval_ms = -1;
};

 public:
    void run() {
        _timer_thread.start(NULL);
    }
    void stop_and_join() {
        _timer_thread.stop_and_join();
    }

    template<class Callable>
    void schedule_after(timespec s, const Callable& runable) {
        TimerTask<Callable>* task =
            new TimerTask<Callable>(&_timer_thread, runable);
        task->set_tp(s);
        task->schedule();
    }
    template<class Callable>
    void schedule_after_ms(size_t ms, const Callable& runable) {
        timespec tp = butil::milliseconds_from_now(ms);
        schedule_after(tp, runable);
    }
    template<class Callable>
    void schedule_after_s(size_t seconds, const Callable& runable) {
        timespec tp = butil::seconds_from_now(seconds);
        schedule_after(tp, runable);
    }

    template<class Callable>
    void schedule_interval_ms(size_t ms, const Callable& runable) {
        TimerTask<Callable>* task =
            new TimerTask<Callable>(&_timer_thread, runable);
        task->set_interval(ms);
        task->schedule();
    }
    template<class Callable>
    void schedule_interval_s(size_t s, const Callable& runable) {
        schedule_interval_ms(s * 1000, runable);
    }

    template<class Callable>
    static void routine(void* args) {
        TimerTask<Callable>* t = (TimerTask<Callable>*)args;
        t->run();
        if (t->deleteable()) {
            delete t;
        }
    }

 private:
    bthread::TimerThread _timer_thread;
};

}  // namespace timer
}  // namespace predictor
