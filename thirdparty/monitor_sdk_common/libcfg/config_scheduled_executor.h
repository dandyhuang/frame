//
// Created by 黄伟锋 on 2019-04-16.
//

#ifndef COMMON_LIBCFG_SCHEDULED_EXECUTOR_H_
#define COMMON_LIBCFG_SCHEDULED_EXECUTOR_H_

#include <memory>

#include "thirdparty/monitor_sdk_common/base/functional.h"
#include "thirdparty/monitor_sdk_common/system/threading/thread_pool.h"

namespace common {

class ConfigScheduledExecutor {
 public:
  ConfigScheduledExecutor();
  explicit ConfigScheduledExecutor(int thread_pool_num);
  ~ConfigScheduledExecutor();

  // 背景线程执行
  void Execute(const std::function<void (bool *)>& callback);

  // 定时执行，间隔单位:毫秒 (10ms 误差)
  void ScheduleWithFixedDelay(const std::function<void (bool *)>& callback, int initial_delay, int scheduled_interval);

  // 关闭
  void Shutdown();

  // 重启
  void Restart();

  bool IsTerminated();

 private:
  void TouchThreadPool();

  std::unique_ptr<ThreadPool> thread_pool_ = nullptr;
  int cur_dispatch_key_ = 0;

  int thread_pool_num_ = 4;

  bool terminated_ = false;
};

} // namespace common

#endif //COMMON_LIBCFG_SCHEDULED_EXECUTOR_H_
