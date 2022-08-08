//
// Created by 吴婷 on 2020-06-15.
//

#ifndef LIBVNS_SCHEDULED_EXECUTOR_H
#define LIBVNS_SCHEDULED_EXECUTOR_H

#include <memory>

#include "thirdparty/monitor_sdk_common/base/functional.h"
#include "thirdparty/monitor_sdk_common/system/threading/thread_pool.h"

namespace common {
/**
 * 调度类
 */
class ScheduledExecutor {
public:
    explicit ScheduledExecutor(int pool_size);
    ~ScheduledExecutor();

    // 瞬时执行
    void Schedule(const std::function<void(bool *)> &callback, int initial_delay = 0);

    // 定时执行，间隔单位:毫秒
    void ScheduleWithFixedDelay(const std::function<void(bool *)> &callback,
                                int initial_delay, int scheduled_interval);

    // 不定时执行，间隔单位:毫秒
    void ScheduleWithDynamicDelay(const std::function<void(bool *)> &callback,
                                  int initial_delay, const int *next_delay);

    // 关闭
    void Shutdown();

    // 重启
    void Restart();

    bool IsTerminated();

private:
    void TouchThreadPool();

    std::unique_ptr<ThreadPool> thread_pool_;
    std::unique_ptr<ThreadPool> reserve_thread_pool_;
    int cur_dispatch_key_;
    int cur_reserve_dispatch_key_;
    bool terminated_ = false;
    int pool_size_;

};

} //namespace common

#endif //LIBVNS_SCHEDULED_EXECUTOR_H
