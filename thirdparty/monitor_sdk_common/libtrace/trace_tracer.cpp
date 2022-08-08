//
// Created by 王锦涛 on 2020-2-12.
//

#include <iostream>

#include "thirdparty/monitor_sdk_common/libtrace/trace_tracer.h"

using namespace common;

TraceTracer::TraceTracer(const float & sample_rate,
                         const std::string & app_name,
                         const std::string & loc,
                         const std::string & env,
                         const std::string & sdk_ver,
                         const std::string & ip) {
    sample_rate_    = sample_rate;
    app_name_       = app_name;
    loc_            = loc;
    env_            = env;
    sdk_ver_        = sdk_ver;
    ip_             = ip;
    trace_id_       = TraceID::Instance()->GenTraceId(env, ip);
    std::cout << "trace_id: " << trace_id_ << std::endl;
}

TraceTracer::~TraceTracer() {
    std::cout << "Delete tracer " << trace_id_ << std::endl;
}

std::shared_ptr<TraceSpan> TraceTracer::StartSpan(const std::string & span_id) {
    std::shared_ptr<TraceSpan> span = std::make_shared<TraceSpan>(this, span_id);
    span->SetStart();
    return span;
}

void TraceTracer::EndSpan(std::shared_ptr<TraceSpan> span) {
    span->SetEnd();
    spans_.push_back(span);
}

void TraceTracer::SetSampleRate(const float & sample_rate) {
    sample_rate_ = sample_rate;
}