//
// Created by wuting
//

#include "thirdparty/monitor_sdk_common/libvep/vep_scheduled_executor.h"
#include "thirdparty/monitor_sdk_common/system/threading/this_thread.h"

#include "thirdparty/glog/logging.h"

using namespace common;

VepScheduledExecutor::VepScheduledExecutor() {
    TouchThreadPool();
}

VepScheduledExecutor::VepScheduledExecutor(int thread_pool_num) {
    thread_pool_num_ = thread_pool_num;
    TouchThreadPool();
}


VepScheduledExecutor::~VepScheduledExecutor() {
    if (!IsTerminated()) {
        Shutdown();
    }
}

/**
 * 立即执行调度【逐个向线程池加入任务，任务只执行一次】
 */
void VepScheduledExecutor::Execute(const std::function<void (bool *)>& callback) {
    TouchThreadPool();

    auto task_func = [this, callback] {
    if (this->terminated_) {
        return;
    }
    callback(&this->terminated_);
    };

    thread_pool_->AddTask(task_func, cur_dispatch_key_ ++);
    cur_dispatch_key_ = cur_dispatch_key_ % thread_pool_num_;
}

/**
 * 定时执行调度【任务重复定时执行】
 * @param callback回调函数
 * @param initial_delay一开始的间隔期
 * @param scheduled_interval 固定的间隔期
 */
void VepScheduledExecutor::ScheduleWithFixedDelay(const std::function<void (bool *)>& callback,
                                                       int initial_delay, int scheduled_interval) {
    TouchThreadPool();

    if (terminated_) {
        return;
    }
    if (cur_dispatch_key_ >= thread_pool_num_) {
        return;
    }

    auto task_func = [this, callback, initial_delay, scheduled_interval] {
        ThisThread::Sleep(initial_delay);

        while (true) {
            if (this->terminated_) {
                break;
            }
            callback(&this->terminated_);
            if (this->terminated_) {
                break;
            }
            ThisThread::Sleep(scheduled_interval);
        }
    };

    thread_pool_->AddTask(task_func, cur_dispatch_key_);
    cur_dispatch_key_++;
}

void VepScheduledExecutor::Shutdown() {
    terminated_ = true;
    thread_pool_->Terminate();
    thread_pool_ = nullptr;
    cur_dispatch_key_ = 0;
}


void VepScheduledExecutor::Restart() {
    Shutdown();
    thread_pool_ = nullptr;
    TouchThreadPool();
}


bool VepScheduledExecutor::IsTerminated() {
    return terminated_;
}


void VepScheduledExecutor::TouchThreadPool() {
    if (thread_pool_ == nullptr) {
        thread_pool_ = std::unique_ptr<ThreadPool>(new ThreadPool(thread_pool_num_));
        cur_dispatch_key_ = 0;
        terminated_ = false;
    }
}
