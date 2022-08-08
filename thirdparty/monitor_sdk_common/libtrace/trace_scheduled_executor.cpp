//
// Created by 王锦涛 on 2020-2-12.
//

#include "thirdparty/monitor_sdk_common/libtrace/trace_scheduled_executor.h"
#include "thirdparty/monitor_sdk_common/system/threading/this_thread.h"

using namespace common;

TraceScheduledExecutor::TraceScheduledExecutor() {
    TouchThreadPool();
}


TraceScheduledExecutor::~TraceScheduledExecutor() {
    Shutdown();
}

void TraceScheduledExecutor::Execute(const std::function<void (bool *)>& callback) {
    TouchThreadPool();

    auto task_func = [this, callback] {
        if (this->terminated_) return;
        callback(&this->terminated_);
    };

    thread_pool_->AddTask(task_func, cur_dispatch_key_ ++);
    cur_dispatch_key_ = cur_dispatch_key_ % thread_pool_num_;
}

void TraceScheduledExecutor::ScheduleWithFixedDelay(const std::function<void (bool *)>& callback, int initial_delay, int scheduled_interval) {
    TouchThreadPool();

    if (this->terminated_) return;

    auto task_func = [this, callback, initial_delay, scheduled_interval] {

        // 初始睡眠时间
        int delay_count = initial_delay;
        while (delay_count > 0 && !this->terminated_) {
            ThisThread::Sleep(1);
            delay_count --;
        }

        // 定时循环
        while (true) {
            if (this->terminated_) break;
            callback(&this->terminated_);
            if (this->terminated_) break;

            // 间隔睡眠时间
            int interval_count = scheduled_interval;
            while (interval_count > 0 && !this->terminated_) {
                ThisThread::Sleep(1);
                interval_count --;
            }
        }
    };

    thread_pool_->AddTask(task_func, cur_dispatch_key_ ++);
    cur_dispatch_key_ = cur_dispatch_key_ % thread_pool_num_;
}

void TraceScheduledExecutor::Shutdown() {
    terminated_ = true;
    thread_pool_->Terminate();
    thread_pool_ = nullptr;
    cur_dispatch_key_ = 0;
}

void TraceScheduledExecutor::Restart() {
    Shutdown();
    thread_pool_ = nullptr;
    TouchThreadPool();
}

bool TraceScheduledExecutor::IsTerminated() {
    return terminated_;
}

void TraceScheduledExecutor::TouchThreadPool() {
    if (thread_pool_ == nullptr) {
        thread_pool_ = std::unique_ptr<ThreadPool>(new ThreadPool(thread_pool_num_));
        cur_dispatch_key_ = 0;
        terminated_ = false;
    }
}