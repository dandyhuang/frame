//
// Created by 黄伟锋 on 2019-04-16.
//

#include "thirdparty/monitor_sdk_common/libcfg/config_utils.h"
#include "thirdparty/monitor_sdk_common/libcfg/config_scheduled_executor.h"
#include "thirdparty/monitor_sdk_common/system/threading/this_thread.h"

#include "thirdparty/glog/logging.h"

using namespace common;

ConfigScheduledExecutor::ConfigScheduledExecutor() {
  TouchThreadPool();
}

ConfigScheduledExecutor::ConfigScheduledExecutor(int thread_pool_num) {
    thread_pool_num_ = thread_pool_num;
    TouchThreadPool();
}

ConfigScheduledExecutor::~ConfigScheduledExecutor() {
  Shutdown();
}

void ConfigScheduledExecutor::Execute(const std::function<void (bool *)>& callback) {
  TouchThreadPool();

  auto task_func = [this, callback] {
    if (this->terminated_) return;
    callback(&this->terminated_);
  };

  thread_pool_->AddTask(task_func, cur_dispatch_key_ ++);
  cur_dispatch_key_ = cur_dispatch_key_ % thread_pool_num_;
}

void ConfigScheduledExecutor::ScheduleWithFixedDelay(const std::function<void (bool *)>& callback, int initial_delay, int scheduled_interval) {
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

void ConfigScheduledExecutor::Shutdown() {
  terminated_ = true;
  thread_pool_->Terminate();
  thread_pool_ = nullptr;
  cur_dispatch_key_ = 0;
}

void ConfigScheduledExecutor::Restart() {
  Shutdown();
  thread_pool_ = nullptr;
  TouchThreadPool();
}

bool ConfigScheduledExecutor::IsTerminated() {
  return terminated_;
}

void ConfigScheduledExecutor::TouchThreadPool() {
  if (thread_pool_ == nullptr) {
    thread_pool_ = std::unique_ptr<ThreadPool>(new ThreadPool(thread_pool_num_));
    cur_dispatch_key_ = 0;
    terminated_ = false;
  }
}