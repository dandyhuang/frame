//
// Created by 王锦涛 on 2020-2-12.
//

#ifndef COMMON_LIBTRACE_TRACE_SCHEDULED_EXECUTOR_H
#define COMMON_LIBTRACE_TRACE_SCHEDULED_EXECUTOR_H

#include <memory>

#include "thirdparty/monitor_sdk_common/base/functional.h"
#include "thirdparty/monitor_sdk_common/system/threading/thread_pool.h"

namespace common {

class TraceScheduledExecutor {
public:
    TraceScheduledExecutor();
    ~TraceScheduledExecutor();

    // 背景线程执行
    void Execute(const std::function<void (bool *)>& callback);

    // 定时执行，间隔单位:毫秒 (10ms 误差)
    void ScheduleWithFixedDelay(const std::function<void (bool *)>& callback, int initial_delay, int scheduled_interval);

    // 关闭
    void Shutdown();

    // 重启
    void Restart();

    //是否结束
    bool IsTerminated();

private:
    void TouchThreadPool();

    std::unique_ptr<ThreadPool> thread_pool_ = nullptr;

    int cur_dispatch_key_ = 0;

    int thread_pool_num_ = 10;

    bool terminated_ = false;
};

} //namespace common
#endif //COMMON_LIBTRACE_TRACE_SCHEDULED_EXECUTOR_H