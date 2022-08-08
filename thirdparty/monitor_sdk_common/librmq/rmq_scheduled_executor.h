//
// Created by 黄伟锋 on 2019-04-22.
//

#ifndef COMMON_LIBRMQ_SCHEDULED_EXECUTOR_H_
#define COMMON_LIBRMQ_SCHEDULED_EXECUTOR_H_

#include <memory>

#include "thirdparty/monitor_sdk_common/base/functional.h"
#include "thirdparty/monitor_sdk_common/system/threading/thread_pool.h"


namespace common {

class RmqScheduledExecutor {
 public:
  RmqScheduledExecutor(int pool_size);
  ~RmqScheduledExecutor();

  // 瞬时执行
  void Schedule(const std::function<void (bool *)>& callback);
  // 定时执行，间隔单位:毫秒
  void ScheduleWithFixedDelay(const std::function<void (bool *)>& callback, int initial_delay, int scheduled_interval);
  // 不定时执行，间隔单位:毫秒
  void ScheduleWithDynamicDelay(const std::function<void (bool *)>& callback, int initial_delay, int *next_delay);

  // 关闭
  void Shutdown();

  // 重启
  void Restart();

 protected:
  RmqScheduledExecutor() {}

 private:
  void TouchThreadPool();

  std::unique_ptr<ThreadPool> thread_pool_;
  std::unique_ptr<ThreadPool> reserve_thread_pool_;
  int cur_dispatch_key_;
  int cur_reserve_dispatch_key_;
  bool terminated_;
  int pool_size_;
};

} // namespace common


#endif //COMMON_LIBRMQ_SCHEDULED_EXECUTOR_H_
