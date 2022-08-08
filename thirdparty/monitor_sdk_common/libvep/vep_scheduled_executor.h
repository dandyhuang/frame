//
// Created by wuting
//

#ifndef COMMON_LIBVEP_SCHEDULED_EXECUTOR_H_
#define COMMON_LIBVEP_SCHEDULED_EXECUTOR_H_

#include <memory>

#include "thirdparty/monitor_sdk_common/base/functional.h"
#include "thirdparty/monitor_sdk_common/system/threading/thread_pool.h"


namespace common {

class VepScheduledExecutor {
 public:
    VepScheduledExecutor();
    explicit VepScheduledExecutor(int thread_pool_num);
    ~VepScheduledExecutor();

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

private:
    std::unique_ptr<ThreadPool> thread_pool_ = nullptr;  //线程池

    int cur_dispatch_key_ = 0;   //当前分派的key

    int thread_pool_num_ = 10;   //池大小

    bool terminated_ = false;   //是否终止
};

} // namespace common

#endif //COMMON_LIBVEP_SCHEDULED_EXECUTOR_H_
