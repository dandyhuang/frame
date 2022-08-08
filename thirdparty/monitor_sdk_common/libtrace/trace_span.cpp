//
// Created by 王锦涛 on 2020-2-12.
//

#include <iostream>

#include "thirdparty/monitor_sdk_common/libtrace/trace_span.h"

using namespace common;

TraceSpan::TraceSpan(TraceTracer * tracer, const std::string & span_id) {
    tracer_ = tracer;
    SetTraceId(tracer_->trace_id_);
    SetId(span_id);
    SetSdkVer(tracer_->sdk_ver_);
    SetType("CLIENT");
    SetComponent("Default Component");
    SetMethodName("Default Method Name");
    SetServiceName("Default Service Name");
    SetDomain("Default Domain");
    SetStatus("SUCCESS");
    SetSize(0);
    SetAttachInfo(tracer_->app_name_, tracer_->loc_, tracer_->env_);
    annotations_.clear();
    tags_.clear();
    SetEndPoint(tracer_->ip_, 0, tracer_->ip_, 0);
}

TraceSpan::~TraceSpan() {
    std::cout << "Delete span " << id_ << std::endl;
}

void TraceSpan::SetTraceId(const std::string & trace_id) {
    trace_id_ = trace_id;
}

void TraceSpan::SetId(const std::string & id) {
    id_ = id;
}

void TraceSpan::SetSdkVer(const std::string & sdk_ver) {
    sdk_ver_ = sdk_ver;
}

void TraceSpan::SetStart() {
    int64_t start = TraceTime::Instance()->GetTimeMS();
    std::cout << "span " << id_ << " start at " << start << std::endl;
    start_ = start;
}

void TraceSpan::SetEnd() {
    int64_t end = TraceTime::Instance()->GetTimeMS();
    std::cout << "span " << id_ << " end at " << end << std::endl;
    end_ = end;
}

void TraceSpan::SetType(const std::string & type) {
    type_ = type;
}

void TraceSpan::SetComponent(const std::string & component) {
    component_ = component;
}

void TraceSpan::SetMethodName(const std::string & method_name) {
    method_name_ = method_name;
}

void TraceSpan::SetServiceName(const std::string & service_name) {
    service_name_ = service_name;
}

void TraceSpan::SetDomain(const std::string & domain) {
    domain_ = domain;
}

void TraceSpan::SetSign(const std::string & sign) {
    sign_ = sign;
}

void TraceSpan::SetStatus(const std::string & status) {
    status_ = status;
}

void TraceSpan::SetSize(const int32_t & size) {
    size_ = size;
}

void TraceSpan::SetAttachInfo(const std::string & app_name,
                              const std::string & loc,
                              const std::string & env) {
    app_name_ = app_name;
    loc_ = loc;
    env_ = env;
}

void TraceSpan::SetAnnotation(const std::string & type,
                              const int64_t & timestamp,
                              const std::string & event,
                              const std::string & ex_name) {
    std::tuple<std::string, int64_t, std::string, std::string> annotation(type, timestamp, event, ex_name);
    annotations_.push_back(annotation);
}

void TraceSpan::SetTag(const std::string & key,
                       const std::string & value) {
    tags_[key] = value;
}

void TraceSpan::SetEndPoint(const std::string & sip,
                            const int32_t & sport,
                            const std::string & dip,
                            const int32_t & dport) {
    sip_ = sip;
    sport_ = sport;
    dip_ = dip;
    dport_ = dport;
}