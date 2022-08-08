//
// Created by 王锦涛 on 2020-2-12.
//

#include "thirdparty/monitor_sdk_common/libtrace/trace_manager.h"
#include "thirdparty/monitor_sdk_common/libtrace/trace_reporter.h"

using namespace common;

TraceReporter::TraceReporter() {
    std::cout << __FUNCTION__ << std::endl;
    //thrift_client_              = std::make_shared<ThriftClient>();
    span_scheduled_executor_    = std::make_shared<TraceScheduledExecutor>();
    metric_scheduled_executor_  = std::make_shared<TraceScheduledExecutor>();
    init_sleep_time_            = 10000;   //10秒
    span_report_interval_       = 10000;   //10秒
    metric_report_interval_     = 60000;   //60秒
    last_report_span_time_      = 0;
    last_report_metric_time_    = 0;
}

TraceReporter::~TraceReporter() {
    std::cout << __FUNCTION__ << std::endl;
}

bool TraceReporter::Start() {
    std::cout << __FUNCTION__ << std::endl;

    StartSpanReporter();

    StartMetricReporter();

    return true;
}

bool TraceReporter::StartSpanReporter() {
    std::cout << __FUNCTION__ << std::endl;

    span_scheduled_executor_->ScheduleWithFixedDelay([this] (bool * terminated) {

        if ((*terminated)) return;

        std::vector<std::shared_ptr<TraceTracer>> tracers = TraceManager::Instance()->PassTracers();
        std::cout << "size of tracers is: " << tracers.size() << std::endl;
        //thrift_client_->SendSpan(tracers);

    }, init_sleep_time_, span_report_interval_);
    
    return true;
}

bool TraceReporter::StartMetricReporter() {
    std::cout << __FUNCTION__ << std::endl;

    span_scheduled_executor_->ScheduleWithFixedDelay([this] (bool * terminated) {

        if ((*terminated)) return;

        std::cout << "do StartMetricReporter " << std::endl;

    }, init_sleep_time_, metric_report_interval_);
    
    return true;
}