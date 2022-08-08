//
// Created by 王锦涛 on 2020-2-12.
//

#ifndef COMMON_LIBTRACE_TRACE_REPORTER_H
#define COMMON_LIBTRACE_TRACE_REPORTER_H

#include <memory>
#include <iostream>
#include <string>
#include <vector>

#include "thirdparty/monitor_sdk_common/libtrace/trace_metric.h"
#include "thirdparty/monitor_sdk_common/libtrace/trace_scheduled_executor.h"
#include "thirdparty/monitor_sdk_common/libtrace/trace_span.h"
#include "thirdparty/monitor_sdk_common/libtrace/trace_tracer.h"

// ？？？？？？？？？？？？？？？？？？？？？
#include "thirdparty/monitor_sdk_common/libtrace/trace_thrift.h"


namespace common {

class TraceReporter{
public:
    TraceReporter();

    ~TraceReporter();

    /**
     * 开启定时上报器
     */
    bool Start();

private:
    bool ReportSpans();

    /**
     * 开启span定时上报器
     */
    bool StartSpanReporter();

    /**
     * 开启metric定时上报器
     */
    bool StartMetricReporter();


    //std::shared_ptr<ThriftClient>   thrift_client_;

    // Span定时执行器
    std::shared_ptr<TraceScheduledExecutor> span_scheduled_executor_;

    // Metric定时执行器
    std::shared_ptr<TraceScheduledExecutor> metric_scheduled_executor_;

    // 初始睡眠时间，单位:毫秒
    int init_sleep_time_;

    // span上报间隔，单位:毫秒
    int span_report_interval_;

    // metric上报间隔，单位:毫秒
    int metric_report_interval_;

    // 上次上报span时间，单位:毫秒
    int64_t last_report_span_time_;

    // 上次上报metirc时间，单位:毫秒
    int64_t last_report_metric_time_;
};

} //namespace common
#endif //COMMON_LIBTRACE_TRACE_REPORTER_H