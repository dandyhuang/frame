//
// Created by 黄伟锋 on 2019-04-22.
//

#include "thirdparty/monitor_sdk_common/librmq/rmq_scheduled_executor.h"
#include "thirdparty/monitor_sdk_common/system/threading/this_thread.h"

using namespace common;


RmqScheduledExecutor::RmqScheduledExecutor(int pool_size) {
  pool_size_ = pool_size;
  TouchThreadPool();
}


RmqScheduledExecutor::~RmqScheduledExecutor() {
  Shutdown();
}


void RmqScheduledExecutor::Schedule(const std::function<void (bool *)>& callback) {
  TouchThreadPool();

  if (terminated_) return;

  auto task_func = [this, callback] {
    callback(&this->terminated_);
  };

  reserve_thread_pool_->AddTask(task_func, cur_reserve_dispatch_key_ ++);
  cur_reserve_dispatch_key_ = cur_reserve_dispatch_key_ % pool_size_;
}


void RmqScheduledExecutor::ScheduleWithFixedDelay(const std::function<void (bool *)>& callback, int initial_delay, int scheduled_interval) {
  TouchThreadPool();

  if (terminated_) return;
  if (cur_dispatch_key_ >= pool_size_) return;

  auto task_func = [this, callback, initial_delay, scheduled_interval] {
    ThisThread::Sleep(initial_delay);

    while (true) {
      if (this->terminated_) break;
      callback(&this->terminated_);
      if (this->terminated_) break;
      ThisThread::Sleep(scheduled_interval);
    }
  };

  thread_pool_->AddTask(task_func, cur_dispatch_key_);
  cur_dispatch_key_ ++;
}


void RmqScheduledExecutor::ScheduleWithDynamicDelay(const std::function<void (bool *)>& callback, int initial_delay, int *next_delay) {
  if (terminated_) return;
  if (cur_dispatch_key_ >= pool_size_) return;

  auto task_func = [this, callback, initial_delay, next_delay] {
    ThisThread::Sleep(initial_delay);

    while (true) {
      if (this->terminated_) break;
      callback(&this->terminated_);
      if (this->terminated_) break;
      ThisThread::Sleep(*next_delay);
    }
  };

  thread_pool_->AddTask(task_func, cur_dispatch_key_);
  cur_dispatch_key_ ++;
}


void RmqScheduledExecutor::Shutdown() {
  terminated_ = true;
  thread_pool_->Terminate();
  thread_pool_ = nullptr;
  cur_dispatch_key_ = 0;
}


void RmqScheduledExecutor::Restart() {
  Shutdown();
  thread_pool_ = nullptr;
  TouchThreadPool();
}


void RmqScheduledExecutor::TouchThreadPool() {
  if (thread_pool_ == nullptr) {
    thread_pool_ = std::unique_ptr<ThreadPool>(new ThreadPool(pool_size_));
    reserve_thread_pool_ = std::unique_ptr<ThreadPool>(new ThreadPool(pool_size_));
    cur_dispatch_key_ = 0;
    cur_reserve_dispatch_key_ = 0;
    terminated_ = false;
  }
}
